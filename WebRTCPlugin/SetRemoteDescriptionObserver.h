#pragma once

#include <api/jsep.h>
#include <api/set_remote_description_observer_interface.h>

#include "WebRTCPlugin.h"

namespace webrtc
{
class SetRemoteDescriptionObserver : public ::webrtc::SetRemoteDescriptionObserverInterface
{
public:
    static rtc::scoped_refptr<SetRemoteDescriptionObserver> Create(PeerConnectionObject* connection);
    static void RegisterCallback(DelegateSetRemoteDesc callback) { s_setRemoteDescCallback = callback; }

    void OnSetRemoteDescriptionComplete(RTCError error) override;

protected:
    explicit SetRemoteDescriptionObserver(PeerConnectionObject* connection);
    ~SetRemoteDescriptionObserver() override = default;

private:
    PeerConnectionObject* m_connection;
    static DelegateSetRemoteDesc s_setRemoteDescCallback;
};

}
