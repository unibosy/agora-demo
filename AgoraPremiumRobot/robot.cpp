#include <sys/time.h>
#include <sys/timerfd.h>
#include <poll.h>
#include <signal.h>
#include <cstring>
#include <cmath>
#include <cinttypes>
#include <iostream>

#include "robot.h"

#include "base/safe_log.h"
#include "base/packer.h"
#include "base/process.h"
#include "base/time_util.h"

using namespace std;

namespace agora {
using namespace rtc;
using namespace base;

namespace pstn {

atomic_bool_t robot::term_sig;

robot::robot(settings_t settings):settings_(settings)
{
  dump_settings();

  rtc_engine = NULL;
  joined_ = false;
}

robot::~robot() {
  if (rtc_engine)
    cleanup();
}

void robot::dump_settings()
{
  std::cout << std::endl;
  std::cout << "  -- key                  : " << settings_.key << std::endl;
  std::cout << "  -- channel name         : " << settings_.channel_name << std::endl;
  std::cout << "  -- uid                  : " << settings_.uid << std::endl;
  std::cout << "  -- mode                 : " << (settings_.mode == MODE_BROADCAST ? "broadcast" : "communicat") << std::endl;
  std::cout << "  -- role                 : " << (settings_.role == ROLE_BROADCASTER ? "broadcaster" : "audience") << std::endl;
  std::cout << "  -- dual stream          : " << (settings_.dual_stream == 1 ? "true" : "false") << std::endl;
  std::cout << "  -- web interoperability : " << (settings_.web_interoperability == 1 ? "true" : "false") << std::endl;
  std::cout << "  -- video profile        : " << settings_.video_profile << std::endl;
  std::cout << "  -- video profile ex     : " << "width:" << settings_.video_profile_ex.width << ", height:" << settings_.video_profile_ex.height << ", fps:" << settings_.video_profile_ex.fps << ", kbps:" << settings_.video_profile_ex.kbps << std::endl;
  std::cout << "  -- audio codec          : " << settings_.audio_codec << std::endl;
  std::cout << "  -- mute                 : " << "local video:"<< settings_.mute.local_video << ", local audio:" << settings_.mute.local_audio << ", remote video:" << settings_.mute.remote_video << ", remote audio:" << settings_.mute.remote_audio << std::endl;
  std::cout << "  -- video source         : " << settings_.video_file << std::endl;
  std::cout << "  -- audio source         : " << settings_.audio_file << std::endl;
  for (auto iter = settings_.parameters.begin(); iter != settings_.parameters.end(); iter++)
  {
    std::cout << "  -- parameters           : " << *iter << std::endl;
  }
  std::cout << std::endl;
}

void robot::cleanup() {
  LOG(INFO, "Leaving channel and ready to cleanup");

  rtc_engine->leaveChannel();
  rtc_engine->release();
  rtc_engine = NULL;

  fs_audio.close();
  fs_video.close();

  sleep(1);
}

int robot::run() {
  int ret = 0;
  fs_audio.open(settings_.audio_file.c_str(), fstream::in|fstream::binary);
  fs_video.open(settings_.video_file.c_str(), fstream::in|fstream::binary);

  term_sig = false;
  rtc_engine = dynamic_cast<rtc::IRtcEngineEx*>(createAgoraRtcEngine());
  if (rtc_engine == NULL) {
    SAFE_LOG(FATAL) << "Failed to create an Agora Rtc Engine!";
    return -1;
  }

  // RtcEngineContextEx context;
  RtcEngineContext context;
  context.eventHandler = this;
  // context.isExHandler = true;
  context.appId = settings_.key.c_str();

  // int ret = rtc_engine->initializeEx(context);
  ret = rtc_engine->initialize(context);
  if(ret != 0)
  {
    return -1;
  }

  rtc_engine->setLogCallback(true);

  // mode & role
  if (settings_.mode == MODE_BROADCAST) {
    rtc_engine->setChannelProfile(rtc::CHANNEL_PROFILE_LIVE_BROADCASTING);
    if(settings_.role == ROLE_BROADCASTER)
    {
      rtc_engine->setClientRole(rtc::CLIENT_ROLE_BROADCASTER, NULL);
    }
    else
    {
      rtc_engine->setClientRole(rtc::CLIENT_ROLE_AUDIENCE, NULL);      
    }
  }
  else
  {
    rtc_engine->setChannelProfile(rtc::CHANNEL_PROFILE_COMMUNICATION);    
  }

  // video enable
  rtc_engine->enableVideo();

  // dual stream
  if (settings_.dual_stream) {
    rtc_engine->setParameters("{\"rtc.dual_stream_mode\":true,\"che.video.enableLowBitRateStream\":1}");
    rtc::RtcEngineParameters param(*rtc_engine);
    param.enableDualStreamMode(true);
  }

  // video profile  
  if(settings_.video_profile >= 0)
  {
      rtc_engine->setVideoProfile((VIDEO_PROFILE_TYPE)settings_.video_profile, false);
  }
  else
  {
      rtc_engine->setVideoProfileEx(settings_.video_profile_ex.width, settings_.video_profile_ex.height, settings_.video_profile_ex.fps, settings_.video_profile_ex.kbps);
  }

  //set parameters
  auto iter = settings_.parameters.begin();
  for(; iter != settings_.parameters.end(); iter++)
  {
    ret = rtc_engine->setParameters(iter->c_str());
    if(ret != 0)
    {
      std::cout << "error:setParameters: " << iter->c_str() << std::endl;
      return -1;
    }
  }

  // web interoperability
  if(settings_.web_interoperability)
  {
    rtc_engine->setParameters("{\"rtc.video.web_h264_interop_enable\":true,\"che.video.web_h264_interop_enable\":true}");
  }
  else
  {
    rtc_engine->setParameters("{\"rtc.video.web_h264_interop_enable\":false,\"che.video.web_h264_interop_enable\":false}");
  } 

  last_active_ts_ = now_ts();
  max_users_ = 0;

  // setup signal handler
  signal(SIGPIPE, term_handler);
  signal(SIGINT, term_handler);
  signal(SIGTERM, term_handler);

  int64_t join_start_ts = now_us();

  registerAudioFrameObserver(this);
  registerVideoFrameObserver(this);

  int fs = 48000;
  agora::rtc::AParameter msp(*rtc_engine);

  // audio
  if(msp)
  {
    msp->setBool("che.audio.external_device", true);
    msp->setInt("che.audio.audioSampleRate", fs);
    msp->setInt("che.video.local.camera_index", 1024);
    if(!settings_.audio_codec.empty())
    {
      msp->setString("che.audio.specify.codec", settings_.audio_codec.c_str());
    }
  }
  else
  {
    std::cout << "msp is nullptr" << std::endl;
  }

  rtc::RtcEngineParameters parameter(*rtc_engine);
  parameter.setRecordingAudioFrameParameters(fs, 1, RAW_AUDIO_FRAME_OP_MODE_WRITE_ONLY, fs/100);
  parameter.setPlaybackAudioFrameParameters(fs, 1, RAW_AUDIO_FRAME_OP_MODE_READ_ONLY, fs/100);

  // mute
  parameter.muteLocalAudioStream(settings_.mute.local_audio);
  parameter.muteLocalVideoStream(settings_.mute.local_video);
  parameter.muteAllRemoteAudioStreams(settings_.mute.remote_audio);
  parameter.muteAllRemoteVideoStreams(settings_.mute.remote_video);

  // if(msp)
  // {
  //   msp->setInt("che.video.local.camera_index", 1024);
  // }

  
  // Join channel
  if (rtc_engine->joinChannel(settings_.key.c_str(), settings_.channel_name.c_str(), NULL, settings_.uid) < 0) {
    SAFE_LOG(ERROR) << "Failed to create the channel " << settings_.channel_name;
    return -2;
  }
  
  return run_interactive(join_start_ts);
}

int robot::run_interactive(int64_t join_start_ts) {

  while (!term_sig) {
    sleep(1);
  }

  cleanup();
  return 0;
}

void robot::term_handler(int sig_no) {
  (void)sig_no;
  term_sig = true;
}

void robot::onError(int rescode, const char *msg) {
}

void robot::onJoinChannelSuccess(const char *channel, uid_t uid,
    int ts) {
  SAFE_LOG(INFO) << uid << " logined successfully in " << channel
      << ", elapsed: " << ts << " ms";

  joined_ = true;
  LOG(INFO, "User %u join channel %s success", uid, channel);
  std::cout << "User " << uid << " join channel " << channel << " success" << std::endl << std::endl;
}

void robot::onRejoinChannelSuccess(const char *channel, uid_t uid,
    int elapsed) {
  SAFE_LOG(INFO) << uid << " rejoin to channel: " << channel << ", time offset "
      << elapsed << " ms";

  std::cout << uid << " rejoin to channel: " << channel << ", time offset "
      << elapsed << " ms" << std::endl;
}

void robot::onWarning(int warn, const char *msg) {
  SAFE_LOG(WARN) << "code: " << warn << ", " << msg;
  // std::cout << "code: " << warn << ", " << msg << std::endl;

}

void robot::onUserJoined(uid_t uid, int elapsed) {
  SAFE_LOG(INFO) << "offset " << elapsed << " ms: " << uid
      << " joined the channel";

  std::cout << "offset " << elapsed << " ms: " << uid
      << " joined the channel" << std::endl;
}

void robot::onUserOffline(uid_t uid,
    USER_OFFLINE_REASON_TYPE reason) {
  const char *detail = reason == USER_OFFLINE_QUIT ? "quit" : "dropped";
  SAFE_LOG(INFO) << "User " << uid << " " << detail;
  std::cout << "User " << uid << " " << detail << std::endl;
}

void robot::onRtcStats(const rtc::RtcStats &stats) {
  if (stats.users > 1) {
    last_active_ts_ = now_ts();
  }
  max_users_ = (std::max)(max_users_, stats.users);
  if (now_ts() - last_active_ts_ > channel_idle_) {
    LOG(WARN, "Channel Time Out.");
    std::cout << "Channel Time Out." << std::endl;
    //term_sig = true;
  }
}

void robot::onLogEvent(int level, const char *msg, int length) {
  (void)length;
  LOG(INFO, "level %d: %s", level, msg);
  std::cout << "level " << level << ": " << msg << std::endl;
}

bool robot::onRecordAudioFrame(AudioFrame& audioFrame) {

  size_t bytes = audioFrame.samples * audioFrame.bytesPerSample * audioFrame.channels;

  if (fs_audio.is_open()) {
    fs_audio.read((char *)audioFrame.buffer, bytes);
    
    if (fs_audio.eof()) {
      fs_audio.clear();
      fs_audio.seekg(0);
    }
  }
  return true;
}

bool robot::onMixedAudioFrame(AudioFrame& audioFrame) {
  return true;
}

bool robot::onPlaybackAudioFrame(AudioFrame& audioFrame) {
  return true;
}

bool robot::onPlaybackAudioFrameBeforeMixing(unsigned int uid, AudioFrame& audioFrame) {
  return true;
}

bool robot::onCaptureVideoFrame(VideoFrame& videoFrame) {

  // int w = videoFrame.width;
  int h = videoFrame.height;
  int yStride = videoFrame.yStride;
  int uStride = videoFrame.uStride;
  int vStride = videoFrame.vStride;
  char *yBuf = (char *)videoFrame.yBuffer;
  char *uBuf = (char *)videoFrame.uBuffer;
  char *vBuf = (char *)videoFrame.vBuffer;

  // Y channel
  //int POT = 3;
  //for(int i = 0; i < yStride; ++i) {
  //  for(int j = 0; j < h; ++j) {
  //    yBuf[j * yStride + i] = (((i ^ j) >> POT) & 1) - 1; // checkerbord
  //  }
  //}

  // U channel
  //float duration = float(now_ms())/1000;
  //float t = 0.5 + 0.5 * sin(duration * 6.2831);
  //for(int i = 0; i < uStride*(h/2); ++i) {
  //  uBuf[i] = (char)(t * 0xFF);
  //}

  // V channel
  //for(int i = 0; i < vStride*(h/2); ++i) {
  //  vBuf[i] = (char)( (1.0 - t) * 0xFF );
  //}

  if (fs_video.is_open()) {
    fs_video.read(yBuf, yStride*h);
    fs_video.read(uBuf, uStride*h/2);
    fs_video.read(vBuf, vStride*h/2);
    
    if (fs_video.eof()) {
      fs_video.clear();
      fs_video.seekg(0);
    }
  }
  return true;
}

bool robot::onRenderVideoFrame(unsigned int uid, VideoFrame& videoFrame) {
  return true;
}

}
}

