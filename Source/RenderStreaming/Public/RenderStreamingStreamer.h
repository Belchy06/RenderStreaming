#pragma once

#include "RenderStreamingSignallingConnection.h"
#include "RenderStreamingSignallingConnectionObserver.h"

#include "RenderStreamingPeer.h"

class FRenderStreamingStreamer
{
public:
    FRenderStreamingStreamer();
    ~FRenderStreamingStreamer();

    void OnPlayerConnected(FPixelStreamingPlayerId InPlayerId, const FPixelStreamingPlayerConfig &InPlayerConfig, bool InbSendOffer);

private:
    webrtc::PeerConnectionInterface::RTCConfiguration PeerConnectionConfig;

private:
    TMap<FPixelStreamingPlayerId, TSharedPtr<FRenderStreamingPeer>> Peers;
    TSharedPtr<FRenderStreamingSignallingConnectionObserver> SignallingObserver;
    TSharedPtr<FRenderStreamingSignallingConnection> SignallingConnection;
};