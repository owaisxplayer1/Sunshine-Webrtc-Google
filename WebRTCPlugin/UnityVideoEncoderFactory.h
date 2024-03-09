#pragma once

#include <api/video_codecs/sdp_video_format.h>
#include <api/video_codecs/video_encoder_factory.h>
#include <api/video_codecs/video_codec.h>
#include <api/video_codecs/video_encoder.h>

namespace unity
{
    namespace webrtc
    {
        using namespace ::webrtc;

        class DummyVideoEncoder : public VideoEncoder
        {
            // webrtc::VideoEncoder
            // Initialize the encoder with the information from the codecSettings
            int32_t InitEncode(const VideoCodec* codec_settings, const VideoEncoder::Settings& settings) override;

            // Free encoder memory.
            int32_t Release() override;
            // Register an encode complete m_encodedCompleteCallback object.
            int32_t RegisterEncodeCompleteCallback(EncodedImageCallback* callback) override;
            // Encode an I420 image (as a part of a video stream). The encoded image
            // will be returned to the user through the encode complete m_encodedCompleteCallback.
            int32_t Encode(const ::webrtc::VideoFrame& frame, const std::vector<VideoFrameType>* frame_types) override;
            // Default fallback: Just use the sum of bitrates as the single target rate.
            void SetRates(const RateControlParameters& parameters) override;
            // Returns meta-data about the encoder, such as implementation name.
            EncoderInfo GetEncoderInfo() const override;
        };

        class IGraphicsDevice;
        class ProfilerMarkerFactory;
        class UnityVideoEncoderFactory : public VideoEncoderFactory
        {
        public:
            // VideoEncoderFactory
            // Returns a list of supported video formats in order of preference, to use
            // for signaling etc.
            std::vector<SdpVideoFormat> GetSupportedFormats() const override;
            // Returns information about how this format will be encoded. The specified
            // format must be one of the supported formats by this factory.
            CodecSupport
                QueryCodecSupport(const SdpVideoFormat& format, absl::optional<std::string> scalability_mode) const override;
            // Creates a VideoEncoder for the specified format.
            std::unique_ptr<webrtc::VideoEncoder> CreateVideoEncoder(const SdpVideoFormat& format) override;

            UnityVideoEncoderFactory(IGraphicsDevice* gfxDevice, ProfilerMarkerFactory* profiler);
            ~UnityVideoEncoderFactory() override;

        private:
            ProfilerMarkerFactory* profiler_;
            std::map<std::string, std::unique_ptr<VideoEncoderFactory>> factories_;
        };
    }
}