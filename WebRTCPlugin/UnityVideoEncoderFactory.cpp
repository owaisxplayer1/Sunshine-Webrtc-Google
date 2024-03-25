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
			~UnityVideoEncoder() override { }
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

			SdpVideoFormat format2("VP9");
			format2.parameters.emplace("implementation_name", "Internal");
			format2.parameters.emplace("profile-id", "0");

			formats.push_back(format1);
			formats.push_back(format2);
			return formats;
		}

		webrtc::VideoEncoderFactory::CodecSupport UnityVideoEncoderFactory::QueryCodecSupport(const SdpVideoFormat& format, absl::optional<std::string> scalability_mode) const
		{
			JLogPrint(rtc::LoggingSeverity::LS_INFO,"QueryCodecSupport");
			return CodecSupport();
		}

		std::unique_ptr<VideoEncoder> UnityVideoEncoderFactory::CreateVideoEncoder(const SdpVideoFormat& format)
		{
			return nullptr;
		}

		UnityVideoEncoderFactory::UnityVideoEncoderFactory(IGraphicsDevice* gfxDevice, ProfilerMarkerFactory* profiler)
		{
		}

		UnityVideoEncoderFactory::~UnityVideoEncoderFactory()
		{
		}
	}
}