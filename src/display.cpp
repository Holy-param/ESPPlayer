#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "display.h"
#include "image.h"

#define WIDTH 128
#define HEIGHT 64
#define ADDRESS 0x3C
int titleX = WIDTH;
int artistX = WIDTH;
Adafruit_SSD1306 display(WIDTH, HEIGHT, &Wire, -1);

bool displayInit()
{
    Wire.begin(21, 22);

    if (!display.begin(SSD1306_SWITCHCAPVCC, ADDRESS))
    {
        Serial.println("OLED initialization failed!");
        return 0;
    }

    display.setRotation(0);
    display.clearDisplay();

    display.drawBitmap(
        0, 0,
        bitmap[0],
        128, 64,
        SSD1306_WHITE
    );

    display.display();
    return 1;
}


void display_song(String title, String artist)
{
    static unsigned long lastMove = 0;

    static String lastTitle = "";
    static String lastArtist = "";

    int16_t x1, y1;
    uint16_t titleWidth, titleHeight;
    uint16_t artistWidth, artistHeight;

    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setTextWrap(false);
    if (title != lastTitle)
    {
        titleX = WIDTH;
        lastTitle = title;
    }

    if (artist != lastArtist)
    {
        artistX = WIDTH;
        lastArtist = artist;
    }

    display.getTextBounds(
        title, 0, 0,
        &x1, &y1,
        &titleWidth, &titleHeight
    );
    display.getTextBounds(
        artist, 0, 0,
        &x1, &y1,
        &artistWidth, &artistHeight
    );

    bool titleNeedsScroll = titleWidth > WIDTH;
    bool artistNeedsScroll = artistWidth > WIDTH;

    if (millis() - lastMove >= 25)
    {
        lastMove = millis();
        if (titleNeedsScroll && artistNeedsScroll)
        {
            titleX--;
            artistX--;

            int longestWidth = max(titleWidth, artistWidth);

            if (titleX < -longestWidth - 30)
            {
                titleX = WIDTH;
                artistX = WIDTH;
            }
        }
      else if (titleNeedsScroll)
        {
            titleX--;

            if (titleX < -((int)titleWidth) - 30)
            {
                titleX = WIDTH;
            }

            artistX = 0;
        }

        else if (artistNeedsScroll)
        {
            artistX--;

            if (artistX < -((int)artistWidth) - 30)
            {
                artistX = WIDTH;
            }

            titleX = 0;
        }

         else
        {
            titleX = 0;
            artistX = 0;
        }
    }

    display.clearDisplay();

    display.setCursor(titleX, 0);
    display.print(title);

  display.setCursor(artistX, 16);
    display.print(artist);

    display.display();
}