#pragma once

#include "rtc_base/ssl_adapter.h"
#include "api/create_peerconnection_factory.h"

/**
 * A specialized representation of a WebRTC peer connection for Render Streaming
 */
class FRenderStreamingPeerConnection : public webrtc::PeerConnectionObserver
{
public:
    /**
     * Creates a new peer connection using the given RTC configuration.
     * @param Config The RTC configuration for the connection. Usually provided from the signalling server.
     * @param IsSFU Set to true to create a peer connection to an SFU. Controls whether it recieves simulcast and some audio settings are affected.
     * @returns A new peer connection ready to be used.
     */
    static TUniquePtr<FRenderStreamingPeerConnection> Create(const webrtc::PeerConnectionInterface::RTCConfiguration &RTCConfig, bool IsSFU = false);

    using VoidCallback = TFunction<void()>;
    using SDPCallback = TFunction<void(const webrtc::SessionDescriptionInterface *)>;
    using ErrorCallback = TFunction<void(const FString &Error)>;

    /**
     * Flags for CreateOffer/CreateAnswer that indicate what kind of media we indend to receive. Dictates what the resulting
     * SDP will contain. To actually receive audio or video you will need to supply sinks via SetAudioSink/SetVideoSink
     */
    enum EReceiveMediaOption : int
    {
        // Receive no media
        Nothing = 0x00,

        // Can receive audio
        Audio = 0x01,

        // Can receive video
        Video = 0x02,

        // Can receive both audio and video
        All = Audio | Video,
    };

    /**
     * Asynchronously creates an offer session description for this local peer connection.
     * @param ReceiveOption Indicates what kind of media this connection intends to receive.
     * @param SuccessCallback A callback to be called when the offer is ready.
     * @param ErrorCallback A callback to be called on the event of an error.
     */
    // void CreateOffer(EReceiveMediaOption ReceiveOption, const SDPCallback &SuccessCallback, const ErrorCallback &ErrorCallback);

    /**
     * Asynchronously creates an answer session description for this local peer connection.
     * @param ReceiveOption Indicates what kind of media this connection intends to receive.
     * @param SuccessCallback A callback to be called when the offer is ready.
     * @param ErrorCallback A callback to be called on the event of an error.
     */
    // void CreateAnswer(EReceiveMediaOption ReceiveOption, const SDPCallback &SuccessCallback, const ErrorCallback &ErrorCallback);

    /**
     * Asynchronously sets the remote description from a session description string offered from a signalling server.
     * @param SDP The string representation of the remote session description.
     * @param SuccessCallback A callback to be called when the remote description was successfully set.
     * @param ErrorCallback A callback to be called on the event of an error.
     */
    // void ReceiveOffer(const FString &SDP, const VoidCallback &SuccessCallback, const ErrorCallback &ErrorCallback);

    /**
     * Asynchronously sets the remote description from a session description string answered from a signalling server.
     * @param SDP The string representation of the remote session description.
     * @param SuccessCallback A callback to be called when the remote description was successfully set.
     * @param ErrorCallback A callback to be called on the event of an error.
     */
    // void ReceiveAnswer(const FString &SDP, const VoidCallback &SuccessCallback, const ErrorCallback &ErrorCallback);

    /**
     * Adds ICE candidate data to the peer connection. Usually supplied from the signalling server.
     * @param SDPMid
     * @param SDPMLineIndex
     * @param SDP
     */
    // void AddRemoteIceCandidate(const FString &SDPMid, int SDPMLineIndex, const FString &SDP);

    /**
     * Called when the ICE candidate is emitted.
     */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnIceCandidate, const webrtc::IceCandidateInterface *);
    FOnIceCandidate OnEmitIceCandidate;

    /**
     * Called when the ICE state changes.
     */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnIceStateChanged, webrtc::PeerConnectionInterface::IceConnectionState);
    FOnIceStateChanged OnIceStateChanged;

    /**
     * Post a task onto the WebRTC signalling thread.
     */
    template <typename FunctorT>
    static void PostSignalingTask(FunctorT &&InFunc)
    {
        // Someone may accidentally call this static function without calling FRenderStreamingPeerConnection::Create first
        if (SignallingThread != nullptr)
        {
            SignallingThread->PostTask(RTC_FROM_HERE, Forward<FunctorT>(InFunc));
        }
    }

protected:
    //
    // webrtc::PeerConnectionObserver implementation.
    //
    virtual void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState NewState) override;
    virtual void OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> Stream) override;
    virtual void OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> Stream) override;
    virtual void OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> Channel) override;
    virtual void OnRenegotiationNeeded() override;
    virtual void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState NewState) override;
    virtual void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState NewState) override;
    virtual void OnIceCandidate(const webrtc::IceCandidateInterface *Candidate) override;
    virtual void OnIceCandidatesRemoved(const std::vector<cricket::Candidate> &candidates) override;
    virtual void OnIceConnectionReceivingChange(bool Receiving) override;
    virtual void OnTrack(rtc::scoped_refptr<webrtc::RtpTransceiverInterface> Transceiver) override;
    virtual void OnRemoveTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver) override;

private:
    FRenderStreamingPeerConnection() = default;

    enum class ESDPType
    {
        Offer,
        Answer,
    };

    // void CreateSDP(ESDPType SDPType, EReceiveMediaOption ReceiveOption, const SDPCallback &SuccessCallback, const ErrorCallback &ErrorCallback);

    rtc::scoped_refptr<webrtc::PeerConnectionInterface> PeerConnection;

    // TODO these static methods can probably be moved off into a singleton or something
    static void CreatePeerConnectionFactory();
    static TUniquePtr<rtc::Thread> SignallingThread;
    static rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> PeerConnectionFactory;
};
