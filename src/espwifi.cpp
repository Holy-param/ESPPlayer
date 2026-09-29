#include <Arduino.h>
#include <esp_wifi.h>
#include <string.h>
#include <HTTPClient.h>
#include "wifi.h"
#include <ArduinoJson.h>
#include <espwifi.h>

const char* ssid = "";
const char* password = "";
String refreshToken="";
String accessToken="";
const char* clientID="";
const char* clientSecret="";
songData data;

#include <WiFi.h>

void wifiInit()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    Serial.println("Connecting to WiFi...");

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.printf("WiFi connected!");
    Serial.printf("IP address:%s | Gateway: %s | DNS: %s |",WiFi.localIP().toString().c_str()
    ,WiFi.gatewayIP().toString().c_str(),WiFi.dnsIP().toString().c_str());
}
bool wifiConnected()
{
return WiFi.status() == WL_CONNECTED;
}



bool spotifyIni()
{
    String resp;
    JsonDocument doc;
    HTTPClient http;
    int code;
    http.begin("https://accounts.spotify.com/api/token");
    http.setAuthorization(clientID,clientSecret);
    http.addHeader("Content-Type","application/x-www-form-urlencoded");
    String body="grant_type=refresh_token&refresh_token="+ String(refreshToken); //use String not char* since HTTP lib expects a arduino style 'String'.
    code=http.POST(body);

    if(code!=200)
        {
        Serial.printf("Error in spotify initialization. Code=%d \n",code);
        http.end();
        return false;        
        }   
    resp= http.getString();
    Serial.printf("Spotify authentication http code %d\n",code);
    
    
    DeserializationError error = deserializeJson(doc, resp);

    if (error)
    {
        Serial.printf("JSON parsing failed: %s\n", error.c_str());
        http.end();
        return false;
    }

    accessToken = doc["access_token"].as<String>();
    if (doc["refresh_token"].is<String>())
        refreshToken = doc["refresh_token"].as<String>();
    http.end();
    return true;
} 
 bool UpdateData()
{   
HTTPClient http;
    int code;
    String resp="";
    JsonDocument doc;
    http.begin("https://api.spotify.com/v1/me/player/currently-playing");
    http.addHeader("Authorization","Bearer "+accessToken);
    code=http.GET();
    if(code ==204)
    {Serial.printf("No song is plaing. ");
    data.is_playing = false;
    return false;}
    else if (code != 200)
    {
        Serial.printf("Spotify API error: %d\n", code);
        http.end();
        return false;
    }
    resp= http.getString();
    DeserializationError error = deserializeJson(doc, resp);
    if (error)
    {
        Serial.printf("JSON parsing failed: %s\n", error.c_str());
        http.end();
        return false;
    }

    if (doc["is_playing"].as<bool>())
    {
        data.title = doc["item"]["name"].as<String>();
        data.is_playing = doc["is_playing"].as<bool>();
        data.timestamp = doc["timestamp"].as<uint32_t>();
        data.progress = doc["progress_ms"].as<uint32_t>();
        data.artist = doc["item"]["artists"][0]["name"].as<String>();
        data.ArtURL = doc["item"]["album"]["images"][0]["url"].as<String>();
        data.duration = doc["item"]["duration_ms"].as<uint32_t>();

        Serial.printf("Song: %s | Artist: %s | Progress: %lu/%lu  ms\n",data.title.c_str(),data.artist.c_str(),data.progress,data.duration);
    }
    else
    {
        data.is_playing = false;
        Serial.println("Play something my brother");
        http.end();
        return false;
    }
http.end();
return true;
}