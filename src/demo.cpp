
#include "agora_sig.h"
#include <iostream>
#include <vector>
#include <string.h>

using namespace agora_sdk_cpp;
using namespace std;

IAgoraAPI *agora ;
IAgoraAPI* createAgoraSDKInstance();


typedef struct{
    string name;
    uint32_t uid;
}User;

vector<User> peers;

string g_vendor = "6D7A26A1D3554A54A9F43BE6797FE3E2";
string g_username = "test";
string g_token = "";
uint32_t g_uid = 0;
string g_channel = "";

int g_call_n = 0;

bool g_queried = false;
bool g_reconnect = false;
string dbg_ip = 
	""
//	"192.168.99.82"
//  "122.141.244.82"
  "123.125.184.6"
;
//string dbg_ip = "";

static void do_login(){
    cout << "Login as " << g_username << " ..." << endl;
	agora->login(
		g_vendor.data(),
		g_vendor.size(),
		g_username.data(),
		g_username.size(),
		g_token.data(),
		g_token.size(),
		g_uid,
		"",
		0);
}

class CallBack : public ICallBack{
    virtual void onLoginSuccess(uint32_t uid, int fd) override {
        cout << "onLoginSuccess" << endl;

        if (g_channel != ""){
            cout << "Ready to start call : " << endl;
            cout << "Join channel : " << g_channel << " ..." << endl;
            agora->channelJoin(g_channel.data(), g_channel.size());
        }


        if (g_queried){
            if (g_reconnect) agora->logout();
        }else{
            agora->channelQueryUserNum(g_channel.data(), g_channel.size());
        }


        // agora->messageAppSend("{\"message\":{\"id\":3,\"timestamp\":1435635235511,\"data\":{\"uids\":[7914]},\"type\":3,\"cmd\":\"getProfiles\"}}", "000");
        // agora->messagePushSend(g_username, g_uid, "hello yy", "000");
    }

    virtual void onMessageAppReceived(char const * msg, size_t msg_size)  override{
        cout << "onMessageAppReceived " << msg << endl;
    }

    virtual void onChannelQueryUserNumResult(char const * channelID, size_t channelID_size,int ecode,int num)  override {
        cout << "onChannelQueryUserNumResult " << channelID << " " << ecode << " " << num << endl;

        if (ecode==0){
            g_queried = true;
			if (g_reconnect) agora->logout();
        }

    }

    virtual void onLoginFailed(int ecode)  override{
        cout << "onLoginFailed : ecode = " << ecode << endl;

        do_login();
    }

    virtual void onLogout(int ecode)  override{
        cout << "onLogout : ecode = " << ecode << endl;

        do_login();
    }

	bool call_next(){
		if (g_call_n<peers.size()){
			User &u = peers[g_call_n++];
            if (u.uid == 1){
                cout << "Invite phone : " << u.name << " to channel ..." << endl;
                agora->channelInvitePhone(g_channel.data(), g_channel.size(), u.name.data(), u.name.size());
            }else{
                cout << "Invite user : " << u.name << "(" << u.uid << ") to channel ..." << endl;
                agora->channelInviteUser(g_channel.data(), g_channel.size(),
										 u.name.data(), u.name.size(),
										 u.uid);
            }
			return true;
		}
		return false;
	}

    virtual void onChannelJoined(char const * channelID, size_t channelID_size)  override{
        cout << "Join channel successfully." << endl;

        // send channel msg
		if (1){
			string msg = string("hello everyone I'm ") + g_username;
			cout << "send channel msg :" << msg << endl;
			string msgID = "join_message";
			agora->messageChannelSend(
				channelID, channelID_size,
				msg.data(), msg.size(),
				msgID.data(), msgID.size()
				);
		}

        // send invite
		while(call_next());

		/*
		for(list<User>::iterator it = peers.begin(); it!=peers.end(); it++){
			User &u = *it;
            if (u.uid == 1){
                cout << "Invite phone : " << u.name << " to channel ..." << endl;
                agora->channelInvitePhone(g_channel, u.name);
            }else{
                cout << "Invite user : " << u.name << "(" << u.uid << ") to channel ..." << endl;
                agora->channelInviteUser(g_channel, u.name, u.uid);
            }
        }
		*/

//    	agora->channelInviteUser("room1", "aa");
    }

    virtual void onChannelUserJoined(char const * account, size_t account_size,uint32_t uid)  override{
        cout << "Event : " << account << ":" << uid << " joined" << endl;


        // send Instant msg
        if (account != g_username){
            string msg ;
            msg += "Hi " + string(account,account_size) + ", I'm " + g_username;
			string msgID = "first_Instant_msg";
            agora->messageInstantSend(
				account, account_size,
				uid,
				msg.data(), msg.size(),
				msgID.data(), msgID.size());
        }
    }
    virtual void onChannelUserLeaved(char const * account, size_t account_size,uint32_t uid)  override{
        cout << "Event : " << account << ":" << uid << " leaved" <<  endl;
    }

    virtual void onChannelUserList(int n, char **accounts, uint32_t* uids)  override{
        cout << "Channel User list : " <<  endl;
        for(int i=0;i<n;i++){
            cout << accounts[i] << ":" << uids[i] <<  endl;
        }


		//agora->channelLeave(g_channel.data(), g_channel.size());

    }

    virtual void onInviteReceived(char const * channelID, size_t channelID_size,char const * account, size_t account_size,uint32_t uid, char const * extra, size_t extra_size)  override{
        cout << "Received invitation from " << account << " join channel : " << channelID << endl;

        cout << "Joining " << channelID << endl;
        agora->channelJoin(channelID, channelID_size);

        cout << "Accept invitaction from " << account << endl;
        agora->channelInviteAccept(
			channelID, channelID_size,
			account, account_size,
			uid);
    }

    virtual void onInviteReceivedByPeer(char const * channelID, size_t channelID_size,char const * account, size_t account_size,uint32_t uid)  override{
        cout << "Invitation received by " << account << "" << endl;
    }

    virtual void onInviteAcceptedByPeer(char const * channelID, size_t channelID_size,char const * account, size_t account_size,uint32_t uid, char const * extra, size_t extra_size)  override{
        cout << "Invitation acceptd by " << account << endl;

		call_next();
    }

    virtual void onInviteRefusedByPeer(char const * channelID, size_t channelID_size,char const * account, size_t account_size,uint32_t uid, char const * extra, size_t extra_size)  override{
        cout << "Invitation refused by " << account << endl;
    }

    virtual void onInviteEndByPeer(char const * channelID, size_t channelID_size,char const * account, size_t account_size,uint32_t uid, char const * extra, size_t extra_size)  override{
        cout << "Invitation end by " << account << endl;
    }

    virtual void onLog(char const * txt, size_t txt_size)  override{
    	cout << "LOG:" << txt << endl;
    }

    virtual void onMessageInstantReceive(char const * account, size_t account_size,uint32_t uid,char const * msg, size_t msg_size)  override{
        cout << "onMessageInstantReceive from " << account << ":" << uid << " " << msg << endl;
    }
    virtual void onMessageChannelReceive(char const * channelID, size_t channelID_size,char const * account, size_t account_size,uint32_t uid,char const * msg, size_t msg_size)  override{
        cout << "onMessageChannelReceive " << channelID << " from " << account << ":" << uid << " " << msg << endl;
    }
};

uint32_t my_atol(char *n){
	uint32_t x=0;
	while(*n){
		x = x*10 + (*n)-'0';
		n++;
	}
	return x;
}

int main(int argc, char** argv){
	agora = getAgoraSDKInstanceCPP();
	agora->callbackSet(new CallBack());
	ICallBack *cb = agora->callbackGet();
/*
    if (argc<6){
        printf(" Usage : demo dbgip [vendorKey] [userName] [uid] [token] \n");
        printf("         demo dbgip [vendorKey] [userName] [uid] [token] [channelName] [peerName] [peerUid] ... \n");
        printf("\n");
		exit(-1);
    }
    int i=1;
    dbg_ip = argv[i++];
    g_vendor = argv[i++];
    g_username = argv[i++];
    g_uid = my_atol(argv[i++]);
    g_token = argv[i++];

    if (argc>6){
        g_channel = argv[6];

        for(int i=7;i<argc;i+=2){
            User user;
            user.name = argv[i];
            user.uid = my_atol(argv[i+1]);
            peers.push_back(user);
        }
    }
*/
	if (dbg_ip!=""){
		agora->dbg("dbgip", strlen("dbgip"), dbg_ip.c_str(), dbg_ip.size());
	}

  string a = "lbs-foshan6.sig.agora.io";
  agora->dbg("lbss", strlen("lbss"), a.data(), a.size());

    do_login();
	agora->start();
    cout << "Bye" << endl;
}

