#include <iostream>

#include "Context.h"
#include "CreateSessionDescriptionObserver.h"
#include "SetLocalDescriptionObserver.h"
#include "SetRemoteDescriptionObserver.h"

#include "Signalling/WSClient.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "nlohmann/json.hpp"

#include "Base64.h"

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

void OnMessageFromClient(std::string& msg) {
    std::cout << "Got Message: " << msg << std::endl;
}

int main() {
    signalling::StartWebsocket();
    signalling::RegisterOnMessage(OnMessageFromClient);

    //Context Manager
    ContextManager* ctx_manager = ContextManager::GetInstance();

    //Context
    ContextDependencies dep{};
    auto ctx = ctx_manager->CreateContext(1, dep);

    //PeerConnectionObject
    auto pco = ContextCreatePeerConnection(ctx);

    //ICECandidate
    pco->RegisterIceCandidate([](PeerConnectionObject* pco, const char* sdp, const char* sdp_mid, const int mline) {
        std::cout << "GOT ICE" << sdp << sdp_mid << mline << std::endl;
        });

    //Datachannel
    auto datachannel = ContextCreateDataChannel(ctx, pco, "data");

    //Offer
    SetLocalDescriptionObserver::RegisterCallback([](PeerConnectionObject*, SetLocalDescriptionObserver*, RTCErrorType, const char*) {
        printf("LocalDescription Set\n");
        });

    const RTCOfferAnswerOptions options = {false, false};
    unity::webrtc::CreateSessionDescriptionObserver::RegisterCallback([](PeerConnectionObject* peerConnectionObject,
        unity::webrtc::CreateSessionDescriptionObserver* observer,
        RTCSdpType sdpType,
        std::string sdp,
        RTCErrorType errorType,
        const char* error) {

            //SetLocaldesc
            const RTCSessionDescription desc = { RTCSdpType::Answer , (char*)sdp.c_str() };
            std::string error_;
            auto sldo = SetLocalDescriptionObserver::Create(peerConnectionObject);
            peerConnectionObject->SetLocalDescription(desc, sldo, error_);

            nlohmann::json json;

            json["type"] = "answer";
            json["sdp"] = sdp;

            // Convert JSON object to string
            std::string jsonString = json.dump();

            signalling::SendMessage(jsonString);
        });
    
    //Read and set Remote Offer
    SetRemoteDescriptionObserver::RegisterCallback([](PeerConnectionObject* pco, SetRemoteDescriptionObserver* srdo, RTCErrorType errorType, const char* error) {
        if (errorType != RTCErrorType::NONE) {
            std::cout << "SetRemoteDescriptionObserver error: " << error << std::endl;
        }

        //Create Answer
        auto csdo = unity::webrtc::CreateSessionDescriptionObserver::Create(pco);
        pco->CreateAnswer(RTCOfferAnswerOptions(), csdo.get());
    });

    auto offer = signalling::ReadMessage();
    const RTCSessionDescription desc = { RTCSdpType::Offer , (char*)offer.c_str()};
    std::string error_;
    auto observer = SetRemoteDescriptionObserver::Create(pco);
    pco->SetRemoteDescription(desc, observer, error_);


    std::this_thread::sleep_for(std::chrono::seconds(3));

    datachannel->Send(webrtc::DataBuffer(std::string("hello world")));
    std::this_thread::sleep_for(std::chrono::seconds(300));
    return 0;
}