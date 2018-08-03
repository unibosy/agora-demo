/*************************************************************
 * Author:	Lionfore Hao (haolianfu@agora.io)
 * Date	 :	Jul 20th, 2018
 * Module:	Agora SD-RTN SDK relative APIs header file
 *
 *
 * This is a part of the Advanced High Performance Library.
 * Copyright (C) 2018 Agora IO
 * All rights reserved.
 *
 *************************************************************/

#ifndef __AGORA_RTC_API_H__
#define __AGORA_RTC_API_H__

#include <sys/types.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
	CHANNEL_PROFILE_COMMUNICATION = 0,
	CHANNEL_PROFILE_LIVE_BROADCASTING = 1,
	CHANNEL_PROFILE_GAME = 2,
} agora_channel_profile_t;

typedef enum {
	CLIENT_ROLE_COMMUNICATION = 0,
	CLIENT_ROLE_BROADCASTER = 1,
	CLIENT_ROLE_AUDIENCE = 2,
} agora_client_role_t;

typedef enum { // sample rate, bit rate, mono/stereo, speech/music codec
    AUDIO_PROFILE_DEFAULT = 0, // use default settings
    AUDIO_PROFILE_SPEECH_STANDARD = 1, // 32Khz, 18kbps, mono, speech
    AUDIO_PROFILE_MUSIC_STANDARD = 2, // 48Khz, 48kbps, mono, music
    AUDIO_PROFILE_MUSIC_STANDARD_STEREO = 3, // 48Khz, 56kbps, stereo, music
    AUDIO_PROFILE_MUSIC_HIGH_QUALITY = 4, // 48Khz, 128kbps, mono, music
    AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO = 5, // 48Khz, 192kbps, stereo, music
    AUDIO_PROFILE_IOT                       = 6,   // G722
    AUDIO_PROFILE_NUM = 7,
} agora_audio_profile_t;

typedef enum { // res       fps
    VIDEO_PROFILE_LANDSCAPE_120P = 0,         // 160x120   15
    VIDEO_PROFILE_LANDSCAPE_120P_3 = 2,       // 120x120   15
    VIDEO_PROFILE_LANDSCAPE_180P = 10,        // 320x180   15
    VIDEO_PROFILE_LANDSCAPE_180P_3 = 12,      // 180x180   15
    VIDEO_PROFILE_LANDSCAPE_180P_4 = 13,      // 240x180   15
    VIDEO_PROFILE_LANDSCAPE_240P = 20,        // 320x240   15
    VIDEO_PROFILE_LANDSCAPE_240P_3 = 22,      // 240x240   15
    VIDEO_PROFILE_LANDSCAPE_240P_4 = 23,      // 424x240   15
    VIDEO_PROFILE_LANDSCAPE_360P = 30,        // 640x360   15
    VIDEO_PROFILE_LANDSCAPE_360P_3 = 32,      // 360x360   15
    VIDEO_PROFILE_LANDSCAPE_360P_4 = 33,      // 640x360   30
    VIDEO_PROFILE_LANDSCAPE_360P_6 = 35,      // 360x360   30
    VIDEO_PROFILE_LANDSCAPE_360P_7 = 36,      // 480x360   15
    VIDEO_PROFILE_LANDSCAPE_360P_8 = 37,      // 480x360   30
    VIDEO_PROFILE_LANDSCAPE_360P_9 = 38,      // 640x360   15
    VIDEO_PROFILE_LANDSCAPE_360P_10 = 39,     // 640x360   24
    VIDEO_PROFILE_LANDSCAPE_360P_11 = 100,    // 640x360   24
    VIDEO_PROFILE_LANDSCAPE_480P = 40,        // 640x480   15
    VIDEO_PROFILE_LANDSCAPE_480P_3 = 42,      // 480x480   15
    VIDEO_PROFILE_LANDSCAPE_480P_4 = 43,      // 640x480   30
    VIDEO_PROFILE_LANDSCAPE_480P_6 = 45,      // 480x480   30
    VIDEO_PROFILE_LANDSCAPE_480P_8 = 47,      // 848x480   15
    VIDEO_PROFILE_LANDSCAPE_480P_9 = 48,      // 848x480   30
    VIDEO_PROFILE_LANDSCAPE_480P_10 = 49,     // 640x480   10
    VIDEO_PROFILE_LANDSCAPE_720P = 50,        // 1280x720  15
    VIDEO_PROFILE_LANDSCAPE_720P_3 = 52,      // 1280x720  30
    VIDEO_PROFILE_LANDSCAPE_720P_5 = 54,      // 960x720   15
    VIDEO_PROFILE_LANDSCAPE_720P_6 = 55,      // 960x720   30
    VIDEO_PROFILE_LANDSCAPE_1080P = 60,       // 1920x1080 15
    VIDEO_PROFILE_LANDSCAPE_1080P_3 = 62,     // 1920x1080 30
    VIDEO_PROFILE_LANDSCAPE_1080P_5 = 64,     // 1920x1080 60
    VIDEO_PROFILE_LANDSCAPE_1440P = 66,       // 2560x1440 30
    VIDEO_PROFILE_LANDSCAPE_1440P_2 = 67,     // 2560x1440 60
    VIDEO_PROFILE_LANDSCAPE_4K = 70,          // 3840x2160 30
    VIDEO_PROFILE_LANDSCAPE_4K_3 = 72,        // 3840x2160 60

    VIDEO_PROFILE_PORTRAIT_120P = 1000,       // 120x160   15
    VIDEO_PROFILE_PORTRAIT_120P_3 = 1002,     // 120x120   15
    VIDEO_PROFILE_PORTRAIT_180P = 1010,       // 180x320   15
    VIDEO_PROFILE_PORTRAIT_180P_3 = 1012,     // 180x180   15
    VIDEO_PROFILE_PORTRAIT_180P_4 = 1013,     // 180x240   15
    VIDEO_PROFILE_PORTRAIT_240P = 1020,       // 240x320   15
    VIDEO_PROFILE_PORTRAIT_240P_3 = 1022,     // 240x240   15
    VIDEO_PROFILE_PORTRAIT_240P_4 = 1023,     // 240x424   15
    VIDEO_PROFILE_PORTRAIT_360P = 1030,       // 360x640   15
    VIDEO_PROFILE_PORTRAIT_360P_3 = 1032,     // 360x360   15
    VIDEO_PROFILE_PORTRAIT_360P_4 = 1033,     // 360x640   30
    VIDEO_PROFILE_PORTRAIT_360P_6 = 1035,     // 360x360   30
    VIDEO_PROFILE_PORTRAIT_360P_7 = 1036,     // 360x480   15
    VIDEO_PROFILE_PORTRAIT_360P_8 = 1037,     // 360x480   30
    VIDEO_PROFILE_PORTRAIT_360P_9 = 1038,     // 360x640   15
    VIDEO_PROFILE_PORTRAIT_360P_10 = 1039,    // 360x640   24
    VIDEO_PROFILE_PORTRAIT_360P_11 = 1100,    // 360x640   24
    VIDEO_PROFILE_PORTRAIT_480P = 1040,       // 480x640   15
    VIDEO_PROFILE_PORTRAIT_480P_3 = 1042,     // 480x480   15
    VIDEO_PROFILE_PORTRAIT_480P_4 = 1043,     // 480x640   30
    VIDEO_PROFILE_PORTRAIT_480P_6 = 1045,     // 480x480   30
    VIDEO_PROFILE_PORTRAIT_480P_8 = 1047,     // 480x848   15
    VIDEO_PROFILE_PORTRAIT_480P_9 = 1048,     // 480x848   30
    VIDEO_PROFILE_PORTRAIT_480P_10 = 1049,    // 480x640   10
    VIDEO_PROFILE_PORTRAIT_720P = 1050,       // 720x1280  15
    VIDEO_PROFILE_PORTRAIT_720P_3 = 1052,     // 720x1280  30
    VIDEO_PROFILE_PORTRAIT_720P_5 = 1054,     // 720x960   15
    VIDEO_PROFILE_PORTRAIT_720P_6 = 1055,     // 720x960   30
    VIDEO_PROFILE_PORTRAIT_1080P = 1060,      // 1080x1920 15
    VIDEO_PROFILE_PORTRAIT_1080P_3 = 1062,    // 1080x1920 30
    VIDEO_PROFILE_PORTRAIT_1080P_5 = 1064,    // 1080x1920 60
    VIDEO_PROFILE_PORTRAIT_1440P = 1066,      // 1440x2560 30
    VIDEO_PROFILE_PORTRAIT_1440P_2 = 1067,    // 1440x2560 60
    VIDEO_PROFILE_PORTRAIT_4K = 1070,         // 2160x3840 30
    VIDEO_PROFILE_PORTRAIT_4K_3 = 1072,       // 2160x3840 60
    VIDEO_PROFILE_DEFAULT = VIDEO_PROFILE_LANDSCAPE_360P,
} agora_video_profile_t;


typedef struct {
	unsigned int duration;
	unsigned int txBytes;
	unsigned int rxBytes;
	unsigned short txKBitRate;
	unsigned short rxKBitRate;
	unsigned int lastmileQuality;
	double cpuAppUsage;
	double cpuTotalUsage;
} agora_rtcstats_t;

enum USER_OFFLINE_REASON_TYPE
{
	USER_OFFLINE_QUIT = 0,
	USER_OFFLINE_DROPPED = 1,
};

/**
 * The Agora SDK event handler structure.
 **/
typedef struct {
	void (*on_join_channel_success) (const char *channel, uint32_t uid, int elapsed);
	void (*on_rejoin_channel_success) (const char *channel, uint32_t uid, int elapsed);
	void (*on_warning) (const char *channel, int warn, const char *msg);
	void (*on_error) (const char *channel, int err, const char *msg);
	void (*on_user_joined) (const char *channel, uint32_t uid, int elapsed);
	void (*on_user_offline) (const char *channel, uint32_t uid, int reason);
	void (*on_rtcstats) (const char *channel, const agora_rtcstats_t *stats);
	void (*on_connection_lost) (const char *channel);
	void (*on_user_mute_audio) (const char *channel, uint32_t uid, int muted);
	void (*on_user_mute_video) (const char *channel, uint32_t uid, int muted);
	int (*on_customized_sei) (const char *channel, const void **content, int *length);
	void (*on_audio_data) (const char *channel, const uint32_t uid, uint8_t codec, const void *data, size_t len);
	void (*on_video_data) (const char *channel, const uint32_t uid, uint8_t codec, uint8_t stream_id, int is_key_frame, const void *data, size_t len);
} agora_rtc_event_handler_t;


/**
 * Create the global Agora RTC Engine instance, only ONE instance allowed for a single process.
 * Parameter:
 *     app_id: The Application ID issued by Agora for the using customer;
 *        uid: The user ID a 32bits integer number, which would be used to identify this user;
 *     event_handler: A set of C callback functions for processing the Agora SDK events;
 * Return value:
 *     0 for success, -1 when failed with errno set.
 **/
extern int agora_rtc_create (const char *app_id, uint32_t uid, const agora_rtc_event_handler_t *event_handler);

/**
 * Enable audio stream. The audio stream is enabled by default.
 * Parameter:
 *      channel_name: the channel you want to affect, NULL for all
 *                    current joined channels.
 *      enable: 0 for disable and all other values for enable.
 * Return value:
 *     0 for success, -1 when failed with errno set.
 **/
extern int agora_rtc_enable_audio (const char *channel_name, int enable);

/**
 * Enable video stream. The video stream is disabled by default, so
 * you need to enable when needed.
 * Parameter:
 *      channel_name: the channel you want to affect, NULL for all
 *                    current joined channels.
 *      enable: 0 for disable and all other values for enable.
 * Return value:
 *     0 for success, -1 when failed with errno set.
 **/
extern int agora_rtc_enable_video (const char *channel_name, int enable);

/**
 * Mute/Unmute audio stream. The audio stream is not muted by default.
 * Parameter:
 *      channel_name: the channel you want to affect, NULL for all
 *                    current joined channels.
 *      mute: 0 for unmute and all other values for mute.
 * Return value:
 *     0 for success, -1 when failed with errno set.
 **/
extern int agora_rtc_mute_audio (const char *channel_name, int mute);

/**
 * Mute/Unmute video stream. The video stream is not muted by default.
 * Parameter:
 *      channel_name: the channel you want to affect, NULL for all
 *                    current joined channels.
 *      mute: 0 for unmute and all other values for mute.
 * Return value:
 *     0 for success, -1 when failed with errno set.
 **/
extern int agora_rtc_mute_video (const char *channel_name, int mute);

/**
 * Join a channel for chatting. You must join a channel before you can
 * communicate with anyone.
 * Parameter:
 *     channel_name: Channel name string;
 * Return value:
 *     0 for success, -1 when failed with errno set.
 **/
extern int agora_rtc_join_channel (const char *channel_name);

#define AGORA_RTC_USER_AUDIO_CODEC ((uint8_t)0xff)
#define AGORA_RTC_USER_VIDEO_CODEC ((uint8_t)0xff)

/**
 * Send transparent audio data to the joined channel, then other users
 * in this channel could receive the audio stream.
 * Parameter:
 *     channel: the channel name the audio data sending to;
 *     codec: the audio codec used, AGORA_RTC_USER_AUDIO_CODEC for cases customer doing the audio codec;
 *     data: The audio data memory;
 *     len: The audio data size in bytes;
 * Return value:
 *     0 for success, -1 when failed with errno set.
 **/
extern int agora_rtc_send_audio_data (const char *channel_name, uint8_t codec, const void *data, size_t len);

/**
 * Send transparent video data to the joined channel, then other users
 * in this channel could receive the video stream.
 * Parameter:
 *     channel: the channel name the audio data sending to;
 *     codec: the video codec used, AGORA_RTC_USER_VIDEO_CODEC for cases customer doing the video codec;
 *     stream_id: a [0, 15] value specify the video stream id;
 *     is_key_frame: Indicates whether the video frame is a key frame,
 *                         1: key frame
 *                         0: non-key frame
 *     data: The video data memory;
 *     len: The video data size in bytes;
 * Return value:
 *     0 for success, -1 when failed with errno set.
 **/
extern int agora_rtc_send_video_data (const char *channel_name, uint8_t codec, uint8_t stream_id, int is_key_frame, const void *data, size_t len);

/**
 * Leave a channel. You must leave channel when you do not want to
 * communicate with others.
 * Parameter:
 *     channel: the channel name the audio data sending to;
 * Return value:
 *     0 for success, -1 when failed with errno set.
 **/
extern int agora_rtc_leave_channel (const char *channel_name);

/**
 * Destroy the single global Agora RTC Engine instance which was
 * created previously by agora_create_rtc_engine.
 * Parameter:
 *      None.
 * Return value:
 *     0 for success, -1 when failed with errno set.
 **/
extern int agora_rtc_destroy ();



#ifdef __cplusplus
}
#endif


#endif /* __AGORA_RTC_API_H__ */