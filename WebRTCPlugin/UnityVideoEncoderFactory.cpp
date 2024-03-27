#include <api/video_codecs/video_encoder.h>
#include <media/engine/internal_encoder_factory.h>
#include <modules/video_coding/include/video_error_codes.h>

#include <absl/strings/match.h>
#include <api/video/video_codec_constants.h>
#include <api/video/video_codec_type.h>
#include <common_video/h264/h264_common.h>
#include <media/base/media_constants.h>
#include <modules/video_coding/include/video_codec_interface.h>
#include <modules/video_coding/utility/simulcast_utility.h>

#include <tuple>

#include "Logger.h"

#include "UnityVideoEncoderFactory.h"
#include <iostream>
#include <string>
#include <WinSock2.h> // Include Windows socket header
#include <WS2tcpip.h> // Include Windows socket IP header

namespace unity
{
	namespace webrtc
	{
		EncodedImageCallback* m_encodedCompleteCallback;

		SOCKET sockfd;
		int CreateTcpSocket() {
			// Initialize Winsock
			WSADATA wsaData;
			if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
				std::cerr << "Error: Failed to initialize Winsock." << std::endl;
				return 1;
			}

			// Create a TCP socket
			sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (sockfd == INVALID_SOCKET) {
				std::cerr << "Error: Failed to create socket." << std::endl;
				WSACleanup(); // Clean up Winsock
				return 1;
			}

			// Server address (change these values according to your server)
			const char* server_ip = "127.0.0.1";  // Server IP address
			uint16_t server_port = 7010;           // Server port

			// Initialize server address structure
			sockaddr_in server_addr;
			memset(&server_addr, 0, sizeof(server_addr));
			server_addr.sin_family = AF_INET;
			server_addr.sin_port = htons(server_port);
			inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

			// Connect to the server
			if (connect(sockfd, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == SOCKET_ERROR) {
				std::cerr << "Error: Failed to connect to the server. Error code: " << WSAGetLastError() << std::endl;
				closesocket(sockfd); // Close the socket
				WSACleanup();        // Clean up Winsock
				return 1;
			}			
		}

		class UnityVideoEncoder : public VideoEncoder
		{
		public:
			UnityVideoEncoder(){}
			~UnityVideoEncoder() override { }
			int32_t InitEncode(const VideoCodec* codec_settings, int32_t number_of_cores, size_t max_payload_size) override
			{
				JLogPrint(rtc::LoggingSeverity::LS_INFO, "Init Encode");
				return WEBRTC_VIDEO_CODEC_OK;
			}
			int InitEncode(const VideoCodec* codec_settings, const VideoEncoder::Settings& settings) override
			{
				JLogPrint(rtc::LoggingSeverity::LS_INFO, "Init Encode");
				return WEBRTC_VIDEO_CODEC_OK;
			}

			static const int BUFFER_SIZE = 5000000;
			//size//payload
			//4b//...
			char buffer[BUFFER_SIZE];
			int32_t Encode(const VideoFrame& frame, const std::vector<VideoFrameType>* frame_types) override
			{
				// Receive data
				int bytes_received;
					
				int readsize = 0;
				bytes_received = recv(sockfd, reinterpret_cast<char*>(&readsize), 4, 0);
				if(readsize == 0) return WEBRTC_VIDEO_CODEC_OK;
				
				bytes_received = recv(sockfd, buffer, readsize, 0);

				//JLogPrint(rtc::LoggingSeverity::LS_INFO, "Got Sample: %d:%d", readsize,bytes_received);


				EncodedImage m_encodedImage;
				m_encodedImage._encodedWidth = 1920;
				m_encodedImage._encodedHeight = 1080;
				m_encodedImage.SetTimestamp(frame.timestamp()); 
				m_encodedImage.SetSimulcastIndex(0);
				m_encodedImage.ntp_time_ms_ = frame.ntp_time_ms();
				m_encodedImage.capture_time_ms_ = frame.render_time_ms();
				m_encodedImage.rotation_ = frame.rotation();
				m_encodedImage.content_type_ = VideoContentType::UNSPECIFIED;
				m_encodedImage.timing_.flags = VideoSendTiming::kInvalid;
				m_encodedImage._frameType = VideoFrameType::kVideoFrameDelta;
				m_encodedImage.SetColorSpace(frame.color_space());

				m_encodedImage.SetEncodedData(EncodedImageBuffer::Create((const uint8_t*) &buffer, readsize));
				m_encodedImage.set_size(readsize);
				

				CodecSpecificInfo codecInfo;
				codecInfo.codecType = kVideoCodecH264;
				codecInfo.codecSpecific.H264.packetization_mode = H264PacketizationMode::NonInterleaved;

				const auto result = m_encodedCompleteCallback->OnEncodedImage(m_encodedImage, &codecInfo);
				if (result.error != EncodedImageCallback::Result::OK)
				{
					RTC_LOG(LS_ERROR) << "Encode m_encodedCompleteCallback failed " << result.error;
					return WEBRTC_VIDEO_CODEC_ERROR;
				}

				return WEBRTC_VIDEO_CODEC_OK;
			}
			EncoderInfo GetEncoderInfo() const override { 
				EncoderInfo info;
				info.scaling_settings.min_pixels_per_frame = 57600;
				info.requested_resolution_alignment = 1;
				info.apply_alignment_to_all_simulcast_layers = false;
				info.supports_native_handle = true;
				info.implementation_name = "NvCodec";
				info.has_trusted_rate_controller = false;
				info.is_hardware_accelerated = true;
				info.preferred_pixel_formats.push_back(VideoFrameBuffer::Type::kI420);

				return info;
			}

			void SetRates(const RateControlParameters& parameters) override { 
				
			}

			void OnPacketLossRateUpdate(float packet_loss_rate) override
			{
			}
			void OnRttUpdate(int64_t rtt_ms) override {}
			void OnLossNotification(const LossNotification& loss_notification) override
			{}
			int32_t RegisterEncodeCompleteCallback(EncodedImageCallback* callback) override
			{
				m_encodedCompleteCallback = callback;
				return WEBRTC_VIDEO_CODEC_OK;
			}
			void SetFecControllerOverride(FecControllerOverride* fec_controller_override) override
			{
			}
			int32_t Release() override {
				return WEBRTC_VIDEO_CODEC_OK;
			}
		};

		std::vector<SdpVideoFormat> UnityVideoEncoderFactory::GetSupportedFormats() const
		{
			auto formats = std::vector<webrtc::SdpVideoFormat>();
			SdpVideoFormat format1("H264");
			format1.name = "H264";
			format1.parameters.emplace("implementation_name", "NvCodec");
			format1.parameters.emplace("level-asymmetry-allowed", "1");
			format1.parameters.emplace("packetization-mode", "1");
			format1.parameters.emplace("profile-level-id", "42e033");

			formats.push_back(format1);
			return formats;
		}

		webrtc::VideoEncoderFactory::CodecSupport UnityVideoEncoderFactory::QueryCodecSupport(const SdpVideoFormat& format, absl::optional<std::string> scalability_mode) const
		{
			JLogPrint(rtc::LoggingSeverity::LS_INFO,"QueryCodecSupport");
			return CodecSupport();
		}

		std::unique_ptr<VideoEncoder> UnityVideoEncoderFactory::CreateVideoEncoder(const SdpVideoFormat& format)
		{
			JLogPrint(rtc::LoggingSeverity::LS_INFO, "CreateVideoEncoder");
			CreateTcpSocket();
			return std::make_unique<UnityVideoEncoder>();
		}

		UnityVideoEncoderFactory::UnityVideoEncoderFactory(IGraphicsDevice* gfxDevice, ProfilerMarkerFactory* profiler)
			: profiler_(profiler)
			, factories_()
		{
		}

		UnityVideoEncoderFactory::~UnityVideoEncoderFactory()
		{
		}
	}
}