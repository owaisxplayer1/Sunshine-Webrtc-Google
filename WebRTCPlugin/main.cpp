#include <iostream>

#include "Context.h"
#include "CreateSessionDescriptionObserver.h"
#include "SetLocalDescriptionObserver.h"
#include "SetRemoteDescriptionObserver.h"

#include "Signalling/WSClient.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "nlohmann/json.hpp"

#include "Logger.h"

using namespace webrtc;
using namespace unity::webrtc;

#define UNITY_INTERFACE_EXPORT
#define JLogPrint unity::webrtc::JLogPrint

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

UNITY_INTERFACE_EXPORT unity::webrtc::CreateSessionDescriptionObserver*
PeerConnectionCreateOffer(Context* context, PeerConnectionObject* obj, const RTCOfferAnswerOptions* options)
{
    auto observer = unity::webrtc::CreateSessionDescriptionObserver::Create(obj);
    obj->CreateOffer(*options, observer.get());
    return observer.get();
}

void OnDatachannel(DataChannelInterface*, const uint8_t* msg, int32_t size) {
    JLogPrint(rtc::LoggingSeverity::LS_INFO,"Datachannel Message: %.*s\n", size, msg);
}

std::vector<std::string> Split(const std::string& str, const std::string& delimiter)
{
    std::vector<std::string> dst;
    std::string s = str;
    size_t pos = 0;

    if (str.empty())
        return dst;

    while (true)
    {
        pos = s.find(delimiter);
        size_t length = pos;
        if (pos == std::string::npos)
            length = str.length();
        dst.push_back(s.substr(0, length));
        if (pos == std::string::npos)
            break;
        s.erase(0, pos + delimiter.length());
    }
    return dst;
}
std::map<std::string, std::string> ConvertSdp(const std::string& src)
{
    std::map<std::string, std::string> map;
    std::vector<std::string> vec = Split(src, ";");

    for (const auto& str : vec)
    {
        std::vector<std::string> pair = Split(str, "=");
        map.emplace(pair[0], pair[1]);
    }
    return map;
}

int main() {
    signalling::StartWebsocket();

    //Context Manager
    ContextManager* ctx_manager = ContextManager::GetInstance();

    //Context
    ContextDependencies dep{};
    auto ctx = ctx_manager->CreateContext(1, dep);

    //PeerConnectionObject
    auto pco = ContextCreatePeerConnection(ctx);

    //ICECandidate
    pco->RegisterIceCandidate([](PeerConnectionObject* pco, const char* sdp, const char* sdp_mid, const int mline) {
        LOG_INFO("Got ICE: sdp:" << sdp << " sdp_mid:" << sdp_mid << " mline:"<< mline);

        //Send ICE
        nlohmann::json json;
        json["type"] = "candidate";
        json["candidate"] = sdp;
        json["sdpMid"] = sdp_mid;
        json["sdpMLineIndex"] = sdp;

        std::string jsonString = json.dump();
        signalling::SendMessage(jsonString);
        });

    //Datachannel
    auto datachannel = ContextCreateDataChannel(ctx, pco, "data");
    ctx->GetDataChannelObject(datachannel)->RegisterOnMessage(OnDatachannel);

    //Create VideoSource
    rtc::scoped_refptr<VideoTrackSourceInterface> video_source = ctx->CreateVideoSource();
    ctx->AddRefPtr(video_source);

    //Create VideoTrack
    rtc::scoped_refptr<VideoTrackInterface> video_track = ctx->CreateVideoTrack("video", video_source.get());
    ctx->AddRefPtr(video_track);

    //Add Track
    const char kStreamId[] = "stream_id";
    pco->connection->AddTrack(video_track, { kStreamId });

    //Create Transceiver
    RtpTransceiverInit TransceiverInit;
    TransceiverInit.direction = RtpTransceiverDirection::kSendOnly;
    RtpEncodingParameters encoding_param1;
    encoding_param1.active = true;
    encoding_param1.max_bitrate_bps = 10000000;
    encoding_param1.min_bitrate_bps = 1000000;
    encoding_param1.max_framerate = 30;
    TransceiverInit.send_encodings.push_back(encoding_param1);
    auto transceiver_or_error = pco->connection->AddTransceiver(video_track, TransceiverInit);
    if (!transceiver_or_error.error().ok()) {
        JLogPrint(rtc::LoggingSeverity::LS_ERROR, "Cannot add Transceiver");
    }
    auto transceiver = transceiver_or_error.value().get();
    
    //Set Codec Preferences
    RtpCodecCapability codec_capability;
    codec_capability.clock_rate = 90000;
    codec_capability.kind = cricket::MediaType::MEDIA_TYPE_VIDEO;
    codec_capability.name = "H264";
    
    // For PC
    //implementation_name=NvCodec;level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=420033
    // For Iphone Safari
    //implementation_name=NvCodec;level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42e033
    codec_capability.parameters = ConvertSdp("implementation_name=NvCodec;level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42e033");
    std::vector<RtpCodecCapability> _codecs = { codec_capability };
    transceiver->SetCodecPreferences(_codecs);

    //Create and Send Offer
    SetLocalDescriptionObserver::RegisterCallback([](PeerConnectionObject* pco, SetLocalDescriptionObserver* sldo, RTCErrorType errorType, const char* error) {
        if (errorType != RTCErrorType::NONE) {
            JLogPrint(rtc::LoggingSeverity::LS_ERROR, "SetLocalDescriptionObserver error: %s", error);
        }
        else {
            std::cout << "Local Description Set" << error << std::endl;
            JLogPrint(rtc::LoggingSeverity::LS_INFO, "Local Description Set");
        }
        });

    unity::webrtc::CreateSessionDescriptionObserver::RegisterCallback([](PeerConnectionObject* peerConnectionObject,
        unity::webrtc::CreateSessionDescriptionObserver* observer,
        RTCSdpType sdpType,
        std::string sdp,
        RTCErrorType errorType,
        const char* error) {
            //SetLocaldesc
            const RTCSessionDescription desc = { RTCSdpType::Offer , (char*)sdp.c_str() };
            std::string error_;
            auto sldo = SetLocalDescriptionObserver::Create(peerConnectionObject);
            peerConnectionObject->SetLocalDescriptionWithoutDescription( sldo, error_);

            nlohmann::json json;

            json["type"] = "offer";
            json["sdp"] = sdp;

            // Convert JSON object to string
            std::string jsonString = json.dump();

            signalling::SendMessage(jsonString);
            JLogPrint(rtc::LoggingSeverity::LS_INFO, "CreateSessionDescriptionObserver: %s\n%s", sdp.c_str(), error);
        });
    auto csdo = unity::webrtc::CreateSessionDescriptionObserver::Create(pco);
    pco->CreateOffer(RTCOfferAnswerOptions(), csdo.get());

    //Read and Set Answer
    SetRemoteDescriptionObserver::RegisterCallback([](PeerConnectionObject* pco, SetRemoteDescriptionObserver* srdo, RTCErrorType errorType, const char* error) {
        if (errorType != RTCErrorType::NONE) {
            JLogPrint(rtc::LoggingSeverity::LS_ERROR, "SetRemoteDescriptionObserver error: %s", error);
        }
        else {
            std::cout << "Remote Description Set" << error << std::endl;
            JLogPrint(rtc::LoggingSeverity::LS_INFO, "Remote Description Set");
        }
        });

    auto answer = signalling::ReadMessage();
    JLogPrint(rtc::LoggingSeverity::LS_INFO, "Got Answer from Client %s", answer.c_str());

    nlohmann::json answer_json = nlohmann::json::parse(answer);

    std::string answer_sdp = answer_json["sdp"];
    const RTCSessionDescription desc = { RTCSdpType::Answer , (char*)answer_sdp.c_str()};
    
    std::string error_;
    auto observer = SetRemoteDescriptionObserver::Create(pco);
    pco->SetRemoteDescription(desc, observer, error_);

    //Read ICE
    auto candidate = signalling::ReadMessage();

    std::this_thread::sleep_for(std::chrono::seconds(300));
    
    return 0;
}