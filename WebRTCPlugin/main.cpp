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

#include "UnityVideoEncoderFactory.h"
#include "Context.h"

using namespace webrtc;
using namespace unity::webrtc;

#define UNITY_INTERFACE_EXPORT

PeerConnectionObject* ContextCreatePeerConnection(Context* context)
{
    PeerConnectionInterface::RTCConfiguration config;
    config.sdp_semantics = SdpSemantics::kUnifiedPlan;
    config.enable_implicit_rollback = true;
    config.set_suspend_below_min_bitrate(false);
    return context->CreatePeerConnection(config);
}

UNITY_INTERFACE_EXPORT DataChannelInterface* ContextCreateDataChannel(
    Context* ctx, PeerConnectionObject* obj, const char* label)
{
    DataChannelInit _options;
    

    return ctx->CreateDataChannel(obj, label, _options);
}

int main() {
    //Context Manager
    ContextManager* ctx_manager = ContextManager::GetInstance();

    //Context
    ContextDependencies dep{};
    auto ctx = ctx_manager->CreateContext(1,dep);

    //PeerConnectionObject
    auto pco = ContextCreatePeerConnection(ctx);

    //Datachannel
    auto datachannel = ContextCreateDataChannel(ctx, pco, "data-channel");

    std::this_thread::sleep_for(std::chrono::seconds(30));
    return 0;
}