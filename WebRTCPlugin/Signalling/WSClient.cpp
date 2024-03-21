#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

#include "WSClient.h"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

namespace signalling {
    const char* host = "localhost";
    const char* port = "7002";

    // The io_context is required for all I/O
    net::io_context ioc;

    // These objects perform our I/O
    tcp::resolver resolver{ ioc };
    websocket::stream<tcp::socket> ws{ ioc };

    // Sends a WebSocket message and prints the response
    void StartWebsocket()
    {
        try
        {
            // Look up the domain name
            auto const results = resolver.resolve(host, port);

            // Make the connection on the IP address we get from a lookup
            auto ep = net::connect(ws.next_layer(), results);

            // Update the host_ string. This will provide the value of the
            // Host HTTP header during the WebSocket handshake.
            // See https://tools.ietf.org/html/rfc7230#section-5.4
            auto uri =  host + ':' + std::to_string(ep.port());

            // Set a decorator to change the User-Agent of the handshake
            ws.set_option(websocket::stream_base::decorator(
                [](websocket::request_type& req)
                {
                    req.set(http::field::user_agent,
                    std::string(BOOST_BEAST_VERSION_STRING) +
                    " websocket-client-coro");
                }));

            // Perform the websocket handshake
            ws.handshake(uri, "/server");
        }
        catch (std::exception const& e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
            return;
        }
        return;
    }

    DelegateOnWSMessage ws_onMessageCB;

    void RegisterOnMessage(DelegateOnWSMessage cb) {
        ws_onMessageCB = cb;
    }

    void SendMessage(char* msg, size_t size) {
        ws.write(net::buffer(msg, size));
    }

    void SendMessage(std::string msg)
    {
        ws.write(net::buffer(msg));
    }

    std::string ReadMessage() {
        beast::flat_buffer buffer;
        ws.read(buffer);
        const char* data = static_cast<const char*>(buffer.data().data());
        std::size_t size = buffer.size();

        std::string str(data, size);
        return str;
    }
}