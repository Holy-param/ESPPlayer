#include <Arduino.h>
#include "wifi.h"
#include <espwifi.h>
#include <display.h>
unsigned long pmillis = 0;

bool initialize()
{
    wifiInit();

    if (!wifiConnected())
    {
        Serial.println("WiFi not connected, please check");
        return false;
    }

    if (!spotifyIni())
    {
        Serial.println("Can't initialize Spotify, please check");
        return false;
    }

    if (!displayInit())
    {
        Serial.println("Can't initialize Display, please check");
        return false;
    }

    return true;
}


void setup()
{
    Serial.begin(115200);
    delay(1000);

    if (!initialize())
    {
        Serial.println("Initialization failed!");
        while (1);
    }

    if (UpdateData())
    {
        pmillis = millis();

        display_song(data.title, data.artist);

        Serial.printf(
            "Current song: %s - %s\n",
            data.title.c_str(),
            data.artist.c_str()
        );

        Serial.printf(
            "Progress: %lu / %lu ms\n",
            data.progress,
            data.duration
        );
    }
}

void loop()
{
    /*
     * Keep the display animation running
     */
    display_song(data.title, data.artist);


    /*
     * Calculate how long is left in the song
     */
    if (data.is_playing && data.duration > data.progress)
    {
        uint32_t remainingTime = data.duration - data.progress;
        if (millis() - pmillis >= remainingTime)
        {
            Serial.println("Song timer expired!");
            Serial.println("Checking Spotify for new song...");

            bool updated = false;

            for (int i = 0; i < 3; i++)
            {
                if (UpdateData())
                {
                    updated = true;
                    break;
                }

                Serial.println("No new song yet...");
                delay(500);
            }

            if (updated)
            {
                pmillis = millis();

                Serial.printf(
                    "New song: %s - %s\n",
                    data.title.c_str(),
                    data.artist.c_str()
                );

                display_song(data.title, data.artist);
            }
            else
            {
                pmillis = millis();
                data.is_playing = false;

                Serial.println("Nothing currently playing.");
            }
        }
    }
}/*
void printMenu()
{
    Serial.println();
    Serial.println("===== ESP32 TEST MENU =====");
    Serial.println("1. Initialize WiFi");
    Serial.println("2. Check WiFi connection");
    Serial.println("3. Initialize Spotify");
    Serial.println("4. Update Spotify data");
    Serial.println("5. Show current SongData");
    Serial.println("0. Show menu");
    Serial.println("===========================");
    Serial.print("Enter choice: ");
}



void loop()
{
    if (Serial.available())
    {
        char choice = Serial.read();
        if (choice == '\r' || choice == '\n')
        {
            return;
        }
        switch (choice)
        {
            case '1':
                Serial.println("\nInitializing WiFi...");
                wifiInit();
                break;
            case '2':
    if (wifiConnected())
    {
        Serial.println("WiFi connected!");
        Serial.printf("IP address:%s \n Gateway: %s \n DNS: %s"
        ,WiFi.localIP().toString().c_str() ,WiFi.gatewayIP().toString().c_str(),WiFi.dnsIP().toString().c_str());
    } 
    else
    {
        Serial.println("WiFi not connected.");
    }
    break;
            case '3':
                Serial.println("\nInitializing Spotify...");
                if (spotifyIni())
                    Serial.println("Spotify initialized successfully.");
                else
                    Serial.println("Spotify initialization failed.");
                break;

            case '4':
                Serial.println("\nUpdating Spotify data...");
                if (UpdateData())
                    Serial.println("Spotify data updated successfully.");
                else
                    Serial.println("Failed to update Spotify data.");
                break;
            case '5':
                Serial.println("\n===== CURRENT SONG DATA =====");
                Serial.printf("Title     : %s\n Artist    : %s\n", data.title.c_str(),data.artist.c_str());
                Serial.printf("Playing   : %s\n",data.is_playing ? "Yes" : "No");
                Serial.printf("Progress  : %lu ms\n Timestamp : %lu\n Art URL   : %s\n", data.progress, data.timestamp, data.ArtURL.c_str());
                Serial.println("=============================");
                break;
            case '6':
                Serial.printf("Displaying current playing song details on screen");
                display_song(data.title,data.artist);
                break;
            case '0':
                printMenu();
                break;

            default:
                Serial.println("\nInvalid choice.");
                Serial.println("Press 0 to show the menu.");
                break;
        }

        Serial.println();
        Serial.print("Enter choice: ");
    }
}
    */