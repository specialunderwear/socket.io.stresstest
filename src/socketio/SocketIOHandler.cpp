#include "SocketIOHandler.hpp"
#include <boost/network/protocol/http/client.hpp>
#include <boost/network/uri/uri.hpp>
#include <boost/format.hpp>
#include <json/json.h>

namespace socketio {
    
    using websocketpp::client;
    using namespace boost::network;
    
    SocketIOHandler::SocketIOHandler(const std::string &host, sequence::ActionSequence &action_sequence)
    :client::handler() {
        _host = host;
        _actions = action_sequence;
        _websocket_token = _get_token(host);
    }
    
    std::string SocketIOHandler::_get_token(const std::string &host) {
        std::stringstream socketio_token_url;
        socketio_token_url << "http://" << host << "/socket.io/1/?t=" << random();

        http::client http_client;
        http::client::request http_request = http::client::request(socketio_token_url.str());
        http_request << header("Connection", "close");
        
        http::client::response http_request_response = http_client.get(http_request);
        std::string socket_io_key = body(http_request_response);
        int first_colon = socket_io_key.find_first_of(":");        
        return socket_io_key.substr(0, first_colon);
    }
    
    Json::Value _parse_message(const std::string &message) {
        std::string json_string = std::string(message.begin()+4, message.end());

        Json::Value root;
        Json::Reader reader;
        bool success = reader.parse(json_string, root);
        if (!success) {
            std::cout  << "Failed to parse message body\n"
                       << reader.getFormattedErrorMessages();
            return Json::Value();
        }
        return root;
    }
    std::string SocketIOHandler::_parse_session_id(const std::string &message) {
        Json::Value root = _parse_message(message);
        return root["args"][0]["SessionID"].asString();
    }
    
    void SocketIOHandler::_log_message(const std::string &message, const boost::format &next_message) {
        Json::StyledWriter writer;
        std::cout << "message received:" << std::endl;
        std::cout << writer.write(_parse_message(message)) << std::endl;
        std::cout << "next message:" << std::endl;
        std::cout << writer.write(_parse_message(next_message.str())) << std::endl;
        std::cout << std::endl;
    }
    
    std::string SocketIOHandler::websocket_uri() const {
        std::stringstream websocket_uri;
        websocket_uri << "ws://" << _host << "/socket.io/1/websocket/" << _websocket_token;
        return websocket_uri.str();
    }
    
    void SocketIOHandler::on_message(connection_ptr con, message_ptr msg) {
        std::string payload = msg->get_payload();
        if (payload.substr(0, 3) == "5::") {
            if (sessionid.empty()) {
                sessionid = _parse_session_id(payload);
            }
            
            std::string next_message = _actions.nextAction();
            if (next_message != "stop") {
                boost::format formatted_message = boost::format(next_message) % sessionid;
                _log_message(payload, formatted_message);
                con->send(formatted_message.str(), websocketpp::frame::opcode::TEXT);
            } else {
                con->close(websocketpp::close::status::NORMAL, "End of test sequence");
            }
        }
    }
    
    void SocketIOHandler::on_load(connection_ptr connection, client::handler_ptr old_handler) {
            std::cout << "connection loaded." << std::endl;
        }
        
    void SocketIOHandler::on_close(connection_ptr connection) {
        std::cout << "connection closed." << std::endl;
    };

    void SocketIOHandler::on_open(connection_ptr con) {
        std::cout << "connection opened." << std::endl;
        con->send(_actions.nextAction(), websocketpp::frame::opcode::TEXT);
    };
    
    void SocketIOHandler::on_fail(connection_ptr con) {
        std::cout << "connection failed" << std::endl;
    }

}