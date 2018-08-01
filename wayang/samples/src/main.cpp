// Compile with:
// g++ -std=gnu++0x example-client-cpp11.cpp -o example-client-cpp11
#include "easywsclient.hpp"
//#include "easywsclient.cpp" // <-- include only if you don't want compile separately
#ifdef _WIN32
#pragma comment( lib, "ws2_32" )
#include <WinSock2.h>
#endif
#include <assert.h>
#include <stdio.h>
#include <string>
#include <memory>
#include <iostream>
#include <unistd.h>
#include "json/json.h"
using namespace std;
using easywsclient::WebSocket;

const string URL_WAYANG="ws://qa.agoralab.co:8081/JavaWebSocket/websocket";
const char* const MY_TEST="";

const string REGISTER="register";

Json::Value str2Json(const std::string& str);
void on_message(WebSocket::pointer wsp,const std::string& str) {
    Json::Value value;
    value = str2Json(str);
}

Json::Value str2Json(const std::string& str) {
    Json::Reader reader;
    Json::Value value;
    if(!reader.parse(str, value)) {
        cout<<"parser error"<<endl;
        return value;
    }
    string cmd_str = value["cmd"].asString();
    if(cmd_str == REGISTER) {
        if(0 != value["info"]["error"].asInt()) {
            cout<<"register to web server failed!"<<endl;
            return value;
        }else {
            cout<<"register to web server successfully!"<<endl;

        }
    }

    return value;
}

int main(int argc, char* argv[])
{
#ifdef _WIN32
    INT rc;
    WSADATA wsaData;

    rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (rc) {
        printf("WSAStartup Failed.\n");
        return 1;
    }
#endif
    std::shared_ptr<WebSocket> ws(WebSocket::from_url(URL_WAYANG.c_str()));
    assert(ws);
    ws->send("\{\"device\": \"UNIBOMAC\",\"cmd\": \"register\" ,\"type\": 2, \"sequence\": 1527584393223\}");
    cout<<"state:"<<ws->getReadyState()<<endl;
    cout<<"WebSocket::CLOSED is:"<<WebSocket::CLOSED<<endl;
    int count = 0;
    while (ws->getReadyState() != WebSocket::CLOSED) {
        WebSocket::pointer wsp = &*ws; // <-- because a unique_ptr cannot be copied into a lambda
        ws->poll();
        ws->dispatch([wsp](const std::string & message) {
            printf(">>> %s\n", message.c_str());
            //received messages
            on_message(wsp, message); 
        });
    }
    cout<<"web socket has been closed!"<<endl;
#ifdef _WIN32
    WSACleanup();
#endif
    // N.B. - unique_ptr will free the WebSocket instance upon return:
    return 0;
}
