#include "RenderStreamingSignallingConnection.h"
#include "Json.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRenderStreamingSS, Log, All);
DEFINE_LOG_CATEGORY(LogRenderStreamingSS);

FRenderStreamingSignallingConnection::FRenderStreamingSignallingConnection(TSharedPtr<IPixelStreamingSignallingConnectionObserver> InObserver)
    : Observer(InObserver)
{
}

void FRenderStreamingSignallingConnection::TryConnect(FString URL)
{
    Channel = grpc::CreateChannel(std::string(TCHAR_TO_UTF8(*URL)), grpc::InsecureChannelCredentials());
    if (!Channel)
    {
        UE_LOG(LogRenderStreamingSS, Warning, TEXT("Failed to create signalling server channel"));
        return;
    }
    Stub = Signalling::NewStub(Channel);

    if (!Stub)
    {
        UE_LOG(LogRenderStreamingSS, Warning, TEXT("Failed to create signalling server stub"));
        return;
    }

    Streamer Request;
    Request.set_id("Streamer");
    Empty Response;
    grpc::ClientContext ClientContext;
    grpc::Status Status;
    Status = Stub->ConnectStreamer(&ClientContext, Request, &Response);

    if (!Status.ok())
    {
        FString ErrorString(Status.error_message().c_str());
        UE_LOG(LogRenderStreamingSS, Warning, TEXT("Connect failed: %d, %s"), Status.error_code(), *ErrorString);
        return;
    }

    Observer->OnSignallingConnected();
}

void FRenderStreamingSignallingConnection::Disconnect()
{
}

bool FRenderStreamingSignallingConnection::IsConnected() const
{
    return Stub ? true : false;
}

void FRenderStreamingSignallingConnection::SendOffer(FPixelStreamingPlayerId PlayerId, const webrtc::SessionDescriptionInterface &SDP)
{
    if (!Stub)
    {
        return;
    }

    grpc::ClientContext ClientContext;
    Offer Request;
    Request.set_id(std::string(TCHAR_TO_UTF8(*PlayerId)));
    std::string SdpAnsi;
    SDP.ToString(&SdpAnsi);
    Request.set_sdp(SdpAnsi);

    Empty Response;
    grpc::Status Status = Stub->SendOfferToPlayer(&ClientContext, Request, &Response);

    if (!Status.ok())
    {
        FString ErrorString(Status.error_message().c_str());
        UE_LOG(LogRenderStreamingSS, Warning, TEXT("SendOffer failed: %d, %s"), Status.error_code(), *ErrorString);
    }
}
void FRenderStreamingSignallingConnection::SendAnswer(FPixelStreamingPlayerId PlayerId, const webrtc::SessionDescriptionInterface &SDP) { unimplemented(); }
void FRenderStreamingSignallingConnection::SendIceCandidate(FPixelStreamingPlayerId PlayerId, const webrtc::IceCandidateInterface &IceCandidate) { unimplemented(); }
void FRenderStreamingSignallingConnection::SendDisconnectPlayer(FPixelStreamingPlayerId PlayerId, const FString &Reason) { unimplemented(); }

void FRenderStreamingSignallingConnection::SendOffer(const webrtc::SessionDescriptionInterface &SDP) { unimplemented(); }
void FRenderStreamingSignallingConnection::SendAnswer(const webrtc::SessionDescriptionInterface &SDP) { unimplemented(); }
void FRenderStreamingSignallingConnection::SendIceCandidate(const webrtc::IceCandidateInterface &IceCandidate) { unimplemented(); }

void FRenderStreamingSignallingConnection::SetKeepAlive(bool bKeepAlive) { unimplemented(); }
void FRenderStreamingSignallingConnection::SetAutoReconnect(bool bAutoReconnect) { unimplemented(); }

void FRenderStreamingSignallingConnection::SendMessage(const FString &Msg)
{
}

void FRenderStreamingSignallingConnection::RequestConfig()
{
    if (!Stub)
    {
        return;
    }

    grpc::ClientContext ClientContext;
    Empty Request;
    PeerConfig Response;
    grpc::Status Status = Stub->Config(&ClientContext, Request, &Response);

    if (!Status.ok())
    {
        FString ErrorString(Status.error_message().c_str());
        UE_LOG(LogRenderStreamingSS, Warning, TEXT("RequestConfig failed: %d, %s"), Status.error_code(), *ErrorString);
    }
    else
    {
        FString PeerConfigString(Response.peerconnectionoptions().c_str());

        TSharedPtr<FJsonObject> PeerConnectionOptions = MakeShareable(new FJsonObject());
        TSharedRef<TJsonReader<TCHAR>> Reader = TJsonReaderFactory<TCHAR>::Create(PeerConfigString);

        webrtc::PeerConnectionInterface::RTCConfiguration Config;

        const TArray<TSharedPtr<FJsonValue>> *IceServers = nullptr;
        if (PeerConnectionOptions->TryGetArrayField(TEXT("iceServers"), IceServers))
        {
            for (const TSharedPtr<FJsonValue> &IceServerVal : *IceServers)
            {
                const TSharedPtr<FJsonObject> *IceServerJson;
                if (!IceServerVal->TryGetObject(IceServerJson))
                {
                    UE_LOG(LogRenderStreamingSS, Error, TEXT("Failed to parse SS config: `iceServer` - not an object\n%s"), *PeerConfigString);
                    return;
                }

                Config.servers.push_back(webrtc::PeerConnectionInterface::IceServer{});
                webrtc::PeerConnectionInterface::IceServer &IceServer = Config.servers.back();

                TArray<FString> Urls;
                if ((*IceServerJson)->TryGetStringArrayField(TEXT("urls"), Urls))
                {
                    for (const FString &UrlElem : Urls)
                    {
                        IceServer.urls.push_back(TCHAR_TO_UTF8(*UrlElem));
                    }
                }
                else
                {
                    // in the RTC Spec, "urls" can be an array or a single string
                    // https://www.w3.org/TR/webrtc/#dictionary-rtciceserver-members
                    FString UrlsSingle;
                    if ((*IceServerJson)->TryGetStringField(TEXT("urls"), UrlsSingle))
                    {
                        IceServer.urls.push_back(TCHAR_TO_UTF8(*UrlsSingle));
                    }
                }
            }
        }

        // force `UnifiedPlan` as we control both ends of WebRTC streaming
        Config.sdp_semantics = webrtc::SdpSemantics::kUnifiedPlan;

        Observer->OnSignallingConfig(Config);
    }
}

void FRenderStreamingSignallingConnection::SubscribeToPlayerConnected()
{
    if (!Stub)
    {
        return;
    }

    // Post our subscriber to a new unnamed thead as it polls forever
    AsyncTask(ENamedThreads::AnyThread, [this]()
              { 
            grpc::ClientContext ClientContext;
            Empty Request;
            PlayerConnected Response;

            std::unique_ptr<grpc::ClientReader<PlayerConnected>> Reader(Stub->SubscribeToPlayerConnected(&ClientContext, Request));
            while (Reader->Read(&Response))
            {
                FPixelStreamingPlayerId PlayerId(Response.id().c_str());
                FPixelStreamingPlayerConfig PlayerConfig;

                UE_LOG(LogRenderStreamingSS, Warning, TEXT("Player \"%s\" connected"), *PlayerId);
                Observer->OnSignallingPlayerConnected(PlayerId, PlayerConfig, true);
            }

            grpc::Status Status = Reader->Finish();
            if (!Status.ok())
            {
                FString ErrorString(Status.error_message().c_str());
                UE_LOG(LogRenderStreamingSS, Warning, TEXT("SubscribeToPlayerConnected failed: %d, %s"), Status.error_code(), *ErrorString);
                return;
            } });
}
/**
void FRenderStreamingSignallingConnection::WaitForPlayerOffer()
{
    if (!Stub)
    {
        return;
    }

    grpc::ClientContext ClientContext;
    Empty Request;
    Offer Response;

    std::unique_ptr<grpc::ClientReader<Offer>> Reader(Stub->WaitForPlayer(&ClientContext, Request));
    while (Reader->Read(&Response))
    {
        FString SdpString(Response.sdp().c_str());
        Observer->OnSignallingSessionDescription("", webrtc::SdpType::kOffer, SdpString);
        UE_LOG(LogRenderStreamingSS, Warning, TEXT("Received offer"));
    }

    grpc::Status Status = Reader->Finish();
    if (!Status.ok())
    {
        FString ErrorString(Status.error_message().c_str());
        UE_LOG(LogRenderStreamingSS, Warning, TEXT("WaitForPlayer failed: %d, %s"), Status.error_code(), *ErrorString);
        return;
    }
}
*/