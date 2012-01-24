/*
 * Copyright (c) 2011, Peter Thorson. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the WebSocket++ Project nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL PETER THORSON BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
#include <iostream>

#include <websocketpp/roles/client.hpp>
#include <websocketpp/websocketpp.hpp>

#include "socketio/SocketIOHandler.hpp"
#include "sequence/ActionSequence.hpp"

using websocketpp::client;

int main(int argc, char* argv[]) {
    srandom(time(0));
    
    std::string uri;
    if (argc == 2) {
        uri = argv[1];
    } else {
        std::cout << "Usage: `echo_client hostname:port`" << std::endl;
        exit(0);
    }
    
    try {
        sequence::ActionSequence actions = sequence::ActionSequence("input.json");
        socketio::SocketIOHandler *socket_io_handler = new socketio::SocketIOHandler(uri, actions);
        
        client::handler::ptr handler(socket_io_handler);
        client::connection_ptr con;
        client endpoint(handler);
        std::cout << "websoket_uri " << socket_io_handler->websocket_uri() << std::endl;
        
        //endpoint.alog().unset_level(websocketpp::log::alevel::ALL);
        //endpoint.elog().unset_level(websocketpp::log::elevel::ALL);
        endpoint.alog().set_level(websocketpp::log::alevel::ALL);
        endpoint.elog().set_level(websocketpp::log::alevel::ALL);
        
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
