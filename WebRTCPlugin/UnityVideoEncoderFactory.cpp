#include <api/video_codecs/video_encoder.h>
#include <media/engine/internal_encoder_factory.h>
#include <modules/video_coding/include/video_error_codes.h>
#include <tuple>

#include "Logger.h"

#include "UnityVideoEncoderFactory.h"


namespace unity
{
	namespace webrtc
	{
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
			int32_t Encode(const VideoFrame& frame, const std::vector<VideoFrameType>* frame_types) override
			{
				JLogPrint(rtc::LoggingSeverity::LS_INFO, "Encode");
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