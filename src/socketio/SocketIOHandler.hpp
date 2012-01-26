#ifndef SOCKET_IO_HANDLER
#define SOCKET_IO_HANDLER

#define BOOST_NETWORK_ENABLE_HTTPS 1
#define BOOST_NETWORK_HTTP_CLIENT_DEFAULT_TAG tags::http_default_8bit_tcp_resolve
#define SOCKET_IO_HEARTBEAT_INTERVAL 25

#include <websocketpp/roles/client.hpp>
#include <websocketpp/websocketpp.hpp>
#include "../sequence/ActionSequence.hpp"
#include <boost/enable_shared_from_this.hpp>

using websocketpp::client;

namespace socketio {
    
    class SocketIOHandler : public client::handler, public boost::enable_shared_from_this<SocketIOHandler> {
    private:
        std::string sessionid;
        bool _is_secure;
        std::string _host;
        std::string _websocket_token;
        sequence::ActionSequence _actions;
        boost::asio::deadline_timer *heartbeat;

        std::string _get_token(const std::string &uri);
        std::string _parse_session_id(const std::string &message);
        void _log_message(const std::string &message, const boost::format &next_message);
        void _reset_heartbeat(connection_ptr con);
        void _on_heartbeat_timeout(connection_ptr con, const boost::system::error_code& error);
    public:
        SocketIOHandler(const std::string &host, sequence::ActionSequence &action_sequence);
        ~SocketIOHandler();
        void loadToken();
        std::string websocket_uri() const;
        void on_message(connection_ptr con, message_ptr msg);        
        virtual void on_close(connection_ptr connection);
        virtual void on_open(connection_ptr con);
        virtual void on_fail(connection_ptr con);
    };

}

#endif