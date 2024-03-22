/* eslint-env browser */

// SPDX-FileCopyrightText: 2023 The Pion community <https://pion.ly>
// SPDX-License-Identifier: MIT

const websocket = new WebSocket("ws://localhost:7002/client");

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

pc.ontrack = function (event) {
  console.log("OnTrack");
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

async function sendAnswer(pc) {
  var answer = await pc.createAnswer();
  await pc.setLocalDescription(answer);
  console.log(answer);
  await waitGatheringComplete();
  websocket.send(JSON.stringify(answer));
}

async function OnOffer(sdp) {
  console.log(sdp);
  await pc.setRemoteDescription(sdp);
  await sendAnswer(pc);
}

function OnICE(msg) {
  var candidate = {
    candidate: msg.candidate,
    sdpMid: msg.sdpMid,
    sdpMLineIndex: msg.sdpMLineIndex,
  };
  // Create an ICE candidate object
  var iceCandidate = new RTCIceCandidate(candidate);
  console.log("OnICE: " + candidate);
  pc.addIceCandidate(iceCandidate);
}

websocket.onmessage = function (event) {
  var msg = JSON.parse(event.data);
  if (msg.type == "offer") {
    OnOffer(msg);
  } else {
    OnICE(msg);
  }
};

websocket.onopen = () => {};
