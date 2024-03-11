#include "UnityVideoEncoderFactory.h"

namespace unity
{
    namespace webrtc
    {
        using namespace ::webrtc;
        std::vector<SdpVideoFormat> UnityVideoEncoderFactory::GetSupportedFormats() const {
            return std::vector<SdpVideoFormat>();
        }
        webrtc::VideoEncoderFactory::CodecSupport UnityVideoEncoderFactory::QueryCodecSupport(const SdpVideoFormat& format, absl::optional<std::string> scalability_mode) const
        {
            return CodecSupport();
        }
        std::unique_ptr<webrtc::VideoEncoder> UnityVideoEncoderFactory::CreateVideoEncoder(const SdpVideoFormat& format)
        {
            printf("CreateVideoEncoder\n");
            return std::unique_ptr<DummyVideoEncoder>();
        }
        UnityVideoEncoderFactory::UnityVideoEncoderFactory(IGraphicsDevice* gfxDevice, ProfilerMarkerFactory* profiler)
        {
        }
        UnityVideoEncoderFactory::~UnityVideoEncoderFactory()
        {
        }


        int32_t DummyVideoEncoder::InitEncode(const ::webrtc::VideoCodec* codec_settings, const VideoEncoder::Settings& settings)
        {
            return 0;
        }
        int32_t DummyVideoEncoder::Release()
        {
            return 0;
        }
        int32_t DummyVideoEncoder::RegisterEncodeCompleteCallback(EncodedImageCallback* callback)
        {
            return 0;
        }
        int32_t DummyVideoEncoder::Encode(const ::webrtc::VideoFrame& frame, const std::vector<VideoFrameType>* frame_types)
        {
            return 0;
        }
        void DummyVideoEncoder::SetRates(const RateControlParameters& parameters)
        {
        }
        
        ::webrtc::VideoEncoder::EncoderInfo DummyVideoEncoder::GetEncoderInfo() const
        {
            return EncoderInfo();
        }
    }
}