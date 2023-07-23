#include "RenderStreamingPeerConnection.h"
#include "NullAudioDeviceModule.h"

#include "rtc_base/internal/default_socket_server.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/video_codecs/builtin_video_encoder_factory.h"
#include "api/video_codecs/builtin_video_decoder_factory.h"

TUniquePtr<rtc::Thread> FRenderStreamingPeerConnection::SignallingThread = nullptr;
rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> FRenderStreamingPeerConnection::PeerConnectionFactory = nullptr;

TUniquePtr<FRenderStreamingPeerConnection> FRenderStreamingPeerConnection::Create(const webrtc::PeerConnectionInterface::RTCConfiguration &RTCConfig, bool IsSFU)
{
    if (!PeerConnectionFactory)
    {
        CreatePeerConnectionFactory();
    }

    TUniquePtr<FRenderStreamingPeerConnection> NewPeerConnection = TUniquePtr<FRenderStreamingPeerConnection>(new FRenderStreamingPeerConnection());

    webrtc::RTCErrorOr<rtc::scoped_refptr<webrtc::PeerConnectionInterface>> Result = PeerConnectionFactory->CreatePeerConnectionOrError(RTCConfig, webrtc::PeerConnectionDependencies(NewPeerConnection.Get()));
    checkf(Result.ok(), TEXT("Failed to create Peer Connection. Msg=%s"), *FString(Result.error().message()));
    NewPeerConnection->PeerConnection = Result.MoveValue();

    return NewPeerConnection;
}

void FRenderStreamingPeerConnection::CreatePeerConnectionFactory()
{
    SignallingThread = MakeUnique<rtc::Thread>(rtc::CreateDefaultSocketServer());
    SignallingThread->SetName("FRenderStreamingPeerConnection SignallingThread", nullptr);
    SignallingThread->Start();

    rtc::scoped_refptr<webrtc::AudioDeviceModule> AudioDeviceModule;
    AudioDeviceModule = new rtc::RefCountedObject<UE::RenderStreaming::FNullAudioDeviceModule>();

    PeerConnectionFactory = webrtc::CreatePeerConnectionFactory(
        nullptr /* network_thread */,
        nullptr /* worker_thread */,
        SignallingThread.Get(),
        AudioDeviceModule /* adm */,
        webrtc::CreateBuiltinAudioEncoderFactory(),
        webrtc::CreateBuiltinAudioDecoderFactory(),
        webrtc::CreateBuiltinVideoEncoderFactory(),
        webrtc::CreateBuiltinVideoDecoderFactory(),
        nullptr /* audio_mixer */,
        nullptr /* audio_processing */);
}

void FRenderStreamingPeerConnection::OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState NewState) {}
void FRenderStreamingPeerConnection::OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> Stream) {}
void FRenderStreamingPeerConnection::OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> Stream) {}
void FRenderStreamingPeerConnection::OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> Channel) {}
void FRenderStreamingPeerConnection::OnRenegotiationNeeded() {}
void FRenderStreamingPeerConnection::OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState NewState) {}
void FRenderStreamingPeerConnection::OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState NewState) {}
void FRenderStreamingPeerConnection::OnIceCandidate(const webrtc::IceCandidateInterface *Candidate) {}
void FRenderStreamingPeerConnection::OnIceCandidatesRemoved(const std::vector<cricket::Candidate> &candidates) {}
void FRenderStreamingPeerConnection::OnIceConnectionReceivingChange(bool Receiving) {}
void FRenderStreamingPeerConnection::OnTrack(rtc::scoped_refptr<webrtc::RtpTransceiverInterface> Transceiver) {}
void FRenderStreamingPeerConnection::OnRemoveTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver) {}