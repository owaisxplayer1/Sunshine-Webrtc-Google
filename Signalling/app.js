const http = require('http');
const websocket = require('websocket');
const express = require('express')
const path = require('path');

const app = express();
const AppPORT = 7001;
const SignallingPORT = 7002;

// Serve static files from the 'public' directory
app.use(express.static(path.join(__dirname, 'public')));

// Start the server
app.listen(AppPORT, "0.0.0.0", () => {
    console.log(`App is running on http://localhost:${AppPORT}`);
});

const server_id = "server"
const client_id = "client"
const clients = {};

const httpServer = http.createServer((req, res) => {

    const respond = (code, data, contentType = 'text/plain') => {
      res.writeHead(code, {
        'Content-Type': contentType,
        'Access-Control-Allow-Origin': '*',
      });
      res.end(data);
    };
  
    respond(404, 'Not Found');
});

const wsServer = new websocket.server({ httpServer });
wsServer.on('request', (req) => {
    const { path } = req.resourceURL;
    const splitted = path.split('/');
    splitted.shift();
  
    const id = splitted[0];
    if (id != "server" && id != "client"){
      req.reject(404)
      return
    }

    const conn = req.accept(null, req.origin);

    conn.on('message', (data) => {
          const destId = id == "server" ? "client" : "server";
          const dest = clients[destId];
          if (dest) {
            dest.send(data.utf8Data);
            console.log("Send to "+destId);
          } else {
            console.error(`Client ${destId} not found`);
          }
      });
    
      conn.on('close', () => {
        delete clients[id];
        console.error(`${id} disconnected`);
      });
    
      clients[id] = conn;
});

httpServer.listen(SignallingPORT, '0.0.0.0',
    () => { console.log(`WS Signalling listening on port:${SignallingPORT}`); });