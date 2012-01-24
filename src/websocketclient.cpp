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
#include <boost/network/protocol/http/client.hpp>
#include <boost/network/uri/uri.hpp>
#include <iostream>

#include <websocketpp/roles/client.hpp>
#include <websocketpp/websocketpp.hpp>

#include "socketio/SocketIOHandler.hpp"

using namespace boost::network;
using websocketpp::client;

int main(int argc, char* argv[]) {
    srandom(time(0));
    std::stringstream socket_io_start;
    socket_io_start << "http://localhost:81/socket.io/1/?t=" << random();
    std::cout << socket_io_start.str() << std::endl;
    
    http::client client_;
    http::client::request request_(socket_io_start.str());
    request_ << header("Connection", "close");
    http::client::response response_ = client_.get(request_);
    std::string socket_io_key = body(response_);
    int first_colon = socket_io_key.find_first_of(":");
    socket_io_key = socket_io_key.substr(0, first_colon);
    std::cout << socket_io_key << std::endl;
    
    std::stringstream uri;
    std::stringstream a;
    uri << "ws://localhost:81/socket.io/1/websocket/" << socket_io_key;// << random();
    std::cout << uri.str() << std::endl;
    if (argc == 2) {
        uri << argv[1];
        std::cout << "kut" << std::endl;
    } else if (argc > 2) {
        std::cout << "Usage: `echo_client test_url`" << std::endl;
    }
    
    try {
        client::handler::ptr handler(new socketio::SocketIOHandler());
        client::connection_ptr con;
        client endpoint(handler);
        std::cout << "hi " << uri << std::endl;
        //endpoint.alog().unset_level(websocketpp::log::alevel::ALL);
        //endpoint.elog().unset_level(websocketpp::log::elevel::ALL);
        endpoint.alog().set_level(websocketpp::log::alevel::ALL);
        con = endpoint.connect(uri.str());
        
        con->add_request_header("User Agent","WebSocket++/0.2.0");
        
        endpoint.run();
        
        //std::cout << "case count: " << boost::dynamic_pointer_cast<echo_client_handler>(handler)->m_case_count << std::endl;
        
        std::cout << "done" << std::endl;
        
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    
    return 0;
}
