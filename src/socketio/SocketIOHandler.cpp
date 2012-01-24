#include "SocketIOHandler.hpp"
#include <boost/network/protocol/http/client.hpp>
#include <boost/network/uri/uri.hpp>

namespace socketio {
    
    using websocketpp::client;
    using namespace boost::network;
    
    SocketIOHandler::SocketIOHandler(const std::string &host)
    :client::handler() {
         this->_host = host;
         
        std::stringstream socketio_token_url;
        socketio_token_url << "http://" << this->_host << "/socket.io/1/?t=" << random();
        
        http::client http_client;
        http::client::request http_request = http::client::request(socketio_token_url.str());
        http_request << header("Connection", "close");
        
        http::client::response http_request_response = http_client.get(http_request);
        std::string socket_io_key = body(http_request_response);
        int first_colon = socket_io_key.find_first_of(":");        
        this->_websocket_token = socket_io_key.substr(0, first_colon);
    }
    
    std::string SocketIOHandler::websocket_uri() const {
        std::stringstream websocket_uri;
        websocket_uri << "ws://" << this->_host << "/socket.io/1/websocket/" << this->_websocket_token;
        return websocket_uri.str();
    }
    
    void SocketIOHandler::on_message(connection_ptr con, message_ptr msg) {
        std::cout << msg->get_payload() << std::endl;
        // con->wait();
    }
    void SocketIOHandler::on_load(connection_ptr connection, client::handler_ptr old_handler) {
            std::cout << " on load lol" << std::endl;
        }
        
    void SocketIOHandler::on_close(connection_ptr connection) {
        std::cout << " on load lol" << std::endl;
    };

    void SocketIOHandler::on_open(connection_ptr con) {
        std::cout << " on open lol" << std::endl;
        std::string a = "5:::{\"name\":\"authentication\",\"args\":[{\"username\":\"admin@amt.de\",\"password\":\"admin\"}]}";
        con->send(a, websocketpp::frame::opcode::TEXT);
    };
    
    void SocketIOHandler::on_fail(connection_ptr con) {
        std::cout << "connection failed" << std::endl;
    }

}