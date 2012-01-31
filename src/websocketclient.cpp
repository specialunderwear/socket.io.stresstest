#include <iostream>

#include <boost/algorithm/string/predicate.hpp>

#include <websocketpp/sockets/tls.hpp>
#include <websocketpp/roles/client.hpp>
#include <websocketpp/websocketpp.hpp>

#include "socketio/SocketIOHandler.hpp"
#include "sequence/ActionSequence.hpp"

#define USAGE "Usage: `websocketclient (http|https)://hostname:port input_file_path.json`"

#if DEBUG
#define LOGLEVEL(...) set_level(websocketpp::log::alevel::ALL)
#else
#define LOGLEVEL(...) unset_level(websocketpp::log::alevel::ALL)
#endif    

using websocketpp::client;
using websocketpp::client_tls;


int main(int argc, char* argv[]) {
    srandom(time(0));
    
    std::string uri;
    char *input_file;
    
    if (argc == 3) {
        uri = argv[1];
		std::string::iterator it = uri.end() - 1;
	    if (*it == '/')
	    {
	         uri.erase(it);
	    }
		std::cout << uri << std::endl;
        input_file = argv[2];
    } else {
        std::cout << USAGE << std::endl;
        return 0;
    }
    
    try {
        // load action sequence from input file.
        sequence::ActionSequence actions = sequence::ActionSequence(input_file);
		
		if (boost::starts_with(uri, "https://")) {
	        socketio::SocketIOHandler<client_tls::handler> *socket_io_handler = new socketio::SocketIOHandler<client_tls::handler>(uri, actions);
			socket_io_handler->loadToken();
			std::cout << "websocket_uri " << socket_io_handler->websocket_uri() << std::endl;
			client_tls::handler::ptr handler(socket_io_handler);
	        client_tls endpoint(handler);
			endpoint.alog().LOGLEVEL();
			endpoint.elog().LOGLEVEL();
	        client_tls::connection_ptr con = endpoint.connect(socket_io_handler->websocket_uri());
			con->add_request_header("User Agent","WebSocket++/0.2.0");
			endpoint.run();
		} else if (boost::starts_with(uri, "http://")) {
			socketio::SocketIOHandler<client::handler> *socket_io_handler = new socketio::SocketIOHandler<client::handler>(uri, actions);
			socket_io_handler->loadToken();
			std::cout << "websocket_uri " << socket_io_handler->websocket_uri() << std::endl;
			client::handler::ptr handler(socket_io_handler);
	        client endpoint(handler);
			endpoint.alog().LOGLEVEL();
			endpoint.elog().LOGLEVEL();
	        client::connection_ptr con = endpoint.connect(socket_io_handler->websocket_uri());
			con->add_request_header("User Agent","WebSocket++/0.2.0");
			endpoint.run();
		} else {
			std::cout << USAGE << std::endl;
			return 0;
		}
        
        std::cout << "done" << std::endl;
        
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
