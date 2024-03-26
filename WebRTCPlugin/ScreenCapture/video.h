/**
 * @file src/video.h
 * @brief todo
 */
#pragma once

#include "platform/common.h"
#include "thread_safe.h"

struct AVPacket;
namespace video {

    struct packet_raw_t {
        void
            init_packet() {
        }

        template <class P>
        explicit packet_raw_t(P* user_data) :
            channel_data{ user_data } {
            init_packet();
        }

        explicit packet_raw_t(std::nullptr_t) :
            channel_data{ nullptr } {
            init_packet();
        }

        struct replace_t {
            std::string_view old;
            std::string_view _new;

            KITTY_DEFAULT_CONSTR_MOVE(replace_t)

                replace_t(std::string_view old, std::string_view _new) noexcept :
                old{ std::move(old) }, _new{ std::move(_new) } {}
        };

        AVPacket* av_packet;
        std::vector<replace_t>* replacements;
        void* channel_data;

        std::optional<std::chrono::steady_clock::time_point> frame_timestamp;
    };

    using packet_t = std::unique_ptr<packet_raw_t>;

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

    using float4 = float[4];
    using float3 = float[3];
    using float2 = float[2];

    struct alignas(16) color_t {
        float4 color_vec_y;
        float4 color_vec_u;
        float4 color_vec_v;
        float2 range_y;
        float2 range_uv;
    };

    extern color_t colors[6];

    extern int active_hevc_mode;

    void
        capture(
            safe::mail_t mail,
            config_t config,
            void* channel_data);

    int
        probe_encoders();
}  // namespace video
