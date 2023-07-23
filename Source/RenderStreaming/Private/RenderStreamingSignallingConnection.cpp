#include "RenderStreamingSignallingConnection.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRenderStreamingSS, Log, All);
DEFINE_LOG_CATEGORY(LogRenderStreamingSS);

FRenderStreamingSignallingConnection::FRenderStreamingSignallingConnection(TSharedPtr<IPixelStreamingSignallingConnectionObserver> InObserver)
    : Observer(InObserver)
{
}

void FRenderStreamingSignallingConnection::TryConnect(FString URL)
{
    this->Channel = grpc::CreateChannel(std::string(TCHAR_TO_UTF8(*URL)), grpc::InsecureChannelCredentials());
    if (!this->Channel)
    {
        UE_LOG(LogRenderStreamingSS, Warning, TEXT("Failed to create signalling server channel"));
        return;
    }
    this->Stub = Signalling::NewStub(this->Channel);

    if (!this->Stub)
    {
        UE_LOG(LogRenderStreamingSS, Warning, TEXT("Failed to create signalling server stub"));
        return;
    }

    Empty Request, Response;
    grpc::ClientContext ClientContext;
    grpc::Status Status;
    Status = this->Stub->Connect(&ClientContext, Request, &Response);

    if (!Status.ok())
    {
        FString ErrorString(Status.error_message().c_str());
        UE_LOG(LogRenderStreamingSS, Warning, TEXT("Connect failed: %d, %s"), Status.error_code(), *ErrorString);
        return;
    }

    this->Observer->OnSignallingConnected();
}

void FRenderStreamingSignallingConnection::Disconnect()
{
}

bool FRenderStreamingSignallingConnection::IsConnected() const
{
    return this->Stub ? true : false;
}

void FRenderStreamingSignallingConnection::SendOffer(FPixelStreamingPlayerId PlayerId, const webrtc::SessionDescriptionInterface &SDP) { unimplemented(); }
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

        webrtc::PeerConnectionInterface::RTCConfiguration Config;

        // TODO (belchy06): Parse FString into a json object and parse that json in the same way PS does
        Config.servers.push_back(webrtc::PeerConnectionInterface::IceServer{});

        // force `UnifiedPlan` as we control both ends of WebRTC streaming
        Config.sdp_semantics = webrtc::SdpSemantics::kUnifiedPlan;

        Observer->OnSignallingConfig(Config);
    }
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