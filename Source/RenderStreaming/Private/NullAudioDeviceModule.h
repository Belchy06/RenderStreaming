// Copyright TensorWorks, Pty Ltd. All Rights Reserved.

#pragma once

#include "modules/audio_device/include/audio_device_defines.h"
#include "modules/audio_device/include/audio_device.h"
#include "modules/audio_device/include/audio_device_defines.h"
#include "modules/audio_device/audio_device_buffer.h"

namespace UE::RenderStreaming
{
    // UE WebRTC is not built with internal audio device in the library, so we make a null one.
    class FNullAudioDeviceModule : public webrtc::AudioDeviceModule
    {
    public:
        FNullAudioDeviceModule() = default;
        virtual ~FNullAudioDeviceModule() = default;

        // ----------------------------------------------
        // BEGIN webrtc::AudioDeviceModule interface
        // ----------------------------------------------

        int32 ActiveAudioLayer(AudioLayer *audioLayer) const override { return 0; };
        int32 RegisterAudioCallback(webrtc::AudioTransport *audioCallback) override { return 0; };

        // Main initialization and termination
        int32 Init() override { return 0; };
        int32 Terminate() override { return 0; };
        bool Initialized() const override { return true; };

        // Device enumeration
        int16 PlayoutDevices() override { return 0; };
        int16 RecordingDevices() override { return 0; };
        int32 PlayoutDeviceName(uint16 index, char name[webrtc::kAdmMaxDeviceNameSize], char guid[webrtc::kAdmMaxGuidSize]) override { return 0; };
        int32 RecordingDeviceName(uint16 index, char name[webrtc::kAdmMaxDeviceNameSize], char guid[webrtc::kAdmMaxGuidSize]) override { return 0; };

        // Device selection
        int32 SetPlayoutDevice(uint16 index) override { return 0; };
        int32 SetPlayoutDevice(WindowsDeviceType device) override { return 0; };
        int32 SetRecordingDevice(uint16 index) override { return 0; };
        int32 SetRecordingDevice(WindowsDeviceType device) override { return 0; };

        // Audio transport initialization
        int32 PlayoutIsAvailable(bool *available) override
        {
            *available = false;
            return -1;
        }
        int32 InitPlayout() override { return 0; }
        bool PlayoutIsInitialized() const override { return false; }
        int32 RecordingIsAvailable(bool *available) override
        {
            *available = false;
            return -1;
        }
        int32 InitRecording() override { return -1; }
        bool RecordingIsInitialized() const override { return false; }

        // Audio transport control
        virtual int32 StartPlayout() override { return 0; };
        virtual int32 StopPlayout() override { return 0; };

        // True when audio is being pulled by the instance.
        virtual bool Playing() const override { return false; };

        virtual int32 StartRecording() override { return -1; };
        virtual int32 StopRecording() override { return -1; };
        virtual bool Recording() const override { return false; };

        // Audio mixer initialization
        virtual int32 InitSpeaker() override { return -1; };
        virtual bool SpeakerIsInitialized() const override { return false; };
        virtual int32 InitMicrophone() override { return -1; };
        virtual bool MicrophoneIsInitialized() const override { return false; };

        // Speaker volume controls
        virtual int32 SpeakerVolumeIsAvailable(bool *available) override
        {
            *available = false;
            return -1;
        };
        virtual int32 SetSpeakerVolume(uint32 volume) override { return -1; };
        virtual int32 SpeakerVolume(uint32 *volume) const override { return -1; };
        virtual int32 MaxSpeakerVolume(uint32 *maxVolume) const override { return -1; };
        virtual int32 MinSpeakerVolume(uint32 *minVolume) const override { return -1; };

        // Microphone volume controls
        virtual int32 MicrophoneVolumeIsAvailable(bool *available) override
        {
            *available = false;
            return -1;
        };
        virtual int32 SetMicrophoneVolume(uint32 volume) override { return -1; };
        virtual int32 MicrophoneVolume(uint32 *volume) const override { return -1; }
        virtual int32 MaxMicrophoneVolume(uint32 *maxVolume) const override { return -1; }
        virtual int32 MinMicrophoneVolume(uint32 *minVolume) const override { return -1; }

        // Speaker mute control
        virtual int32 SpeakerMuteIsAvailable(bool *available) override
        {
            *available = false;
            return -1;
        };
        virtual int32 SetSpeakerMute(bool enable) override { return false; }
        virtual int32 SpeakerMute(bool *enabled) const override
        {
            *enabled = false;
            return -1;
        };

        // Microphone mute control
        virtual int32 MicrophoneMuteIsAvailable(bool *available) override
        {
            *available = false;
            return -1;
        };
        virtual int32 SetMicrophoneMute(bool enable) override { return -1; };
        virtual int32 MicrophoneMute(bool *enabled) const override { return -1; };

        // Stereo support
        virtual int32 StereoPlayoutIsAvailable(bool *available) const override
        {
            *available = false;
            return -1;
        };
        virtual int32 SetStereoPlayout(bool enable) override { return -1; };
        virtual int32 StereoPlayout(bool *enabled) const override
        {
            *enabled = false;
            return -1;
        };
        virtual int32 StereoRecordingIsAvailable(bool *available) const override
        {
            *available = false;
            return -1;
        };
        virtual int32 SetStereoRecording(bool enable) override { return -1; };
        virtual int32 StereoRecording(bool *enabled) const override { return -1; };

        // Playout delay
        virtual int32 PlayoutDelay(uint16 *delayMS) const override { return -1; };
        virtual bool BuiltInAECIsAvailable() const override { return false; };
        virtual bool BuiltInAGCIsAvailable() const override { return false; };
        virtual bool BuiltInNSIsAvailable() const override { return false; };

        // Enables the built-in audio effects. Only supported on Android.
        virtual int32 EnableBuiltInAEC(bool enable) override { return -1; };
        virtual int32 EnableBuiltInAGC(bool enable) override { return -1; };
        virtual int32 EnableBuiltInNS(bool enable) override { return -1; };

        // ----------------------------------------------
        // END webrtc::AudioDeviceModule interface
        // ----------------------------------------------
    };
} // UE::RenderStreaming