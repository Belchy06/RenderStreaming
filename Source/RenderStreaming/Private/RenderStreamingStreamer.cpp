#include "RenderStreamingStreamer.h"
#include "RenderStreamingPrivate.h"

FRenderStreamingStreamer::FRenderStreamingStreamer()
{
    SignallingObserver = MakeShared<FRenderStreamingSignallingConnectionObserver>();
    SignallingConnection = MakeShared<FRenderStreamingSignallingConnection>(SignallingObserver);
    PeerConnectionConfig = webrtc::PeerConnectionInterface::RTCConfiguration();

    SignallingObserver->OnSignallingConnected = [this]()
    {
        SignallingConnection->RequestConfig();
    };

    SignallingObserver->OnSignallingConfig = [this](const webrtc::PeerConnectionInterface::RTCConfiguration &Config)
    {
        PeerConnectionConfig = Config;
        // We want periodic bandwidth probing so ramping happens quickly
        PeerConnectionConfig.media_config.video.periodic_alr_bandwidth_probing = true;

        SignallingConnection->SubscribeToPlayerConnected();
    };

    SignallingObserver->OnSignallingPlayerConnected = [this](FPixelStreamingPlayerId PlayerId, const FPixelStreamingPlayerConfig &PlayerConfig, bool bSendOffer)
    {
        OnPlayerConnected(PlayerId, PlayerConfig, bSendOffer);
    };

    SignallingObserver->OnSignallingSessionDescription = [this](FPixelStreamingPlayerId PlayerId, webrtc::SdpType Type, const FString &Sdp)
    {
        // UE_LOG(LogRenderStreaming, Warning, TEXT("Received offer: %s"), *Sdp);
    };

    SignallingConnection->TryConnect("localhost:8080");
}

FRenderStreamingStreamer::~FRenderStreamingStreamer()
{
    SignallingConnection->Disconnect();
}

void FRenderStreamingStreamer::OnPlayerConnected(FPixelStreamingPlayerId InPlayerId, const FPixelStreamingPlayerConfig &InPlayerConfig, bool InbSendOffer)
{
    if (!InbSendOffer)
    {
        // If we're not sending the offer, don't create the player session
        // we'll wait until the offer arrives to do that
        return;
    }

    TUniquePtr<FRenderStreamingPeerConnection> PeerConnection = FRenderStreamingPeerConnection::Create(PeerConnectionConfig);
    Peers.Add(InPlayerId, MakeShared<FRenderStreamingPeer>(MakeShareable(PeerConnection.Release())));
}