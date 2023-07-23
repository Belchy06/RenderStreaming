#pragma once

#include "RenderStreamingPeerConnection.h"

#include "rtc_base/ssl_adapter.h"
#include "api/create_peerconnection_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/video_codecs/builtin_video_encoder_factory.h"
#include "api/video_codecs/builtin_video_decoder_factory.h"

#include "PixelStreamingPlayerId.h"

class FRenderStreamingPeer
{
public:
    FRenderStreamingPeer(TSharedPtr<FRenderStreamingPeerConnection> InPeerConnection);

private:
    // WebRTC's clunky observer pattern that is fired when an SDP offer/answer is done being created.
    class FCreateSDPObserver : public webrtc::CreateSessionDescriptionObserver
    {
    public:
        DECLARE_MULTICAST_DELEGATE_OneParam(FOnCreatedSDP, webrtc::SessionDescriptionInterface *);
        FOnCreatedSDP OnCreatedSDP;
        virtual void OnSuccess(webrtc::SessionDescriptionInterface *Desc) override { OnCreatedSDP.Broadcast(Desc); };

        DECLARE_MULTICAST_DELEGATE_OneParam(FOnCreatedSDPFailed, webrtc::RTCError);
        FOnCreatedSDPFailed OnFailedCreateSDP;
        virtual void OnFailure(webrtc::RTCError Error) override
        {
            OnFailedCreateSDP.Broadcast(Error);
        };
    };

    // WebRTC's clunky observer pattern that is fired when an SDP offer/answer is set on the PeerConnection
    class FSetSDPObserver : public webrtc::SetSessionDescriptionObserver
    {
    public:
        DECLARE_MULTICAST_DELEGATE(FOnSetSDP);
        FOnSetSDP OnSetSDP;
        virtual void OnSuccess() override { OnSetSDP.Broadcast(); }

        DECLARE_MULTICAST_DELEGATE_OneParam(FOnSetSDPFailed, webrtc::RTCError);
        FOnSetSDPFailed OnSetSDPFailed;
        virtual void OnFailure(webrtc::RTCError Error) override
        {
            OnSetSDPFailed.Broadcast(Error);
        }
    };

public:
    rtc::scoped_refptr<FSetSDPObserver> SetSDPObserver;
    rtc::scoped_refptr<FCreateSDPObserver> CreateSDPObserver;

private:
    TSharedPtr<FRenderStreamingPeerConnection> PeerConnection;
};