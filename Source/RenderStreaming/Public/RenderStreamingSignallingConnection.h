#pragma once

#include <grpcpp/grpcpp.h>

#include "IPixelStreamingSignallingConnection.h"
#include "IPixelStreamingSignallingConnectionObserver.h"
#include "PixelStreamingPlayerId.h"
#include "../Private/Generated/Signalling.pb.h"
#include "../Private/Generated/Signalling.grpc.pb.h"

class FRenderStreamingSignallingConnection : public IPixelStreamingSignallingConnection
{
public:
    FRenderStreamingSignallingConnection(TSharedPtr<IPixelStreamingSignallingConnectionObserver> Observer);

    virtual void TryConnect(FString URL) override;
    virtual void Disconnect() override;
    virtual bool IsConnected() const override;

    virtual void SendOffer(FPixelStreamingPlayerId PlayerId, const webrtc::SessionDescriptionInterface &SDP) override;
    virtual void SendAnswer(FPixelStreamingPlayerId PlayerId, const webrtc::SessionDescriptionInterface &SDP) override;
    virtual void SendIceCandidate(FPixelStreamingPlayerId PlayerId, const webrtc::IceCandidateInterface &IceCandidate) override;
    virtual void SendDisconnectPlayer(FPixelStreamingPlayerId PlayerId, const FString &Reason) override;

    virtual void SendOffer(const webrtc::SessionDescriptionInterface &SDP) override;
    virtual void SendAnswer(const webrtc::SessionDescriptionInterface &SDP) override;
    virtual void SendIceCandidate(const webrtc::IceCandidateInterface &IceCandidate) override;

    virtual void SetKeepAlive(bool bKeepAlive) override;
    virtual void SetAutoReconnect(bool bAutoReconnect) override;

    virtual void SendMessage(const FString &Msg) override;

private:
    TSharedPtr<IPixelStreamingSignallingConnectionObserver> Observer;

    std::shared_ptr<grpc::Channel> Channel;
    std::unique_ptr<SignallingService::Stub> Stub;

    FTimerHandle TimerHandle;
    void KeepAlive();
};