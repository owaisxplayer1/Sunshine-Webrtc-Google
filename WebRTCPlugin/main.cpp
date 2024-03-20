#include <iostream>

#include "Context.h"
#include "CreateSessionDescriptionObserver.h"
#include "SetLocalDescriptionObserver.h"

#include "Signalling/WSClient.h"

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
    std::thread ws_th(signalling::StartWebsocket);
    signalling::RegisterOnMessage(OnMessageFromClient);

    //Context Manager
    ContextManager* ctx_manager = ContextManager::GetInstance();

    //Context
    ContextDependencies dep{};
    auto ctx = ctx_manager->CreateContext(1,dep);

    //PeerConnectionObject
    auto pco = ContextCreatePeerConnection(ctx);

    //ICECandidate
    pco->RegisterIceCandidate([](PeerConnectionObject* pco, const char* sdp, const char* sdp_mid, const int mline) {
        std::cout << "GOT ICE" << sdp << sdp_mid << mline <<std::endl;
        });

    //Datachannel
    auto datachannel = ContextCreateDataChannel(ctx, pco, "data-channel");

    //Offer
    const RTCOfferAnswerOptions options = {false, false};
    unity::webrtc::CreateSessionDescriptionObserver::RegisterCallback([](PeerConnectionObject* peerConnectionObject,
        unity::webrtc::CreateSessionDescriptionObserver* observer,
        RTCSdpType sdpType,
        const char* sdp,
        RTCErrorType errorType,
        const char* error) {
            //Send L
        });
    auto csdo = PeerConnectionCreateOffer(ctx, pco, &options);

    std::this_thread::sleep_for(std::chrono::seconds(30));
    return 0;
}