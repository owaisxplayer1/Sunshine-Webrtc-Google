#include "UnityVideoTrackSource.h"

namespace unity
{
	namespace webrtc
	{
		rtc::scoped_refptr<UnityVideoTrackSource> UnityVideoTrackSource::Create(
			bool is_screencast, absl::optional<bool> needs_denoising, TaskQueueFactory* taskQueueFactory)
		{
			return rtc::make_ref_counted<UnityVideoTrackSource>(is_screencast, needs_denoising, taskQueueFactory);
		}

		UnityVideoTrackSource::UnityVideoTrackSource(bool is_screencast, absl::optional<bool> needs_denoising, TaskQueueFactory* taskQueueFactory)
			: AdaptedVideoTrackSource(/*required_alignment=*/1)
			, is_screencast_(is_screencast)
			, syncApplicationFramerate_(true)
		{
		}

		UnityVideoTrackSource::~UnityVideoTrackSource()
		{
		}

		UnityVideoTrackSource::SourceState UnityVideoTrackSource::state() const
		{
			return SourceState();
		}

		bool UnityVideoTrackSource::remote() const
		{
			return false;
		}

		bool UnityVideoTrackSource::is_screencast() const
		{
			return false;
		}

		absl::optional<bool> UnityVideoTrackSource::needs_denoising() const
		{
			return absl::optional<bool>();
		}

		void UnityVideoTrackSource::OnFrameCaptured(rtc::scoped_refptr<VideoFrame> frame)
		{
		}

		void UnityVideoTrackSource::SetSyncApplicationFramerate(bool value)
		{
		}

		void UnityVideoTrackSource::OnUpdateVideoFrame()
		{
		}

		void UnityVideoTrackSource::CaptureVideoFrame()
		{
			
		}

		void UnityVideoTrackSource::SendFeedback()
		{
		}

		UnityVideoTrackSource::FrameAdaptationParams UnityVideoTrackSource::ComputeAdaptationParams(int width, int height, int64_t time_us)
		{
			return FrameAdaptationParams();
		}
	} // end namespace webrtc
} // end namespace unity
