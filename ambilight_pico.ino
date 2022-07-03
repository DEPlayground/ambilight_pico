/*
    Lara Maia <dev@lara.monster> 2022

    The RGBFanControl is free software: you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation, either version 3 of
    the License, or (at your option) any later version.

    The RGBFanControl is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see http://www.gnu.org/licenses/.
*/

#include <FastLED.h>
#include <CorsairLightingProtocol.h>

#define BRIGHTNESS 255
#define LED_DATA_PIN 16
#define NUM_LEDS 106

uint8_t currentBrightness = BRIGHTNESS;
uint8_t blankLeds = 0;
FastLEDController controller(nullptr);
CRGB leds[NUM_LEDS];

DeviceID deviceID = {0xA6, 0xA6, 0xA6, 0xA6};
CorsairLightingFirmwareStorageStatic storage(deviceID);
CorsairLightingFirmware firmware(CORSAIR_LIGHTING_NODE_PRO, &storage);
CorsairLightingProtocolController lightningProtocol(&controller, &firmware);
CorsairLightingProtocolTinyUSBHID HID(&lightningProtocol);

void corsairUpdateCallback() {
    blankLeds = 0;

    for ( int i = 0; i < NUM_LEDS; i++ )
    {
        if(leds[i].getLuma() > 100)
        {
          blankLeds += 1;
        }

        if (i <= 53) {
          leds[i].r = leds[i].r / 4;
          leds[i].g = leds[i].g / 4;
          leds[i].b = leds[i].b / 4;
        }
        else
        {
          leds[i].r = leds[i].r / 2;
          leds[i].g = leds[i].g / 2;
          leds[i].b = leds[i].b / 2;
        }
    }
}

void setup() {
    HID.setup();

    FastLED.addLeds<WS2812B, LED_DATA_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(currentBrightness);
    FastLED.setDither(0);

    controller.addLEDs(0, leds, NUM_LEDS);
    controller.onUpdateHook(0, []() {corsairUpdateCallback();});
}

void loop() {
    HID.update();

    controller.updateLEDs();

    if(blankLeds >= 25)
    {
        if ( currentBrightness > 20 )
        {
            currentBrightness--;
        }
    }
    else if ( currentBrightness < BRIGHTNESS )
    {
        currentBrightness++;
    }

    FastLED.setBrightness(currentBrightness);
    FastLED.show();
}
