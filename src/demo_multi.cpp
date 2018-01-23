
#include "agora_sig.h"
#include <iostream>
#include <vector>
#include <string.h>
#include <sstream>
#include <pthread.h>
#include <sys/timerfd.h>
#include <poll.h>
#include <thread>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <memory>
#include <map>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include "utils.h"

using namespace agora_sdk_cpp;
using namespace std;

typedef struct{
  string name;
  uint32_t uid;
}User;

vector<User> peers;

//map<string, string> p2pHistory;//user-user
static int countOk = 0;
static int countFailed = 0;
string g_vendor = "";
string appCertificateId = "";

string g_username = "";
string g_token = "";
uint32_t g_uid = 0;
string g_channel = "";

uint32_t g_call_n = 0;

bool g_queried = false;
bool g_reconnect = false;
//string dbg_ip = "123.125.184.7";
string dbg_ip = "";



static std::string generateSignallingToken(const std::string &account, const std::string &appId, const std::string appCertificateId, int expiredTsInSeconds){
  std::ostringstream ostr;
  ostr << account.c_str()<<appId.c_str()<<appCertificateId.c_str()<<expiredTsInSeconds;
  string md5StrSrc = ostr.str();
  std::string md5StrDest = agora2::tools::md5(md5StrSrc);
  std::ostringstream token;
  token<<1<<":"<<appId.c_str()<<":"<<expiredTsInSeconds<<":"<<md5StrDest.c_str();
  string md5DestStr = token.str();
  return md5DestStr;
}

class CallBack : public ICallBack {
private:
  IAgoraAPI *agora ;
  string name;
public:
  CallBack(IAgoraAPI *_agora, string _name):agora(_agora),name(_name)
  {
  }
  void do_login(){
    string token = "_no_need_token";
#if 0
  if(!appCertificateId.empty()){
    time_t ltime;
    time(&ltime);
    int expiredSecond = ltime + 3600;
    token = generateSignallingToken(g_username, g_vendor, appCertificateId, expiredSecond);
  }
#endif
  cout << "Login as name:" << name << " ..." << endl;
  agora->login(g_vendor.data(),g_vendor.size(),name.data(),name.size(),token.data(),token.size(),g_uid,"",0);
  }
  void do_logout(){
    agora->logout();
  }
  void destory(){
    agora->destory();
  }
  virtual void onLoginSuccess(uint32_t uid, int fd) override {
    cout << "onLoginSuccess" <<",uid:"<<uid<<endl;
    countOk++;
    cout<<"countOk:"<<countOk<<endl;
    /*if (g_channel != ""){
      cout << "Ready to start call :"<< endl;
      cout << "uid:"<<uid<<" Join channel:" << g_channel << " ..." << endl;
      agora->channelJoin(g_channel.data(), g_channel.size());
      }*/
    /*cout<<"online:"<<agora->isOnline()<<",pointer:"<<&agora<<endl;
      if(g_queried){
      if (g_reconnect) agora->logout();
      }else{
      agora->channelQueryUserNum(g_channel.data(), g_channel.size());
      }*/
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
      if (g_reconnect)
      {
        cout<<"[Warn]:onChannelQueryUserNumResult g_reconnect is flase,now logout!"<<endl;
        agora->logout();
      }
    }

  }

  virtual void onLoginFailed(int ecode)  override{
    cout << "[Errot]:onLoginFailed : ecode =" << ecode << endl;
    countFailed++;
    cout<<"countFailed:"<<countFailed<<endl;
    //do_login();
  }

  virtual void onLogout(int ecode)  override{
    cout << "[CB]:onLogout:ecode="<< ecode << endl;
    //do_login();
  }

  bool call_next(){
    if (g_call_n<peers.size()){
      User &u = peers[g_call_n++];
      if (u.uid == 1){
        cout << "Invite phone : " << u.name << " to channel ..." << endl;
        agora->channelInvitePhone(g_channel.data(), g_channel.size(), u.name.data(), u.name.size());
      }else{
        cout << "Invite user : " << u.name << "(" << u.uid << ") to channel ..." << endl;
        agora->channelInviteUser(g_channel.data(), g_channel.size(),u.name.data(), u.name.size(),u.uid);
      }
      return true;
    }
    return false;
  }

  virtual void onChannelJoined(char const * channelID, size_t channelID_size)  override{
    cout << "Join channel successfully. this channelID::" << channelID<<",channel size:"<<channelID_size<<endl;
    // send channel msg
#if 0 
    if (1){
      string msg = string("hello everyone I'm ") + g_username;
      cout << "send channel msg :" << msg << endl;
      string msgID = "join_message";
      agora->messageChannelSend(channelID, channelID_size,msg.data(), msg.size(),msgID.data(), msgID.size());
    }
    // send invite
    while(call_next()){
    }
#endif
  }
  virtual void onChannelLeaved(char const * channelID, size_t channelID_size,int ecode)  override{
    cout << "onChannelJoined : " << channelID << ":" << channelID_size << endl;
  }

  virtual void onChannelUserJoined(char const * account, size_t account_size,uint32_t uid)  override{
    cout << "Event onChannelUserJoined: " << account << ":" << uid << " joined" << endl;
    // send Instant msg
    if (account != g_username){
      string msg;
      msg += "Hi " + string(account,account_size) + ", I'm " + g_username;
      string msgID = "first_Instant_msg";
      agora->messageInstantSend(account, account_size,uid,msg.data(), msg.size(),msgID.data(), msgID.size());
    }
  }
  virtual void onChannelUserLeaved(char const * account, size_t account_size,uint32_t uid)  override{
    cout << "Event onChannelUserLeaved:" << account << ":" << uid << " leaved" <<  endl;
  }
  virtual void onChannelUserList(int n, char **accounts, uint32_t* uids)  override{
    cout << "Channel User list:"<<  endl;
    for(int i=0;i<n;i++){
      cout << accounts[i] << ":" << uids[i] <<  endl;
    }
    //agora->channelLeave(g_channel.data(), g_channel.size());
  }

  virtual void onInviteReceived(char const * channelID, size_t channelID_size,char const * account, size_t account_size,uint32_t uid, char const * extra, size_t extra_size)  override{
    cout << "Received invitation from account:" << account << ", channel : " << channelID << endl;
    cout << "Joining Channel:" << channelID << endl;
    agora->channelJoin(channelID, channelID_size);
    cout << "Accept invitaction from " << account << endl;
    agora->channelInviteAccept(channelID, channelID_size,account, account_size,uid);
  }

  virtual void onInviteReceivedByPeer(char const * channelID, size_t channelID_size,char const * account, size_t account_size,uint32_t uid)  override{
    cout << "onInviteReceivedByPeer Invitation received by " << account << ",channelID:"<<channelID<<endl;
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
    //cout << "[CB]:LOG------------------:" << txt << endl;
  }

  virtual void onMessageInstantReceive(char const * account, size_t account_size,uint32_t uid,char const * msg, size_t msg_size)  override{
    //cout << "onMessageInstantReceive from " << account << ":" << uid << " " << msg << endl;
  }
  virtual void onMessageChannelReceive(char const * channelID, size_t channelID_size,char const * account, size_t account_size,uint32_t uid,char const * msg, size_t msg_size)  override{
    //cout << "onMessageChannelReceive " << channelID << " from " << account << ":" << uid << " " << msg << endl;
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

string operator +(string const &a, int i){
  ostringstream os;
  os << a << i;
  return os.str();
}
#define N 2
IAgoraAPI *agoras [N];
CallBack *cb[N];
std::thread threads[N];
std::string firstName = "";
typedef map<string, IAgoraAPI*> NameAgoraMap_t;
NameAgoraMap_t NameAgoraMap;

IAgoraAPI* getSpecialAgoraAPI(const string& name){
  NameAgoraMap_t::iterator iter = NameAgoraMap.find(name);
  if( iter != NameAgoraMap.end()){
    return iter->second;
  }
}

void run_start()
{
#if 1
  cout<<"run_start"<<endl; 
  agoras[0]->start();
  cout<<""<<endl;
#endif
}
void p2pHelp(){
  cout<<"sendmsg $message"<<endl;
  cout<<"quitp2p"<<endl;
}
void help(){
  cout<<"Please input ' switchp2p $somebody ' to chat with someone!"<<endl;
  cout<<"Please input ' switchp2c $oneChannel ' to chat in the channel!"<<endl;
  cout<<"Please input ' changeAccout $account' to change current account to special one! [default is the first account]"<<endl;
  cout<<"Please input quit to logout!"<<endl;
}
void splitString(const string& names, vector<string>& v, const string& c)
{   
  cout<<"names:"<<names<<",c:"<<c<<endl;
  string::size_type pos1, pos2;
  pos2 = names.find(c);
  pos1 = 0;
  while(string::npos != pos2)
  {
    cout << "has :"<<endl;
    v.push_back(names.substr(pos1, pos2-pos1));   
    pos1 = pos2 + c.size();
    pos2 = names.find(c, pos1);
  }
  if(pos1 != names.length())
    v.push_back(names.substr(pos1));
}
void do_business(){
  pid_t pid = getpid();
  cout << "thead:"<<this_thread::get_id()<<endl;
  cout<<"pid:"<<pid<<endl;
  char commands[999] = {0};
  IAgoraAPI* agora = getSpecialAgoraAPI(firstName);
  if(!agora){
    cout<<"[fatal]:no user joined!"<<endl;
    return;
  }
  string dump;
  while(fgets(commands, sizeof(commands)-1, stdin)){
    if(strncmp(commands,"switchp2p",strlen("switchp2p"))==0){//switch p2p chat!
      istringstream is(commands);
      std::string dump, account = "";
      is>>dump>>account;
      cout<<"chat with "<< account <<" now"<<endl;
      cout<<"history..."<<endl;
      while(fgets(commands, sizeof(commands)-1, stdin))
      {
        if(strncmp(commands,"sendmsg",strlen("sendmsg"))==0){ //sendmsg &msg 
          string msg,MsgID = "";
          istringstream is(commands);
          is>>dump>>msg;
          cout<<"send to:"<<account<<", message is:"<<msg<<endl;
          agora->messageInstantSend(account.c_str(), size_t(1),0, msg.data(), msg.size(),MsgID.data(), size_t(1));
        }
        else if(strncmp(commands,"quitp2p",strlen("quitp2p"))==0){
          cout<<"quit p2p chat now..."<<endl;
          break;
        }
        else if(strncmp(commands,"helpp2p",strlen("helpp2p"))==0){
          p2pHelp();
        }
        else{
          cout<<"Please input helpp2p!"<<endl;
          continue;
        }
        usleep(1000*15);//sleep 15 ms
      }
    }
    else if(strncmp(commands,"switchp2c",strlen("switchp2c"))==0){//switch p2c chat!
      istringstream is(commands);
      string channel;
      is>>dump>>channel;
      cout<<"switchp2c channel:"<<channel<<endl;
      //auto join this channel!
      agora->channelJoin(channel.data(), channel.size());

      while(fgets(commands, sizeof(commands)-1, stdin)){

      }

      //agora->messageChannelSend(channelID.c_str(), 1,msg.data(), msg.size(),msgID.data(), msgID.size());

    }
    else if(strncmp(commands, "quit", strlen("quit")) == 0){
      istringstream is(commands);
      cout<<"quit"<<endl;
      agora->logout();
      break;
    }
    else if(strncmp(commands,"changeac",strlen("changeac"))==0){ //change current accout to special one
      istringstream is(commands);
      std::string name,dump;
      is>>dump>>name;
      cout<<"start to use this account:"<<name<<endl;
      agora = getSpecialAgoraAPI(name);
      if(!agora) {
        cout<<"not find this account info!"<<endl;
        continue;
      }
      cout<<"this account:"<<name<<" is online? "<<agora->isOnline()<<",and change this account successfully!"<<endl;
    }
    else if(strncmp(commands,"help",strlen("help"))==0){
      help();
    }
    else{
      cout<<"Please input help!"<<endl;
      continue;
    }
    //fprintf(stdout, "%s %d\n", commands, strlen(commands));
    usleep(10000);//sleep 10 ms
  }
}

pthread_t id_1;
int main(int argc, char** argv){
  cout<<"main argc:"<<argc<<endl;
  if (argc<4){
    cout<<"Usage:"<<endl;
    cout<<"./sig_demo_mul appId name1 appCertificateId"<<endl;
    return 0;
  }
  g_vendor = argv[1];
  std::string str_name = argv[2];
#if 0
  if(names.find(" ") != std::string::npos){
    cout<<"[Error]:name connot contain space..."<<endl;
    return 0;
  }
  if(names.empty()){
    cout<<"name connot be empty"<<endl;
    return 0;
  }
  appCertificateId = argv[3];
  cout<<"vendor appid:"<<argv[1]<<",base name:"<<names<<",appCertificateId:"<<appCertificateId<<endl;
  //split names
  vector<string> v;
#if 0
  if(names.find(":") != std::npos){
    v[0] = names;
  }else{
#endif
  splitString(names, v,":");
  for(vector<string>::size_type i = 0; i != v.size(); ++i){
    cout << "name:"<< v[i] << ";";
  }
  //}
  //better to set N by name size!  
#endif	 
  char temp[10] = {0};
  for (int i=0;i<N;i++){
    usleep(30*1000);
    if(i>=500){
      usleep(5*1000*1000);
      cout<<"reach 500 instance!!!!"<<endl;
      int j = i-16;
      cb[j]->destory();
      cout<<"log out 499"<<endl;

      usleep(4000*1000);
    }
    agoras[i] = createAgoraSDKInstanceCPP();
    if(agoras[i] == NULL){
      usleep(2*1000*1000);
      return 0;
    }
    sprintf(temp, "%d", i); 
    std::string myname = str_name + std::string(temp);
    std::cout<<"str_name:"<<myname<<std::endl;
    cb[i] = new CallBack(agoras[i], myname);
    agoras[i]->callbackSet(cb[i]);
    cb[i]->do_login();

  }
  // set default instance to control onLog
  IAgoraAPI *agora = getAgoraSDKInstanceCPP();
  agora->callbackSet(new CallBack(agora, "default"));
  pid_t pid = getpid();
  cout<<"this pid:"<<pid<<endl;
  cout << "thead:"<<this_thread::get_id()<<endl;
  // run signal event loop , or start a new thread to do it
  agoras[0]->start();
  while(1){
    usleep(1000*10);
  }
  //do_business(agora);
  cout << "Bye" << endl;
}

