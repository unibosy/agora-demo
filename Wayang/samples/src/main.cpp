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
#include <string.h>
#include <memory>
#include <iostream>
#include <unistd.h>
#include "json/json.h"

#include "commands.h"
#include "agora_rtc_api.h"

using namespace std;
using easywsclient::WebSocket;

//agora infos
string g_appid = "";
int64_t g_uid = 0;
string g_channel = "";
bool b_handler = true;
string g_token = "";
int iCreateEngine = -1;
int channel_profile = 0;//0:com, 1:live

const string URL_WAYANG="ws://qa.agoralab.co:8081/JavaWebSocket/websocket";

WebSocket::pointer g_wsp;
static void on_join_channel_success (const char *channel, uint32_t uid, int elapsed)
{
	fprintf (stderr, "%u logined successfully in %s, elapsed: %d ms\n", uid, channel, elapsed);
    cout<<"on_join_channel_success"<<endl;

    g_wsp->send("\{URI_DEVICE: \"linux_001\",\"cmd\": CMD_JOINCHANNEL_SUCCESS ,\"type\": 4, \"sequence\": 1527584393223\}");

}

static void on_rejoin_channel_success (const char *channel, uint32_t uid, int elapsed) 
{
	fprintf (stderr, "%u rejoin to channel: %s, time offset %d ms\n", uid, channel, elapsed);
}

static void on_warning (const char *channel, int warn, const char *msg) 
{
	fprintf(stderr, "onWarning code: %d, %s, channel=%p\n", warn, msg, channel);
}

static void on_error (const char *channel, int rescode, const char *msg) 
{
	switch (rescode) {
	default:
		fprintf (stderr, "Error in mediasdk: %d, %s, channel=%p\n", rescode, msg, channel);
		break;
	}
}

static void on_user_joined (const char *channel, uint32_t uid, int elapsed) 
{
	fprintf(stderr, "offset %d ms: %u joined the channel, channel=%p\n", elapsed, uid, channel);
}

static void on_user_offline (const char *channel, uint32_t uid, int reason) 
{
	const char *detail = reason == USER_OFFLINE_QUIT ? "quit" : "dropped";
	fprintf (stderr, "User %u %s, channel=%p\n", uid, detail, channel);
}

static void on_connection_lost (const char *channel) 
{
	fprintf (stderr, "onConnectionLost, aborting, channel=%p\n", channel);
}

static void on_rtcstats (const char *channel, const agora_rtcstats_t *stats)
{
	fprintf (stderr, "on_rtcstats: %s \n", channel);
}

static char audio_data [128];
static char video_data [1024];

static void on_video_data  (const char *channel, const uint32_t uid, uint8_t codec, uint8_t stream_id, int is_key_frame, const void *data, size_t len)
{
	printf ("on_video_data: channel %s, user %u, %ld, cmp=%d, codec=%d \n", channel, uid, len, memcmp (data, video_data, sizeof video_data), codec);
}

static void on_audio_data (const char *channel, const uint32_t uid, uint8_t codec, const void *data, size_t len)
{
	printf ("on_audio_data: channel %s, user %u, %ld, cmp=%d\n", channel, uid, len, memcmp (data, audio_data, sizeof audio_data));
}

static void on_user_mute_audio (const char *channel, uint32_t uid, int muted) 
{

}

static void on_user_mute_video (const char *channel, uint32_t uid, int muted)
{

}

static int on_customized_sei (const char *channel, const void **content, int *length)
{

}

static agora_rtc_event_handler_t event_handler = {
    .on_join_channel_success = on_join_channel_success,
	.on_rejoin_channel_success = on_rejoin_channel_success,
	.on_warning = on_warning,
	.on_error = on_error,
	.on_user_joined = on_user_joined,
	.on_user_offline = on_user_offline,
	.on_rtcstats = on_rtcstats,
	.on_connection_lost = on_connection_lost,
    .on_user_mute_audio = on_user_mute_audio,
    .on_user_mute_video = on_user_mute_video,
    .on_customized_sei = on_customized_sei,
    .on_audio_data = on_audio_data,
    .on_video_data = on_video_data
};


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
    string cmd_str = value[CMD].asString();
    cout<<"cmd_str:"<<cmd_str<<endl;
    if(cmd_str == REGISTER) {
        if(0 != value[INFO][ERROR].asInt()) {
            cout<<"register to web server failed!"<<endl;
            return value;
        }else {
            cout<<"register to web server successfully!"<<endl;

        }
    }else if(cmd_str == CREATE) {
        g_appid = value[INFO][APPID].asString();
        g_uid = value[INFO][URI_UID].asInt();
        b_handler = value[INFO][HANDLER].asBool();
        cout <<"g_appid:"<<g_appid<<", uid:"<<g_uid<<", b_handler:"<<b_handler<<endl;
        iCreateEngine = agora_rtc_create (g_appid.c_str(), g_uid, &event_handler);
    }else if(cmd_str == JOINCHANNEL) {
        g_channel = value[INFO][CHANNELNAME].asString();
        g_token = value[INFO][TOKEN].asString();
        cout<<"g_channel:"<<g_channel<<", g_token:"<<g_token<<endl;
        if(!iCreateEngine){
            int ret = -1;
            ret = agora_rtc_join_channel (g_channel.c_str());
            if(ret){
                cout<<"join channel "<<g_channel<<" failed, ret:"<<ret<<endl;
            }else{
                cout<<"join channel "<<g_channel<<" successfully"<<endl;
            }
        }
    }else if(cmd_str == CMD_SETCHANNELPROFILE) {
        channel_profile = value[INFO][URI_PROFILE].asInt();
        cout<<"channel profile:"<<channel_profile<<endl;
        //no api to set this yet
    }else if(cmd_str == CMD_ENABLEVIDEO) {
        int optVlaue = value[INFO][URI_OPTVAL].asInt();
        cout<<"opt value:"<<optVlaue<<endl;
        agora_rtc_enable_video(g_channel.c_str(), optVlaue);
    }else if(cmd_str == CMD_ENABLEAUDIO) {
        int optVlaue = value[INFO][URI_OPTVAL].asInt();
        cout<<"opt value:"<<optVlaue<<endl;
        agora_rtc_enable_audio(g_channel.c_str(), optVlaue);
    }else if(cmd_str == CMD_MUTEAUDIO) {
        int optVlaue = value[INFO][URI_OPTVAL].asInt();
        cout<<"opt value:"<<optVlaue<<endl;
        agora_rtc_mute_audio(g_channel.c_str(), optVlaue);
    }else if(cmd_str == CMD_MUTEVIDEO) {
        int optVlaue = value[INFO][URI_OPTVAL].asInt();
        cout<<"opt value:"<<optVlaue<<endl;
        agora_rtc_mute_video(g_channel.c_str(), optVlaue);
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
    std::shared_ptr<WebSocket> ws(WebSocket::from_url(URL_WAYANG));
    assert(ws);
    ws->send("\{\"device\": \"linux_001\",\"cmd\": \"register\" ,\"type\": 2, \"sequence\": 1527584393223\}");
    cout<<"state:"<<ws->getReadyState()<<endl;
    cout<<"WebSocket::CLOSED is:"<<WebSocket::CLOSED<<endl;
    int count = 0;
    while (ws->getReadyState() != WebSocket::CLOSED) {
        g_wsp = &*ws; // <-- because a unique_ptr cannot be copied into a lambda
        ws->poll();
        ws->dispatch([g_wsp](const std::string & message) {
            printf(">>> %s\n", message.c_str());
            //received messages
            on_message(g_wsp, message); 
        });
    }
    cout<<"web socket has been closed!"<<endl;
#ifdef _WIN32
    WSACleanup();
#endif
    // N.B. - unique_ptr will free the WebSocket instance upon return:
    return 0;
}
