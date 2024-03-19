#include "PeerConnectionObject.h"


namespace unity
{
	namespace webrtc
	{
		PeerConnectionObject::PeerConnectionObject(Context& context)
		{
		}

		PeerConnectionObject::~PeerConnectionObject()
		{
		}

		void PeerConnectionObject::Close()
		{
		}

		RTCErrorType PeerConnectionObject::SetLocalDescription(const RTCSessionDescription& desc, rtc::scoped_refptr<SetLocalDescriptionObserverInterface> observer, std::string& error)
		{
			return RTCErrorType();
		}

		RTCErrorType PeerConnectionObject::SetLocalDescriptionWithoutDescription(rtc::scoped_refptr<SetLocalDescriptionObserverInterface> observer, std::string& error)
		{
			return RTCErrorType();
		}

		RTCErrorType PeerConnectionObject::SetRemoteDescription(const RTCSessionDescription& desc, rtc::scoped_refptr<SetRemoteDescriptionObserverInterface>, std::string& error)
		{
			return RTCErrorType();
		}

		bool PeerConnectionObject::GetSessionDescription(const SessionDescriptionInterface* sdp, RTCSessionDescription& desc) const
		{
			return false;
		}

		RTCErrorType PeerConnectionObject::SetConfiguration(const std::string& config)
		{
			return RTCErrorType();
		}

		std::string PeerConnectionObject::GetConfiguration() const
		{
			return std::string();
		}

		void PeerConnectionObject::CreateOffer(const RTCOfferAnswerOptions& options, CreateSessionDescriptionObserver* observer)
		{
		}

		void PeerConnectionObject::CreateAnswer(const RTCOfferAnswerOptions& options, CreateSessionDescriptionObserver* observer)
		{
		}
		void PeerConnectionObject::OnSignalingChange(PeerConnectionInterface::SignalingState new_state)
		{
		}
		void PeerConnectionObject::OnAddStream(rtc::scoped_refptr<MediaStreamInterface> stream)
		{
		}
		void PeerConnectionObject::OnRemoveStream(rtc::scoped_refptr<MediaStreamInterface> stream)
		{
		}
		void PeerConnectionObject::OnDataChannel(rtc::scoped_refptr<DataChannelInterface> data_channel)
		{
		}
		void PeerConnectionObject::OnRenegotiationNeeded()
		{
		}
		void PeerConnectionObject::OnIceConnectionChange(PeerConnectionInterface::IceConnectionState new_state)
		{
		}
		void PeerConnectionObject::OnConnectionChange(PeerConnectionInterface::PeerConnectionState new_state)
		{
		}
		void PeerConnectionObject::OnIceGatheringChange(PeerConnectionInterface::IceGatheringState new_state)
		{
		}
		void PeerConnectionObject::OnIceCandidate(const IceCandidateInterface* candidate)
		{
		}
		void PeerConnectionObject::OnTrack(rtc::scoped_refptr<RtpTransceiverInterface> transceiver)
		{
		}
		void PeerConnectionObject::OnRemoveTrack(rtc::scoped_refptr<RtpReceiverInterface> receiver)
		{
		}
	}
}