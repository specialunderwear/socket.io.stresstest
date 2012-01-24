#ifndef SOCKET_IO_HANDLER
#define SOCKET_IO_HANDLER

#include <websocketpp/roles/client.hpp>
#include <websocketpp/websocketpp.hpp>

using websocketpp::client;

namespace socketio {
    
    class SocketIOHandler : public client::handler {
    private:
        std::string _host;
        std::string _websocket_token;

    public:
        SocketIOHandler(const std::string &host);
        std::string websocket_uri() const;
        void on_message(connection_ptr con, message_ptr msg);        
        virtual void on_load(connection_ptr connection, client::handler_ptr old_handler);
        virtual void on_close(connection_ptr connection);
        virtual void on_open(connection_ptr con);
        virtual void on_fail(connection_ptr con);
    };

}

#endif