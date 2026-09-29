#ifndef WIFI_H
#define WIFI_H

#include <Arduino.h>

struct songData
{
    String artist = "";
    String title = "";
    bool is_playing = false;
    uint32_t progress = 0;
    uint32_t timestamp = 0;
    String ArtURL = "";
    uint32_t duration =0;
};

extern songData data;

void wifiInit();
bool wifiConnected();
bool spotifyIni();
bool UpdateData();

#endif