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

template <typename CLIENT_TYPE>
void connect_and_run(std::string &uri, sequence::ActionSequence &actions) {
    socketio::SocketIOHandler<typename CLIENT_TYPE::handler> *socket_io_handler = new socketio::SocketIOHandler<typename CLIENT_TYPE::handler>(uri, actions);
    socket_io_handler->loadToken();
    std::cout << "websocket_uri " << socket_io_handler->websocket_uri() << std::endl;
    typename CLIENT_TYPE::handler::ptr handler(socket_io_handler);
    CLIENT_TYPE endpoint(handler);
    endpoint.alog().LOGLEVEL();
    endpoint.elog().LOGLEVEL();
    typename CLIENT_TYPE::connection_ptr con = endpoint.connect(socket_io_handler->websocket_uri());
    endpoint.run();
}

int main(int argc, char* argv[]) {
    srandom(time(0));
    
    std::string uri;
    char *input_file;
    
    if (argc == 3) {
        uri = argv[1];
        // strip trailing slash
        std::string::iterator it = uri.end() - 1;
        if (*it == '/') uri.erase(it);
        
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
            connect_and_run<client_tls>(uri, actions);
        } else if (boost::starts_with(uri, "http://")) {
            connect_and_run<client>(uri, actions);
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
