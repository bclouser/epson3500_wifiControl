
Get the Code
------------
esp_mqtt (https://github.com/tuanpmt/esp_mqtt) is now subtree'd in

Build
-----
In order to build you will need to create user/secrets.h.
It should look like this. 
```c
#ifndef SECRETS
#define SECRETS

#define WIFI_SSID "ssid"
#define WIFI_PASSWD "secret"

#endif
```

There is a "buildForMac.sh" script which should work on linux as well but won't require a case sensitve mounted image (thanks apple). Modify the variables in that script appropriately to your environment and it should build.
