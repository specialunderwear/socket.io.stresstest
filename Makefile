JSON_CPP_SRC = parts/jsoncpp
JSON_CPP_BUILD = build/jsoncpp
NETLIB_LIB_SRC = parts/cpp-netlib/libs/network/src
NETLIB_BUILD = build/netlib
WEBSOCKETPP_SRC = parts/websocketpp/src
WEBSOCKETPP_BUILD = build/websocketpp

INCLUDE_DIRS = -Iinclude
LD_PATH = -Llib

CFLAGS = -O2 -Wall -DDEBUG=0 -Bstatic

all: websocketclient

libs: lib/libjsoncpp.a lib/libnetlib.a lib/libwebsocketpp.a

clean:
	@-rm -fv lib/*
	@-rm -fv build/jsoncpp/*
	@-rm -fv build/netlib/*
	@-rm -fv build/websocketpp/*
	@-rm websocketclient
	@-rm build/*.o

######################################
# JSONCPP BUILD TARGETS
######################################

$(JSON_CPP_BUILD)/json_reader.o: $(JSON_CPP_SRC)/src/lib_json/json_reader.cpp
	g++ -o build/jsoncpp/json_reader.o -c $(CFLAGS) $(INCLUDE_DIRS) $(JSON_CPP_SRC)/src/lib_json/json_reader.cpp

$(JSON_CPP_BUILD)/json_value.o:
	g++ -o build/jsoncpp/json_value.o -c $(CFLAGS) $(INCLUDE_DIRS) $(JSON_CPP_SRC)/src/lib_json/json_value.cpp

$(JSON_CPP_BUILD)/json_writer.o:
	g++ -o build/jsoncpp/json_writer.o -c $(CFLAGS) $(INCLUDE_DIRS) $(JSON_CPP_SRC)/src/lib_json/json_writer.cpp

lib/libjsoncpp.a: $(JSON_CPP_BUILD)/json_reader.o $(JSON_CPP_BUILD)/json_value.o $(JSON_CPP_BUILD)/json_writer.o
	ar -rcv lib/libjsoncpp.a $(JSON_CPP_BUILD)/json_reader.o $(JSON_CPP_BUILD)/json_value.o $(JSON_CPP_BUILD)/json_writer.o
	ranlib lib/libjsoncpp.a

#####################################
# NETLIB BUILD TARGETS
#####################################

$(NETLIB_BUILD)/server_request_parsers_impl.o: $(NETLIB_LIB_SRC)/server_request_parsers_impl.cpp
	c++ $(INCLUDE_DIRS) -o $(NETLIB_BUILD)/server_request_parsers_impl.o $(CFLAGS) -DBOOST_NETWORK_ENABLE_HTTPS=1 -c $(NETLIB_LIB_SRC)/server_request_parsers_impl.cpp

$(NETLIB_BUILD)/parse.o: $(NETLIB_LIB_SRC)/uri/parse.cpp
	c++ $(INCLUDE_DIRS) -o $(NETLIB_BUILD)/parse.o $(CFLAGS) -DBOOST_NETWORK_ENABLE_HTTPS=1 -c $(NETLIB_LIB_SRC)/uri/parse.cpp

$(NETLIB_BUILD)/client.o: $(NETLIB_LIB_SRC)/client.cpp
	c++ $(INCLUDE_DIRS) -o $(NETLIB_BUILD)/client.o $(CFLAGS) -DBOOST_NETWORK_ENABLE_HTTPS=1 -c $(NETLIB_LIB_SRC)/client.cpp

lib/libnetlib.a: $(NETLIB_BUILD)/server_request_parsers_impl.o $(NETLIB_BUILD)/parse.o $(NETLIB_BUILD)/client.o
	ar -rcv lib/libnetlib.a $(NETLIB_BUILD)/client.o $(NETLIB_BUILD)/parse.o $(NETLIB_BUILD)/server_request_parsers_impl.o
	ranlib lib/libnetlib.a

#####################################
# WEBSOCKETPP
#####################################

$(WEBSOCKETPP_BUILD)/network_utilities.o: $(WEBSOCKETPP_SRC)/network_utilities.cpp
	g++ $(WEBSOCKETPP_SRC)/network_utilities.cpp -o $(WEBSOCKETPP_BUILD)/network_utilities.o -c $(CFLAGS) -DNDEBUG $(INCLUDE_DIRS)

$(WEBSOCKETPP_BUILD)/sha1.o: $(WEBSOCKETPP_SRC)/sha1/sha1.cpp
	g++ $(WEBSOCKETPP_SRC)/sha1/sha1.cpp -o $(WEBSOCKETPP_BUILD)/sha1.o -c $(CFLAGS) -DNDEBUG $(INCLUDE_DIRS)

$(WEBSOCKETPP_BUILD)/base64.o: $(WEBSOCKETPP_SRC)/base64/base64.cpp
	g++ $(WEBSOCKETPP_SRC)/base64/base64.cpp -o $(WEBSOCKETPP_BUILD)/base64.o -c $(CFLAGS) -DNDEBUG $(INCLUDE_DIRS)

$(WEBSOCKETPP_BUILD)/md5.o: $(WEBSOCKETPP_SRC)/md5/md5.c
	g++ $(WEBSOCKETPP_SRC)/md5/md5.c -o $(WEBSOCKETPP_BUILD)/md5.o -c $(CFLAGS) -DNDEBUG $(INCLUDE_DIRS)

$(WEBSOCKETPP_BUILD)/uri.o: $(WEBSOCKETPP_SRC)/uri.cpp
	g++ $(WEBSOCKETPP_SRC)/uri.cpp -o $(WEBSOCKETPP_BUILD)/uri.o -c $(CFLAGS) -DNDEBUG $(INCLUDE_DIRS)

$(WEBSOCKETPP_BUILD)/hybi_header.o: $(WEBSOCKETPP_SRC)/processors/hybi_header.cpp
	g++ $(WEBSOCKETPP_SRC)/processors/hybi_header.cpp -o $(WEBSOCKETPP_BUILD)/hybi_header.o -c $(CFLAGS) -DNDEBUG $(INCLUDE_DIRS)

$(WEBSOCKETPP_BUILD)/data.o: $(WEBSOCKETPP_SRC)/messages/data.cpp
	g++ $(WEBSOCKETPP_SRC)/messages/data.cpp -o $(WEBSOCKETPP_BUILD)/data.o -c $(CFLAGS) -DNDEBUG $(INCLUDE_DIRS)

lib/libwebsocketpp.a: $(WEBSOCKETPP_BUILD)/data.o $(WEBSOCKETPP_BUILD)/hybi_header.o $(WEBSOCKETPP_BUILD)/md5.o $(WEBSOCKETPP_BUILD)/base64.o $(WEBSOCKETPP_BUILD)/sha1.o $(WEBSOCKETPP_BUILD)/network_utilities.o $(WEBSOCKETPP_BUILD)/uri.o
	ar -rcv lib/libwebsocketpp.a $(WEBSOCKETPP_BUILD)/network_utilities.o $(WEBSOCKETPP_BUILD)/sha1.o $(WEBSOCKETPP_BUILD)/base64.o $(WEBSOCKETPP_BUILD)/md5.o $(WEBSOCKETPP_BUILD)/uri.o $(WEBSOCKETPP_BUILD)/hybi_header.o $(WEBSOCKETPP_BUILD)/data.o
	ranlib lib/libwebsocketpp.a

#####################################
# WEBSOCKETCLIENT
#####################################

build/websocketclient.o: src/websocketclient.cpp
	g++ src/websocketclient.cpp $(INCLUDE_DIRS) $(CFLAGS) -c -o build/websocketclient.o

build/ActionSequence.o: src/sequence/ActionSequence.cpp
	g++ src/sequence/ActionSequence.cpp $(INCLUDE_DIRS) $(CFLAGS) -c -o build/ActionSequence.o

websocketclient: build/websocketclient.o build/ActionSequence.o libs
	g++ -o websocketclient $(LD_PATH) build/websocketclient.o build/ActionSequence.o -lwebsocketpp -lnetlib -ljsoncpp -lboost_system -lboost_thread -lboost_date_time -lboost_regex -lboost_random -lboost_program_options -pthread -lssl -lcrypto
	strip websocketclient

.PHONY : clean
