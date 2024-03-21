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

pc.ontrack = function (event) {};

pc.oniceconnectionstatechange = (e) => log(pc.iceConnectionState);
pc.onicecandidate = (event) => {

    console.log("On ICE");
    // websocket.send(JSON.stringify(pc.localDescription));
};

// Receive data channel
pc.ondatachannel = (evt) => {
  dc = evt.channel;

  dc.onopen = () => {
    console.log("datachannel open");
  };

  let dcTimeout = null;
  dc.onmessage = (evt) => {
    console.log("dc: "+evt);
  };

  dc.onclose = () => {
    clearTimeout(dcTimeout);
    dcTimeout = null;
  };
};

async function sendAnswer(pc) {
  await pc.setLocalDescription(await pc.createAnswer());
  await waitGatheringComplete();

  const answer = pc.localDescription;

  console.log("answer: " + answer);
}

async function OnOffer(sdp) {
  console.log(sdp);
  await pc.setRemoteDescription(sdp);
  await sendAnswer(pc);
}

websocket.onmessage = function (event) {
  // var sdp = "{\"type\":\"answer\",\"sdp\":\""+ atob(event.data) + "\"}"
  var sdp = JSON.parse(event.data);
  pc.setRemoteDescription(sdp).then(()=>{console.log("Answer Set");})
  console.log(sdp);
};

websocket.onopen = () => {
  pc.createOffer()
    .then((d) => {
      pc.setLocalDescription(d);
      websocket.send(d.sdp);
    })
    .catch(log);
};
