#pragma once
#include <api/peer_connection_interface.h>

#include "DataChannelObject.h"
#include "WebRTCPlugin.h"

namespace unity
{
	namespace webrtc
	{
		using namespace ::webrtc;

        extern webrtc::SdpType ConvertSdpType(RTCSdpType type);
        extern RTCSdpType ConvertSdpType(webrtc::SdpType type);

		class PeerConnectionObject : public PeerConnectionObserver
		{
        public:
            PeerConnectionObject(Context& context);
            ~PeerConnectionObject() override;

            void Close();
            RTCErrorType SetLocalDescription(
                const RTCSessionDescription& desc,
                rtc::scoped_refptr<SetLocalDescriptionObserverInterface> observer,
                std::string& error);
            RTCErrorType SetLocalDescriptionWithoutDescription(
                rtc::scoped_refptr<SetLocalDescriptionObserverInterface> observer, std::string& error);
            RTCErrorType SetRemoteDescription(
                const RTCSessionDescription& desc,
                rtc::scoped_refptr<SetRemoteDescriptionObserverInterface>,
                std::string& error);

            bool GetSessionDescription(const SessionDescriptionInterface* sdp, RTCSessionDescription& desc) const;
            RTCErrorType SetConfiguration(const std::string& config);
            std::string GetConfiguration() const;
            void CreateOffer(const RTCOfferAnswerOptions& options, CreateSessionDescriptionObserver* observer);
            void CreateAnswer(const RTCOfferAnswerOptions& options, CreateSessionDescriptionObserver* observer);

            // webrtc::PeerConnectionObserver
        // Triggered when the SignalingState changed.
            void OnSignalingChange(PeerConnectionInterface::SignalingState new_state) override;
            // Triggered when media is received on a new stream from remote peer.
            void OnAddStream(rtc::scoped_refptr<MediaStreamInterface> stream) override;
            // Triggered when a remote peer closes a stream.
            void OnRemoveStream(rtc::scoped_refptr<MediaStreamInterface> stream) override;
            // Triggered when a remote peer opens a data channel.
            void OnDataChannel(rtc::scoped_refptr<DataChannelInterface> data_channel) override;
            // Triggered when renegotiation is needed. For example, an ICE restart
            // has begun.
            void OnRenegotiationNeeded() override;
            // Called any time the IceConnectionState changes.
            void OnIceConnectionChange(PeerConnectionInterface::IceConnectionState new_state) override;
            // Called any time the PeerConnectionState changes.
            virtual void OnConnectionChange(PeerConnectionInterface::PeerConnectionState new_state) override;
            // Called any time the IceGatheringState changes.
            void OnIceGatheringChange(PeerConnectionInterface::IceGatheringState new_state) override;
            // A new ICE candidate has been gathered.
            void OnIceCandidate(const IceCandidateInterface* candidate) override;
            // Ice candidates have been removed.
            void OnIceCandidatesRemoved(const std::vector<cricket::Candidate>& candidates) override { }
            // Called when the ICE connection receiving status changes.
            void OnIceConnectionReceivingChange(bool Receiving) override { }
            // This is called when signaling indicates a transceiver will be receiving
            // media from the remote endpoint. This is fired during a call to
            // SetRemoteDescription. The receiving track can be accessed by:
            // |transceiver->receiver()->track()| and its associated streams by
            // |transceiver->receiver()->streams()|.
            // Note: This will only be called if Unified Plan semantics are specified.
            // This behavior is specified in section 2.2.8.2.5 of the "Set the
            // RTCSessionDescription" algorithm:
            // https://w3c.github.io/webrtc-pc/#set-description
            void OnTrack(rtc::scoped_refptr<RtpTransceiverInterface> transceiver) override;

            void OnRemoveTrack(rtc::scoped_refptr<RtpReceiverInterface> receiver) override;

            friend class DataChannelObject;

            rtc::scoped_refptr<PeerConnectionInterface> connection = nullptr;
		};
	}
}