/* eslint-env browser */

// SPDX-FileCopyrightText: 2023 The Pion community <https://pion.ly>
// SPDX-License-Identifier: MIT

const websocket = new WebSocket('ws://localhost:7002/client');

const pc = new RTCPeerConnection({
  iceServers: [{
    urls: 'stun:stun.l.google.com:19302'
  }]
})
const log = msg => {
  document.getElementById('div').innerHTML += msg + '<br>'
}

pc.oniceconnectionstatechange = e => log(pc.iceConnectionState)
pc.onicecandidate = event => {
  if (event.candidate === null) {
    const sdp = btoa(JSON.stringify(pc.localDescription));
    document.getElementById('localSessionDescription').value = sdp
    websocket.send(sdp)
  }
}

// Receive data channel
pc.ondatachannel = (evt) => {
  dc = evt.channel;

  dc.onopen = () => {
     console.log("datachannel open");
  };

  let dcTimeout = null;
  dc.onmessage = (evt) => {
    
  }

  dc.onclose = () => {
      clearTimeout(dcTimeout);
      dcTimeout = null;
  };
}

window.startSession = () => {
  const sd = document.getElementById('remoteSessionDescription').value
  if (sd === '') {
    return alert('Session Description must not be empty')
  }

  try {
    pc.setRemoteDescription(JSON.parse(atob(sd)))
  } catch (e) {
    alert(e)
  }
}

websocket.onmessage = function(event) {
  const sdp = event.data
  console.log("Got SDP: "+sdp);
  pc.setRemoteDescription(sdp)
};