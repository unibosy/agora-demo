# Getting Started
## Build
Only for linux platform
```
make
```
## Configuration
### Points in file config.json
* **uid** : When "uid" is 0 robot will get uid from sdk.
* **mode** : When "mode" is "communicat" "role" is meanless.
* **video_profile** : When "video_profile" is -1 robot will use "video_profile_ex" instead otherwise use "video_profile".
    * For "video_profile" definition please refer to **video profile list**
## run
```
python ./robots_run.py
```
## video profile list
| index     |  res       | fps   |  kbps  |
| --------- | :-------:  | :-----: | ------: |
| 0         | 160x120    |15fps  | 80kbps |
| 1		    | 120x160    |15fps  | 80kbps |
| 2		    | 120x120    |15fps  | 60kbps |
| 10		| 320x180    |15fps  | 160kbps |
| 11		| 180x320    |15fps  | 160kbps |
| 12		| 180x180    |15fps  | 120kbps |
| 20		| 320x240    |15fps  | 200kbps |
| 21		| 240x320    |15fps  | 200kbps |
| 22		| 240x240    |15fps  | 160kbps |
| 30		| 640x360    |15fps  | 400kbps |
| 31		| 360x640    |15fps  | 400kbps |
| 32		| 360x360    |15fps  | 300kbps |
| 33		| 640x360    |30fps  | 800kbps |
| 34		| 360x640    |30fps  | 800kbps |
| 35		| 360x360    |30fps  | 600kbps |
| 40		| 640x480    |15fps  | 500kbps |
| 41		| 480x640    |15fps  | 500kbps |
| 42		| 480x480    |15fps  | 400kbps |
| 43		| 640x480    |30fps  | 1000kbps |
| 44		| 480x640    |30fps  | 1000kbps |
| 45		| 480x480    |30fps  | 800kbps |
| 50		| 1280x720   |15fps  | 1000kbps |
| 51		| 720x1280   |15fps  | 1000kbps |
| 52		| 1280x720   |30fps  | 2000kbps |
| 53		| 720x1280   |30fps  | 2000kbps |
| 60		| 1920x1080  |15fps  | 1500kbps |
| 61		| 1080x1920  |15fps  | 1500kbps |
| 62		| 1920x1080  |30fps  | 3000kbps |
| 63		| 1080x1920  |30fps  | 3000kbps |
| 64		| 1920x1080  |60fps  | 6000kbps |
| 65		| 1080x1920  |60fps  | 6000kbps |
| 70		| 3840x2160  |30fps  | 8000kbps |
| 71		| 2160x3080  |30fps  | 8000kbps |
| 72		| 3840x2160  |60fps  | 16000kbps |
| 73		| 2160x3840  |60fps  | 16000kbps |
