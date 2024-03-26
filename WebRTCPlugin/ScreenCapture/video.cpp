#include <atomic>
#include <bitset>
#include <list>
#include <thread>

#include "sync.h"
#include "platform/common.h"
#include "video.h"

void
free_buffer(AVBufferRef* ref) {
}

using img_event_t = std::shared_ptr<safe::event_t<std::shared_ptr<platf::img_t>>>;
using buffer_t = util::safe_ptr<AVBufferRef, free_buffer>;

struct config_t {
    int width;
    int height;
    int framerate;
    int bitrate;
    int slicesPerFrame;
    int numRefFrames;
    int encoderCscMode;
    int videoFormat;
    int dynamicRange;
};

struct capture_ctx_t {
    img_event_t images;
    config_t config;
};

struct encoder_t {
    std::string_view name;
    enum flag_e {
        PASSED,  // Is supported
        REF_FRAMES_RESTRICT,  // Set maximum reference frames
        CBR,  // Some encoders don't support CBR, if not supported --> attempt constant quantatication parameter instead
        DYNAMIC_RANGE,  // hdr
        VUI_PARAMETERS,  // AMD encoder with VAAPI doesn't add VUI parameters to SPS
        MAX_FLAGS
    };

    struct option_t {
        KITTY_DEFAULT_CONSTR_MOVE(option_t)
            option_t(const option_t&) = default;

        std::string name;
        std::variant<int, int*, std::optional<int>*, std::string, std::string*> value;

        option_t(std::string&& name, decltype(value) && value) :
            name{ std::move(name) }, value{ std::move(value) } {}
    };

    struct {
        std::vector<option_t> common_options;
        std::vector<option_t> sdr_options;
        std::vector<option_t> hdr_options;
        std::optional<option_t> qp;

        std::string name;
        std::bitset<MAX_FLAGS> capabilities;

        bool
            operator[](flag_e flag) const {
            return capabilities[(std::size_t)flag];
        }

        std::bitset<MAX_FLAGS>::reference
            operator[](flag_e flag) {
            return capabilities[(std::size_t)flag];
        }
    } hevc, h264;

    int flags;

    std::function<util::Either<buffer_t, int>(platf::hwdevice_t* hwdevice)> make_hwdevice_ctx;
};

void
captureThread(
    std::shared_ptr<safe::queue_t<capture_ctx_t>> capture_ctx_queue,
    sync_util::sync_t<std::weak_ptr<platf::display_t>>& display_wp,
    safe::signal_t& reinit_event,
    const encoder_t& encoder) {
    std::vector<capture_ctx_t> capture_ctxs;

    auto fg = util::fail_guard([&]() {
        capture_ctx_queue->stop();

        // Stop all sessions listening to this thread
        for (auto& capture_ctx : capture_ctxs) {
            capture_ctx.images->stop();
        }
        for (auto& capture_ctx : capture_ctx_queue->unsafe()) {
            capture_ctx.images->stop();
        }
    });

    // Get all the monitor names now, rather than at boot, to
    // get the most up-to-date list available monitors
    auto display_names = platf::display_names(platf::mem_type_e::dxgi);
    int display_p = 0;
}