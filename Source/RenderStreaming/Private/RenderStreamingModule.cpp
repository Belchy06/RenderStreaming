#include "RenderStreamingModule.h"
#include "NullAudioDeviceModule.h"
#include "rtc_base/ssl_adapter.h"
#include "api/create_peerconnection_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/video_codecs/builtin_video_encoder_factory.h"
#include "api/video_codecs/builtin_video_decoder_factory.h"

void FRenderStreamingModule::StartupModule()
{
    // Init SSL for WebRTC
    rtc::InitializeSSL();

    CreateSDPObserver = rtc::make_ref_counted<FCreateSDPObserver>();
    SetSDPObserver = rtc::make_ref_counted<FSetSDPObserver>();

    SignallingThread = rtc::Thread::CreateWithSocketServer();
    SignallingThread->SetName("signaling_thread", nullptr);
    SignallingThread->Start();

    rtc::scoped_refptr<webrtc::AudioDeviceModule> AudioDeviceModule;
    AudioDeviceModule = new rtc::RefCountedObject<UE::RenderStreaming::FNullAudioDeviceModule>();
    // Note: WebRTC is a multithreaded library!
    PeerConnectionFactory = webrtc::CreatePeerConnectionFactory(
        nullptr /* network_thread */,
        nullptr /* worker_thread */,
        SignallingThread.get(),
        AudioDeviceModule /* adm */,
        webrtc::CreateBuiltinAudioEncoderFactory(),
        webrtc::CreateBuiltinAudioDecoderFactory(),
        webrtc::CreateBuiltinVideoEncoderFactory(),
        webrtc::CreateBuiltinVideoDecoderFactory(),
        nullptr /* audio_mixer */,
        nullptr /* audio_processing */);

    SignallingObserver = MakeShared<IPixelStreamingSignallingConnectionObserver>();
    SignallingObserver->OnSignallingPlayerConnected = [this](FPixelStreamingPlayerId PlayerId, const FPixelStreamingPlayerConfig &PlayerConfig, bool bSendOffer)
    {
        if (!bSendOffer)
        {
            // If we're not sending the offer, don't create the player session
            // we'll wait until the offer arrives to do that
            return;
        }

        // Create a peer connection
        rtc::scoped_refptr<webrtc::PeerConnectionInterface> PeerConnection = PeerConnectionFactory->CreatePeerConnection(PCConfig, webrtc::PeerConnectionDependencies(this));

        // Create an SDP `offer` where we indicate we are not sending audio or video
        webrtc::PeerConnectionInterface::RTCOfferAnswerOptions Opts;
        Opts.offer_to_receive_video = false;
        Opts.offer_to_receive_audio = false;

        CreateSDPObserver->OnCreatedSDP.AddLambda([this, PlayerId, PeerConnection](webrtc::SessionDescriptionInterface *OfferSDP)
                                                  {
                                                    PeerConnection->SetLocalDescription(SetSDPObserver.get(), OfferSDP);
                                                    SignallingConnection->SendOffer(PlayerId, *OfferSDP); });
        PeerConnection->CreateOffer(CreateSDPObserver.get(), Opts);
    };

    SignallingObserver->OnSignallingConfig = [this](const webrtc::PeerConnectionInterface::RTCConfiguration &Config)
    {
        PCConfig = Config;
        // Force unified plan (no need to have plan b support)
        PCConfig.sdp_semantics = webrtc::SdpSemantics::kUnifiedPlan;
    };

    FCoreDelegates::OnFEngineLoopInitComplete.AddLambda([this]()
                                                        { SignallingConnection = MakeShared<FRenderStreamingSignallingConnection>(SignallingObserver);
                                                        SignallingConnection->TryConnect("localhost:8080"); });
}

void FRenderStreamingModule::ShutdownModule()
{
}

IMPLEMENT_MODULE(FRenderStreamingModule, RenderStreaming)