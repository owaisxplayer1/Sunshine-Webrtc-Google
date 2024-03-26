/* eslint-env browser */

// SPDX-FileCopyrightText: 2023 The Pion community <https://pion.ly>
// SPDX-License-Identifier: MIT

const websocket = new WebSocket("ws://localhost:7002/client");

let assert_equals = window.assert_equals ? window.assert_equals : (a, b, msg) => { if (a === b) { return; } throw new Error(`${msg} expected ${b} but got ${a}`); };

const pc = new RTCPeerConnection({
  iceServers: [
    {
      urls: "stun:stun.l.google.com:19302",
    },
  ],
});
const log = (msg) => {
  document.getElementById("div").innerHTML += msg + "<br>";
};

var videoElement = document.createElement('video');
videoElement.id = 'Video';
videoElement.style.touchAction = 'none';
videoElement.playsInline = true;
videoElement.srcObject = new MediaStream();
document.getElementById("remoteVideos").appendChild(videoElement);

pc.ontrack = function (event) {
  console.log("OnTrack");
  videoElement.srcObject.addTrack(event.track);
};

pc.oniceconnectionstatechange = (e) => log(pc.iceConnectionState);
pc.onicecandidate = (event) => {
  console.log(event.candidate);
  // websocket.send(JSON.stringify(pc.localDescription));
};

function sendMessage(message) {
  if (dataChannel.readyState === "open") {
    dataChannel.send(message);
    console.log("Message sent:", message);
  } else {
    console.error("Data channel is not open");
  }
}

// Receive data channel
pc.ondatachannel = (evt) => {
  dc = evt.channel;

  dc.onopen = () => {
    console.log("datachannel open");
    // Continuously send a message every 2 seconds
    setInterval(function () {
      if (dc.readyState === "open") {
        dc.send("Hello world!");
        console.log("Message sent:");
      } else {
        console.error("Data channel is not open");
      }
    }, 2000); // 2000 milliseconds = 2 seconds
  };

  let dcTimeout = null;
  dc.onmessage = (evt) => {
    console.log("dc: " + evt);
  };

  dc.onclose = () => {
    clearTimeout(dcTimeout);
    dcTimeout = null;
  };
};

async function waitGatheringComplete() {
  return new Promise((resolve) => {
    if (pc.iceGatheringState === "complete") {
      resolve();
    } else {
      pc.addEventListener("icegatheringstatechange", () => {
        if (pc.iceGatheringState === "complete") {
          resolve();
        }
      });
    }
  });
}

async function sendAnswer(description) {
  await pc.setRemoteDescription(description);
  var answer = await pc.createAnswer();
  await pc.setLocalDescription(answer);
  console.log(answer);
  await waitGatheringComplete();
  websocket.send(JSON.stringify(answer));
}

async function onGotDescription(description) {
  await pc.setRemoteDescription(description);
  if (description.type == 'offer') {
    //_dispatchEvent(new CustomEvent('ongotoffer', { detail: { connectionId: connectionId } }));

    assert_equals(pc.signalingState, 'have-remote-offer', 'Remote offer');
    assert_equals(pc.remoteDescription.type, 'offer', 'SRD worked');
    log('SLD to get back to stable');
    await pc.setLocalDescription();
    assert_equals(pc.signalingState, 'stable', 'onmessage not racing with negotiationneeded');
    assert_equals(pc.localDescription.type, 'answer', 'onmessage SLD worked');
    //dispatchEvent(new CustomEvent('sendanswer', { detail: { connectionId: connectionId, sdp: pc.localDescription.sdp } }));
    console.log("Got Answer: "+pc.localDescription.sdp);
    websocket.send(JSON.stringify({ type: "answer", sdp: pc.localDescription.sdp }));
  }
}

function OnICE(candidate) {
  // Create an ICE candidate object
  var iceCandidate = new RTCIceCandidate({ candidate: candidate.candidate, sdpMid: candidate.sdpMid, sdpMLineIndex: candidate.sdpMLineIndex });
  pc.addIceCandidate(iceCandidate);
}

websocket.onmessage = function (event) {
  var msg = JSON.parse(event.data);
  if (msg.type == "offer") {
    console.log("Got Offer: "+msg.sdp);
    sendAnswer(msg)
  } else if(msg.type == "candidate") {
    OnICE(msg);
  }
};

websocket.onopen = () => {};
