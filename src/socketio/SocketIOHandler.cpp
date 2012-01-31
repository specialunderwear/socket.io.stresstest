#include "SocketIOHandler.hpp"
#include <boost/network/protocol/http/client.hpp>
#include <boost/network/uri/uri.hpp>
#include <boost/format.hpp>
#include <boost/asio.hpp>
#include <json/json.h>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace socketio {
    
    using websocketpp::client;
    using namespace boost::network;
    using boost::asio::ip::tcp;

    SocketIOHandler::SocketIOHandler(const std::string &uri, sequence::ActionSequence &action_sequence)
    :client_tls::handler() {
        int protocol_start = uri.find_first_of("://");
        _host = std::string(uri.begin() + protocol_start + 3, uri.end());
        // if protocol length is 5 it is https.
        _is_secure = protocol_start == 5;
        _actions = action_sequence;
        heartbeat = 0;
    }
    
    SocketIOHandler::~SocketIOHandler() {
        if(heartbeat) {
            delete heartbeat;
        }
    }
    
    void SocketIOHandler::loadToken() {
        _websocket_token = _get_token(_host);
    }

    std::string SocketIOHandler::websocket_uri() const {
        std::stringstream websocket_uri;
        websocket_uri << (_is_secure ? "wss://" : "ws://") << _host << "/socket.io/1/websocket/" << _websocket_token;
        return websocket_uri.str();
    }
    
    /* Callbacks */
    
    void SocketIOHandler::on_message(connection_ptr con, message_ptr msg) {
        _reset_heartbeat(con);

        std::string payload = msg->get_payload();
        if (payload.substr(0, 3) == "5::") {
            if (sessionid.empty()) {
                sessionid = _parse_session_id(payload);
            }
            
            std::string next_message = _actions.nextAction();
            if (next_message != "stop") {
                boost::format formatted_message = boost::format(next_message);
                formatted_message.exceptions(boost::io::all_error_bits ^ ( boost::io::too_many_args_bit | boost::io::too_few_args_bit ));
                formatted_message = formatted_message % sessionid;

                _log_message(payload, formatted_message);
                
                con->send(formatted_message.str(), websocketpp::frame::opcode::TEXT);
            } else {
                con->close(websocketpp::close::status::NORMAL, "End of test sequence ++++++++++++++++++++++++++++++++");
            }
        } else {
            std::cout << "message received: " << payload << std::endl;
        }
    }
    
    void SocketIOHandler::on_close(connection_ptr connection) {
        std::cout << "connection closed." << std::endl;
        heartbeat->cancel();
    };

    void SocketIOHandler::on_open(connection_ptr con) {
        std::cout << "connection opened." << std::endl;

        // turn on keep alive.
        //boost::asio::socket_base::keep_alive keepAlive(true);
        //con->get_socket().set_option(keepAlive);
        
        // since keep alive only pings after 2 hours, also check the socket every SOCKET_IO_HEARTBEAT_INTERVAL
        // if no message was reveived within that time span, a 2:: will be sent to the server to check to socket.
        heartbeat = new boost::asio::deadline_timer(con->get_io_service(), boost::posix_time::seconds(SOCKET_IO_HEARTBEAT_INTERVAL));
        _reset_heartbeat(con);
        
        // run first action
        con->send(_actions.nextAction(), websocketpp::frame::opcode::TEXT);
    };
    
    void SocketIOHandler::on_fail(connection_ptr con) {
        std::cerr << "connection failed" << std::endl;
        exit(1);
    }
    
    boost::shared_ptr<boost::asio::ssl::context> SocketIOHandler::on_tls_init(boost::asio::io_service &io_service) {
        boost::asio::ssl::context *ctx = new boost::asio::ssl::context(io_service, boost::asio::ssl::context::sslv23_client);
        ctx->set_verify_mode(boost::asio::ssl::context::verify_none);
		boost::shared_ptr<boost::asio::ssl::context> p(ctx);
		return p;
    }
    
    /* Private */
        
    std::string SocketIOHandler::_get_token(const std::string &host) {
        std::stringstream socketio_token_url;
        socketio_token_url << (_is_secure ? "https://" : "http://") << host << "/socket.io/1/?t=" << random();

        http::client http_client;
        http::client::request http_request = http::client::request(socketio_token_url.str());
        http_request << header("Connection", "close");
        
        http::client::response http_request_response = http_client.get(http_request);
        
        if (status(http_request_response) == 200) {
            std::string socket_io_key = body(http_request_response);
            int first_colon = socket_io_key.find_first_of(":");
            return socket_io_key.substr(0, first_colon);
        } else {
            std::cerr << "socket.io request failed status_code:" << status(http_request_response) << std::endl;
            exit(1);
        }
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
    
    void SocketIOHandler::_reset_heartbeat(connection_ptr con) {
        heartbeat->expires_from_now(boost::posix_time::seconds(SOCKET_IO_HEARTBEAT_INTERVAL));
        heartbeat->async_wait(
            boost::bind(
                &SocketIOHandler::_on_heartbeat_timeout,
                shared_from_this(),
                con->shared_from_this(),
                boost::asio::placeholders::error
            )
        );
    }
    
    void SocketIOHandler::_on_heartbeat_timeout(connection_ptr con, const boost::system::error_code& error) {
        if (!error) {
            con->send("2::", websocketpp::frame::opcode::TEXT);
        }
    }

    std::string SocketIOHandler::_parse_session_id(const std::string &message) {
        Json::Value root = _parse_message(message);
        return root["args"][0]["SessionID"].asString();
    }
    
    void SocketIOHandler::_log_message(const std::string &message, const boost::format &next_message) {
        Json::StyledWriter writer;
        std::cout << "message received:" << std::endl;
        std::cout << ((message.size() > 4) ? writer.write(_parse_message(message)) : message) << std::endl;
        std::cout << "next message:" << std::endl;
        std::cout << ((next_message.str().size() > 4) ? writer.write(_parse_message(next_message.str())) : next_message.str()) << std::endl;
        std::cout << std::endl;
    }
}