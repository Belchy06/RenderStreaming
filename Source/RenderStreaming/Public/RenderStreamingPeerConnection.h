#pragma once

#include "PixelStreamingPeerConnection.h"

class FRenderStreamingPeerConnection : public FPixelStreamingPeerConnection
{
public:
    FRenderStreamingPeerConnection();

    static TUniquePtr<FRenderStreamingPeerConnection> Create(const FRTCConfig &RTCConfig, bool IsSFU = false);
};