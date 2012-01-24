#include "SocketIOHandler.hpp"

namespace socketio {
    
    using websocketpp::client;
    
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