#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "RenderStreamingSignallingConnection.h"

class FRenderStreamingModule : public IModuleInterface, public webrtc::PeerConnectionObserver
{
public:
	static inline FRenderStreamingModule &Get()
	{
		return FModuleManager::LoadModuleChecked<FRenderStreamingModule>("RenderStreaming");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("RenderStreaming");
	}

private:
	void StartupModule() override;
	void ShutdownModule() override;

	TSharedPtr<IPixelStreamingSignallingConnectionObserver> SignallingObserver;
	TSharedPtr<FRenderStreamingSignallingConnection> SignallingConnection;

private:
	std::unique_ptr<rtc::Thread> SignallingThread;
	rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> PeerConnectionFactory;
	webrtc::PeerConnectionInterface::RTCConfiguration PCConfig;
	;
	rtc::scoped_refptr<webrtc::PeerConnectionInterface> PeerConnection;

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
			// UE_LOG(LogPixelBlaster, Error, TEXT("Failed to created offer: %s"), UTF8_TO_TCHAR(Error.message()));
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
			// UE_LOG(LogPixelBlaster, Error, TEXT("Failed to set answer: %s"), UTF8_TO_TCHAR(Error.message()));
			OnSetSDPFailed.Broadcast(Error);
		}
	};

	virtual void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState NewState) override{};

	// Triggered when a remote peer opens a data channel.
	virtual void OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> DataChannel) override{};

	// Called any time the PeerConnectionState changes.
	virtual void OnConnectionChange(webrtc::PeerConnectionInterface::PeerConnectionState NewState) override{};

	// Called any time the IceGatheringState changes.
	virtual void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState NewState) override{};

	// A new ICE candidate has been gathered.
	virtual void OnIceCandidate(const webrtc::IceCandidateInterface *Candidate) override{};

private:
	rtc::scoped_refptr<FCreateSDPObserver> CreateSDPObserver;
	rtc::scoped_refptr<FSetSDPObserver> SetSDPObserver;
};