#include "RenderStreamingPeerConnection.h"

TUniquePtr<FRenderStreamingPeerConnection> FRenderStreamingPeerConnection::Create(const FRTCConfig &RTCConfig, bool IsSFU)
{
    return TUniquePtr<FRenderStreamingPeerConnection>(static_cast<FRenderStreamingPeerConnection *>(FPixelStreamingPeerConnection::Create(RTCConfig, IsSFU).Release()));
}