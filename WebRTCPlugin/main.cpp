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

bool is_datachannel_open = false;
void OnDatachannel(DataChannelInterface*, const uint8_t*, int32_t) {
    LogPrint(rtc::LoggingSeverity::LS_INFO,"OnDatachannel");
    is_datachannel_open = true;
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
        LogPrint(rtc::LoggingSeverity::LS_INFO, "Got ICE: sdp:%s sdp_mid:%s mline:%d", sdp, sdp_mid, mline);

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

    rtc::scoped_refptr<VideoTrackInterface> video_track = ctx->CreateVideoTrack("video", video_source.get());
    ctx->AddRefPtr(video_track);

    const char kStreamId[] = "stream_id";
    pco->connection->AddTrack(video_track, { kStreamId });

    //Create and Send Offer
    SetLocalDescriptionObserver::RegisterCallback([](PeerConnectionObject*, SetLocalDescriptionObserver*, RTCErrorType, const char*) {
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
            peerConnectionObject->SetLocalDescription(desc, sldo, error_);

            nlohmann::json json;

            json["type"] = "offer";
            json["sdp"] = sdp;

            // Convert JSON object to string
            std::string jsonString = json.dump();

            signalling::SendMessage(jsonString);
            LogPrint(rtc::LoggingSeverity::LS_INFO, "Offer Sent to Client %s", jsonString.c_str());
        });
    auto csdo = unity::webrtc::CreateSessionDescriptionObserver::Create(pco);
    pco->CreateOffer(RTCOfferAnswerOptions(), csdo.get());

    //Read and Set Answer
    SetRemoteDescriptionObserver::RegisterCallback([](PeerConnectionObject* pco, SetRemoteDescriptionObserver* srdo, RTCErrorType errorType, const char* error) {
        if (errorType != RTCErrorType::NONE) {
            LogPrint(rtc::LoggingSeverity::LS_ERROR, "SetRemoteDescriptionObserver error: %s", error);
        }
        else {
            std::cout << "Remote Description Set" << error << std::endl;
            LogPrint(rtc::LoggingSeverity::LS_INFO, "Remote Description Set");
        }
        });

    auto answer = signalling::ReadMessage();
    LogPrint(rtc::LoggingSeverity::LS_INFO, "Got Answer from Client %s", answer.c_str());

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