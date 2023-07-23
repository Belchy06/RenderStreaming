#include "RenderStreamingStreamer.h"
#include "RenderStreamingPrivate.h"

FRenderStreamingStreamer::FRenderStreamingStreamer()
{
    this->SignallingObserver = MakeShared<FRenderStreamingSignallingConnectionObserver>();
    this->SignallingConnection = MakeShared<FRenderStreamingSignallingConnection>(SignallingObserver);

    SignallingObserver->OnSignallingConnected = [this]()
    {
        SignallingConnection->RequestConfig();
    };

    SignallingObserver->OnSignallingConfig = [this](const webrtc::PeerConnectionInterface::RTCConfiguration &Config)
    {
        this->PeerConnectionConfig = Config;
        // We want periodic bandwidth probing so ramping happens quickly
        this->PeerConnectionConfig.media_config.video.periodic_alr_bandwidth_probing = true;
    };

    SignallingObserver->OnSignallingSessionDescription = [this](FPixelStreamingPlayerId PlayerId, webrtc::SdpType Type, const FString &Sdp)
    {
        // UE_LOG(LogRenderStreaming, Warning, TEXT("Received offer: %s"), *Sdp);
    };

    SignallingObserver->OnSignallingPlayerConnected = [this](FPixelStreamingPlayerId PlayerId, const FPixelStreamingPlayerConfig &PlayerConfig, bool bSendOffer)
    {
        this->OnPlayerConnected(PlayerId, PlayerConfig, bSendOffer);
    };
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

    TUniquePtr<FRenderStreamingPeerConnection> PeerConnection = FRenderStreamingPeerConnection::Create(this->PeerConnectionConfig);
    Peers.Add(InPlayerId, MakeShared<FRenderStreamingPeer>(MakeShareable(PeerConnection.Release())));
}