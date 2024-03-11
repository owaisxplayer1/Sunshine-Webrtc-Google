#include <iostream>

#include <api/create_peerconnection_factory.h>
#include <api/task_queue/default_task_queue_factory.h>
#include <rtc_base/ssl_adapter.h>
#include <rtc_base/strings/json.h>

#include <api/task_queue/default_task_queue_factory.h>

#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"


#include "examples/peerconnection/client/conductor.h"

#include <stddef.h>
#include <stdint.h>

#include <memory>
#include <utility>
#include <vector>

#include "absl/memory/memory.h"
#include "absl/types/optional.h"
#include "api/audio/audio_mixer.h"
#include "api/audio_codecs/audio_decoder_factory.h"
#include "api/audio_codecs/audio_encoder_factory.h"
#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include "api/audio_options.h"
#include "api/create_peerconnection_factory.h"
#include "api/rtp_sender_interface.h"
#include "api/video_codecs/video_decoder_factory.h"
#include "api/video_codecs/video_decoder_factory_template.h"
#include "api/video_codecs/video_decoder_factory_template_dav1d_adapter.h"
#include "api/video_codecs/video_decoder_factory_template_libvpx_vp8_adapter.h"
#include "api/video_codecs/video_decoder_factory_template_libvpx_vp9_adapter.h"
#include "api/video_codecs/video_decoder_factory_template_open_h264_adapter.h"
#include "api/video_codecs/video_encoder_factory.h"
#include "api/video_codecs/video_encoder_factory_template.h"
#include "api/video_codecs/video_encoder_factory_template_libaom_av1_adapter.h"
#include "api/video_codecs/video_encoder_factory_template_libvpx_vp8_adapter.h"
#include "api/video_codecs/video_encoder_factory_template_libvpx_vp9_adapter.h"
#include "api/video_codecs/video_encoder_factory_template_open_h264_adapter.h"
#include "examples/peerconnection/client/defaults.h"
#include "modules/audio_device/include/audio_device.h"
#include "modules/audio_processing/include/audio_processing.h"
#include "modules/video_capture/video_capture.h"
#include "modules/video_capture/video_capture_factory.h"
#include "p2p/base/port_allocator.h"
#include "pc/video_track_source.h"
#include "rtc_base/checks.h"
#include "rtc_base/logging.h"
#include "rtc_base/rtc_certificate_generator.h"
#include "rtc_base/strings/json.h"


#include <d3d11.h>
#include <d3d11_4.h>
#include <d3dcommon.h>
#include <dwmapi.h>
#include <dxgi.h>
#include <dxgi1_6.h>

using namespace webrtc;
#include "UnityVideoEncoderFactory.h"

namespace unity
{
    namespace webrtc
    {
        int CreatePeerConnectionFactory() {
            rtc::InitializeSSL();
            rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> m_peerConnectionFactory;

            std::unique_ptr<rtc::Thread> signaling_thread_;
            std::unique_ptr<webrtc::TaskQueueFactory> m_taskQueueFactory = CreateDefaultTaskQueueFactory();

            if (!signaling_thread_.get()) {
                signaling_thread_ = rtc::Thread::CreateWithSocketServer();
                signaling_thread_->Start();
            }

            std::unique_ptr<webrtc::VideoEncoderFactory> videoEncoderFactory =
                std::make_unique<UnityVideoEncoderFactory>(nullptr, nullptr);

            ::webrtc::CreatePeerConnectionFactory(
                nullptr /* network_thread */, nullptr /* worker_thread */,
                signaling_thread_.get(), nullptr /* default_adm */,
                webrtc::CreateBuiltinAudioEncoderFactory(),
                webrtc::CreateBuiltinAudioDecoderFactory(),
                std::move(videoEncoderFactory),
                std::make_unique<webrtc::VideoDecoderFactoryTemplate<
                webrtc::LibvpxVp8DecoderTemplateAdapter,
                webrtc::LibvpxVp9DecoderTemplateAdapter,
                webrtc::OpenH264DecoderTemplateAdapter,
                webrtc::Dav1dDecoderTemplateAdapter>>(),
                nullptr /* audio_mixer */, nullptr /* audio_processing */);

            std::cout << "CreatePeerConnectionFactory" << std::endl;
            std::string userInput;
            std::cin >> userInput;
            return 0;
        }
    }
}

int main() {
    unity::webrtc::CreatePeerConnectionFactory();
}