#include "RenderStreamingSignallingConnection.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRenderStreamingSS, Log, All);
DEFINE_LOG_CATEGORY(LogRenderStreamingSS);

FRenderStreamingSignallingConnection::FRenderStreamingSignallingConnection(TSharedPtr<IPixelStreamingSignallingConnectionObserver> Observer)
    : Observer(Observer)
{
    if (GWorld && !GWorld->GetTimerManager().IsTimerActive(TimerHandle))
    {
        GWorld->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateRaw(this, &FRenderStreamingSignallingConnection::KeepAlive), 5.0f, true);
    }
}

void FRenderStreamingSignallingConnection::TryConnect(FString URL)
{
    Channel = grpc::CreateChannel(std::string(TCHAR_TO_UTF8(*URL)), grpc::InsecureChannelCredentials());
    Stub = SignallingService::NewStub(Channel);
}

void FRenderStreamingSignallingConnection::Disconnect()
{
}

bool FRenderStreamingSignallingConnection::IsConnected() const
{
    return false;
}

void FRenderStreamingSignallingConnection::SendOffer(FPixelStreamingPlayerId PlayerId, const webrtc::SessionDescriptionInterface &SDP)
{
    if (!Stub)
    {
        return;
    }

    OfferMsg Request;
    Request.set_playerid(std::string(TCHAR_TO_UTF8(*PlayerId)));

    std::string SdpAnsi;
    SDP.ToString(&SdpAnsi);
    Request.set_sdp(SdpAnsi);

    Empty Response;
    grpc::ClientContext ClientContext;
    grpc::Status Status;
    Status = Stub->Offer(&ClientContext, Request, &Response);

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

void FRenderStreamingSignallingConnection::KeepAlive()
{
    UE_LOG(LogRenderStreamingSS, Warning, TEXT("Sending ping"));

    if (!Stub)
    {
        return;
    }

    grpc::ClientContext ClientContext;
    PingMsg Request;
    Request.set_message("Hello");
    PongMsg Response;
    grpc::Status Status = Stub->Ping(&ClientContext, Request, &Response);

    if (!Status.ok())
    {
        FString ErrorString(Status.error_message().c_str());
        UE_LOG(LogRenderStreamingSS, Warning, TEXT("SendOffer failed: %d, %s"), Status.error_code(), *ErrorString);
    }
    else
    {
        FString PongString(Response.message().c_str());
        UE_LOG(LogRenderStreamingSS, Warning, TEXT("Received Pong: %s"), *PongString);
    }
}