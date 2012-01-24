#include <iostream>

#include <websocketpp/roles/client.hpp>
#include <websocketpp/websocketpp.hpp>

#include "socketio/SocketIOHandler.hpp"
#include "sequence/ActionSequence.hpp"

using websocketpp::client;

void setlogging(client &endpoint) {
#if DEBUG
    endpoint.alog().set_level(websocketpp::log::alevel::ALL);
    endpoint.elog().set_level(websocketpp::log::alevel::ALL);
#else
    endpoint.alog().unset_level(websocketpp::log::alevel::ALL);
    endpoint.elog().unset_level(websocketpp::log::elevel::ALL);
#endif    
}

int main(int argc, char* argv[]) {
    srandom(time(0));
    
    std::string uri;
    char *input_file;
    
    if (argc == 3) {
        uri = argv[1];
        input_file = argv[2];
    } else {
        std::cout << "Usage: `websocketclient hostname:port input_file_path.json`" << std::endl;
        exit(0);
    }
    
    try {
        sequence::ActionSequence actions = sequence::ActionSequence(input_file);
        socketio::SocketIOHandler *socket_io_handler = new socketio::SocketIOHandler(uri, actions);
        
        std::cout << "websocket_uri " << socket_io_handler->websocket_uri() << std::endl;

        client::handler::ptr handler(socket_io_handler);
        client::connection_ptr con;
        client endpoint(handler);
        
        setlogging(endpoint);
        
        con = endpoint.connect(socket_io_handler->websocket_uri());
        con->add_request_header("User Agent","WebSocket++/0.2.0");
        
        endpoint.run();
        
        //std::cout << "case count: " << boost::dynamic_pointer_cast<echo_client_handler>(handler)->m_case_count << std::endl;
        
        std::cout << "done" << std::endl;
        
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    
    return 0;
}
