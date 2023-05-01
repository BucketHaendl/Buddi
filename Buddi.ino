/**
 * Buddi Sketch, V 1.0
 * Compatible with Arduino Nano 33 BLE
 * 
 * (c) Cedric-Pascal Sommer, 2023
*/

/**
 * MANDATORY TODO SECTION
 * TODO: Use scheduler and yield() to run network, buttons (user interface) and screen update in parallel
 */

/**
 * DESIGN TODO SECTION
 * TODO: Change avatar size to 42x42 (instead of full screen)
 * TODO: Enable character rendering again
 * TODO: Enable character rendering of other characters again
 * TODO: Enable background rendering again
 * TODO: Enable foreground rendering again
 * TOOD: Remove rotary encoder, unless I want to use it instead of buttons, and change button pin definitions
*/

/**
 * OPTIONAL TODO SECTION
 * TODO: No network connection error screen & handling...just you yourself appears & you can reset?...or just a network connection screen at beginning
 * TODO: Error / abort display when setup fails and ask to reset
 * TODO: Maybe implement that we can controll the other characters
 * TODO: Also implement high five and ducking with joystick (just for fun)
 * TODO: Arrange more object oriented, create separate classes for objects (Room, should have background, foreground...Character, should have avatar, actionPlaying, etc...)
 * TODO: Add floating messages tags, like "Dishi sends you flowers, etc" or like a broadcast (maybe just for accessibility)
*/

/**
 * INCLUDE LIBRARY & DEFINITION SECTION
*/
// Include relevant libraries for the SSD1306-driven OLED display
#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <Wire.h>
#include <WiFi.h>
#include <WebSocketsClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Define OLED display configuration
#define SCREEN_WIDTH 128 // display width, in pixels
#define SCREEN_HEIGHT 64 // display height, in pixels
#define OLED_RESET -1 // reset pin or -1, for Arduino pin
#define SCREEN_ADDRESS 0x3C // display I2C address
#define CHARS_PER_BITMAP 1024 // number of chars in PROGMEM per bitmap image
#define COLOR_MODE WHITE // color theme of the on-screen objects
#define COLOR_HIGHLIGHT BLACK // color highlight color for important information
#define BITMAP_COLOR WHITE // background color of the bitmaps

#define LEFT_BUTTON 0 // define left button
#define MIDDLE_BUTTON 0 // define middle button
#define RIGHT_BUTTON 0 // define right button

#define VRX 25 // Pin for rotary encoder, CLK pin
#define VRY 33 // Pin for rotary encoder, DT pin
#define SW 32 // Pin for rotary encoder, SW pin

/**
 * BITMAP (IMAGE) DECLARATION SECTION
*/
// 'tama0', 128x64px
unsigned char epd_bitmap_tama0[CHARS_PER_BITMAP] PROGMEM = {
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0x81, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xdf, 0x7e, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x9e, 0xcf, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x9e, 0x7f, 0x8f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x9d, 0xff, 0xaf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfa, 0x9e, 0xff, 0x8f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfa, 0x9d, 0xff, 0xaf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfa, 0xdc, 0x7f, 0x8f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf6, 0x1b, 0xff, 0xaf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xa0, 0x7f, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf7, 0x9e, 0x9f, 0x2f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x7e, 0x8f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x2f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xfc, 0xaa, 0x8f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xfc, 0x80, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xfc, 0xf9, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xfc, 0xc0, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};
// 'tama1', 128x64px
unsigned char epd_bitmap_tama1[CHARS_PER_BITMAP] PROGMEM = {
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1f, 0x81, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xdf, 0x7e, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x9e, 0xdf, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x9e, 0x5f, 0x8f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x9d, 0xff, 0xaf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfa, 0x9e, 0xff, 0x8f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfa, 0x9d, 0xff, 0xaf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfa, 0xde, 0x7f, 0x8f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf6, 0x1f, 0x6b, 0xaf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf7, 0xa1, 0x6b, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf7, 0x9e, 0xb7, 0x2f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x7e, 0x8f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x2f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xfc, 0xaa, 0x8f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xfc, 0x80, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xfc, 0xf9, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf3, 0xfc, 0xc0, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

/**
 * CONSTANT DECLARATION SECTION
*/
// Create networking constants
const char* ssid = "Harvard University";
const char* password = "";

const char* host = "socketsbay.com";
const char* url = "/wss/v2/1/demo/";
const int port = 443;

// Create joystick constants
const int joystickInputBits = 4095; // Maximum possible input value for joystick
const float joystickThresholdMargin = 0.2; // Threshhold as a factor of the maximum value
const int joystickLeftThreshold = joystickInputBits/2-joystickThresholdMargin*joystickInputBits; // Left threshhold of the joystick, beyond which movement is recognized as left movement
const int joystickRightThreshold = joystickInputBits/2+joystickThresholdMargin*joystickInputBits; // Right threshhold of the joystick, beyond which movement is recognized as right movement

// Create game constants
const int leftFOV = (int) SCREEN_WIDTH*0.3; // Left threshhold of character position, before the background scrolls
const int rightFOV = (int) SCREEN_WIDTH*0.7; // Right threshhold of character position, before the background scrolls
const float parallaxFactor = 0.5; // Factor by which the frame scroll (parallax) of the background will be slowed down.

const int nAxes = 2; // Number of axes to fully describe a location, here 2 (room number and real location in the room)
const int nRooms = 3; // Number of rooms (0 = left office, 1 = right office, 2 = garden)
const int roomSizes[nRooms] = {200, 200, 200}; // Sizes (x-dimensions) of rooms, for parallax scrolling across the background (use sizes larger than the screen size)
const unsigned char* roomBackgrounds[nRooms] = {epd_bitmap_tama0, epd_bitmap_tama0, epd_bitmap_tama0}; // Define backgrounds for each room (drawn behind player), because these are actually pointers at this point
const unsigned char* roomForegrounds[nRooms] = {epd_bitmap_tama0, epd_bitmap_tama0, epd_bitmap_tama0}; // Define foregrounds for each room (drawn on top of player), because these are actually pointers at this point

const int roomWidth = SCREEN_WIDTH; // Width of a room frame
const int roomHeight = SCREEN_HEIGHT; // Heigth of a room frame
const int nameWidth = 20; // Width of the name
const int othersRoomLocsOffset = -10; // x offset each other player at same location is added at

const int nAvatars = 4; // Number of avatars possible in the game
const int playerAvatarYOffset = 0; // Offset in frame from y axis of own player
const int othersAvatarYOffset = 10; // Offset in frame from y axis of other players
const int avatarWidth = 128; // Width of an avatar frame
const int avatarHeigth = 64; // Height of an avatar frame
const unsigned char* allAvatars[nAvatars] = {epd_bitmap_tama0, epd_bitmap_tama0, epd_bitmap_tama0, epd_bitmap_tama0}; // Base avatar characters

const int animationSpeed = 50; // Screen is refreshed every x milliseconds (implemented as a delay at the end of the main loop)
const float frameSpeed = 0.1; // Relative frame speed (sub-stepping of animation speed), so every loop, the frame counter is increased by x. This makes it possible to advance the frame counter slowly while allowing for faster animations (like walking or not blocking the menu)
const float walkingSpeed = 4; // Relative walking speed while holding down button. NOTE: walkingSpeed*parallaxFactor MUST be a full (int) number
const int nFrames = 2; // Number of frames an animation consists of
const int nAnimations = 2; // Number of animations implemented in current version
const int nMaxRepetitions = 3; // How many times non-repetitive animations are being repeated
const unsigned char* workingAnimation[nAvatars][nFrames] = {{epd_bitmap_tama0, epd_bitmap_tama1},{epd_bitmap_tama0, epd_bitmap_tama1},{epd_bitmap_tama0, epd_bitmap_tama1},{epd_bitmap_tama0, epd_bitmap_tama1}}; // Frames of working animation of all avatars
const unsigned char* flowersAnimation[nAvatars][nFrames] = {{epd_bitmap_tama0, epd_bitmap_tama1},{epd_bitmap_tama0, epd_bitmap_tama1},{epd_bitmap_tama0, epd_bitmap_tama1},{epd_bitmap_tama0, epd_bitmap_tama1}}; // Frames of giving flowers animation of all avatars
const unsigned char* (*allAnimations[nAnimations])[nAvatars][nFrames] = {&workingAnimation, &flowersAnimation}; // Animation look up array
const bool repeatTypeAnimations[nAnimations] = {true, false}; // Whether the type of animation is a repeat one or execute once

const int nMaxOthers = 3; // Maximum number of other players
const int nMaxOthersPerLoc = 3; // Maximum number of other players per location

const int nActionMenuItemLabels = 3; // Total number of menu items in the action menu
const String actionMenuItemLabels[nActionMenuItemLabels] = {"Work", "Flowers", "Cancel"}; // String labels for each action item
const int actionMenuItemLabelHeight = 12; // Number of pixels each action item is high
const int actionMenuItemLabelMargin = 4; // Margin to the screen for each action menu item
const int actionMenuItemLabelFontSize = 1; // Font size for the label

/**
 * VARIABLE DECLARATION SECTION
*/
// Networking variables
WebSocketsClient webSocketClient;

// Joystick variables
int lastButtonPress = 0;

// Create OLED display object with given constraints
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
int imgSelector = 0; /// TODO: To be removed
bool currentPause = false; // Whether the player is currently in the start screen or in an active session
bool webSocketConnected = false; // Whether the WebSocket is currently connected

float currentFrame = 0.0; // Counts what animation frame is currently presented, increases after every frame
int currentPlayerAnimationRepeatCounter = 0; // Counts how many times the current animation has already been repeated for this player
int currentOthersAnimationRepeatCounter[nMaxOthers] = {0, 0, 0}; // Counts how many times the current animation has already been repeated for other players

int currentPlayerLoc[nAxes] = {0, (int)(avatarWidth/2)}; // Which room the player is currently in and what his real and rendered location is. Possible values are 0 (left office), 1 (right office), 2 (garden)
int currentOthersLocs[nMaxOthers][nAxes] = {{0, 0}, {0, 20}, {0, 40}};

long currentPlayerUID = -1; // Unique user ID, generated randomly or assigned by server
long currentOthersUIDs[nMaxOthers] = {-1, -1, -1}; // Unique user ids of other players joined

int currentPlayerAvatar = 0; // Which avatar the player has currently selected
int currentOthersAvatars[nMaxOthers] = {0, 0, 0}; // Which avatars the other players have selected

String currentPlayerName = "Cedi"; // Name initials of the current player
String currentOthersNames[nMaxOthers] = {"Jon", "Ana", "Dishi"}; // Name initials of the current player

int currentBackgroundLoc = 0; // Current location of the background the player is currently in (for rendering)
int currentPlayerAnimation = 0; // Which animation the player is currently executing
int currentPlayerPreviousAnimation = 0; // Which looped animation was executing before

int currentOthersAnimation[nMaxOthers] = {0, 0, 0}; // Which animation the other players are executing at the moment
int currentOthersPreviousAnimation[nMaxOthers] = {0, 0, 0}; // Which non-loop animation the other players are executing before

bool currentActionMenuOpen = false; // Whether or not the menu is currently open or not (has effects on LEFT and RIGHT button functionality)
int currentActionMenuItemLabelSelected = 0; // Which option in the menu is selected from default

/**
 * Enum for different event types
*/
enum PlayerEvent 
{
  LOGIN = 1,
  LOGOUT = 2,
  LOCCHANGE = 3,
  ANIMCHANGE = 4
};

const char* jsonEvent = "event";
const char* jsonUID = "uid";
const char* jsonName = "name";
const char* jsonAvatar = "avatar";
const char* jsonLoc = "location";
const char* jsonLoc0 = "room";
const char* jsonLoc1 = "position";
const char* jsonAnim = "animation";

/**
 * JSON Package Generator
*/
StaticJsonDocument<1000> generateJson(PlayerEvent playerEvent) {

  // Create empty JSON document
  StaticJsonDocument<1000> doc;
  doc[jsonUID] = currentPlayerUID;

  // Populate document with information depending on event type
  switch(playerEvent) {

    case LOGIN:
    {
      doc[jsonEvent] = LOGIN;

      doc[jsonName] = currentPlayerName;
      doc[jsonAvatar] = currentPlayerAvatar;

      JsonArray jsonLocArray = doc.createNestedArray(jsonLoc);
      jsonLocArray.add(currentPlayerLoc[0]);
      jsonLocArray.add(currentPlayerLoc[1]);

      doc[jsonAnim] = currentPlayerAnimation;
      break;
    }

    case LOGOUT:
    {
      doc[jsonEvent] = LOGOUT;
      break;
    }

    case LOCCHANGE:
    {
      doc[jsonEvent] = LOCCHANGE;

      JsonArray jsonLocArray = doc.createNestedArray(jsonLoc);
      jsonLocArray.add(currentPlayerLoc[0]);
      jsonLocArray.add(currentPlayerLoc[1]);
      break;
    }

    case ANIMCHANGE:
    {
      doc[jsonEvent] = ANIMCHANGE;

      doc[jsonAnim] = currentPlayerAnimation;
      break;
    }

    default :
      printf("[JsonGenerator] Unknown player event type %i\n", playerEvent);
      break;

  }

  // Return generated JSON document
  return doc;

}

/**
 * JSON Serializer & WebSocket Sender
*/
void sendJson(StaticJsonDocument<1000> doc) {

  if(!webSocketConnected) {
    Serial.println("[JsonSender] No WebServer connected, will skip sending JSON");
    return;
  }

  String jsonOutput;
  serializeJson(doc, jsonOutput);

  webSocketClient.sendTXT(jsonOutput);
  printf("[WebSocket] SENT: %s\n", jsonOutput);

}

/**
 * Assign new player id based on UID
*/
int assignPlayerId(long playerUID) {

  // Loop over UIDs and find last free spot
  for(int i = 0; i < nMaxOthers; i++) {

    // If UID is -1, it is a free slot
    if(currentOthersUIDs[i] == -1) {
      currentOthersUIDs[i] = playerUID;
      return i;
    }

  }

  // If no playerId assigned, they must be all occupied at the moment
  printf("[JsonReceiver] No more free player spots for UID %lo\n", playerUID);
  return -1;

}

/**
 * Deassign player id of UID for logout
*/
void deassignPlayerId(int playerId) {
  currentOthersUIDs[playerId] = -1;
}

/**
 * Find player based on UID
*/
int getPlayerId(long playerUID) {
  
  // Identify player based on UID by going through all players joined
  int playerId = -1;
  for(int i = 0; i < nMaxOthers; i++) {
    if(currentOthersUIDs[i] == playerUID) {
      playerId = i;
    }
  }

  // If lookup unsuccessful, it must be a new player
  if(playerId == -1) {
    printf("[JsonReceiver] No player recognized with UID %lo\n", playerUID);
  }

  return playerId;

}

/**
 * JSON Deserializer & WebSocket Receiver
*/
void receiveJson(uint8_t * payload) {

  printf("[WebSocket] RECEIVED: %s\n", payload);

  // Deserialize string into JSON
  StaticJsonDocument<1000> doc;
  DeserializationError error = deserializeJson(doc, payload);
  if (error) {
    Serial.print("[JsonReceiver] Deserialization error: ");
    Serial.println(error.f_str());
    return;
  }

  // Identify & update based on event
  const long playerUID = doc[jsonUID];
  int playerId = getPlayerId(playerUID);

  const PlayerEvent playerEvent = doc[jsonEvent];
  switch(playerEvent) {

    case LOGIN:
    {

      // If no playerId assigned, the player is joining for the first time this session
      if(playerId == -1) {
        playerId = assignPlayerId(playerUID);
        printf("[JsonReceiver] Welcome player with UID %lo\n", playerUID);

        // If still no playerId assigned, there are too many players
        if(playerId == -1) {
          printf("[JsonReceiver] No more space for player with UID %lo\n", playerUID);
          return;
        }

      }

      // Parse login information
      const char* playerName = doc[jsonName];
      currentOthersNames[playerId] = playerName;

      const int playerAvatar = doc[jsonAvatar];
      currentOthersAvatars[playerId] = playerAvatar;

      currentOthersLocs[playerId][0] = doc[jsonLoc][0];
      currentOthersLocs[playerId][1] = doc[jsonLoc][1];

      const int playerAnim = doc[jsonAnim];
      currentOthersAnimation[playerId] = playerAnim;

      printf("[JsonReceiver] LOGIN processed for player %i\n", playerId);
      break;

    }

    case LOGOUT:
    {
      // If no playerId assigned, the player hasn't actually joined / was in the waiting room
      if(playerId == -1) {
        printf("[JsonReceiver] No logout needed for player with UID %lo\n", playerUID);
        return;
      }

      // Complete logout event
      deassignPlayerId(playerId);

      printf("[JsonReceiver] LOGOUT processed for player %i\n", playerId);
      break;
    }

    case LOCCHANGE:
    {
      // If no playerId assigned, the player hasn't actually joined / was in the waiting room
      if(playerId == -1) {
        printf("[JsonReceiver] No location for player with UID %lo\n", playerUID);
        return;
      }

      // Parse location change information
      currentOthersLocs[playerId][0] = doc[jsonLoc][0];
      currentOthersLocs[playerId][1] = doc[jsonLoc][1];

      printf("[JsonReceiver] LOCCHANGE processed for player %i\n", playerId);
      break;
    }

    case ANIMCHANGE:
    {
      // If no playerId assigned, the player hasn't actually joined / was in the waiting room
      if(playerId == -1) {
        printf("[JsonReceiver] No animation for player with UID %lo\n", playerUID);
        return;
      }

      // Parse animation change information
      const int playerAnim = doc[jsonAnim];
      currentOthersAnimation[playerId] = playerAnim;

      printf("[JsonReceiver] ANIMCHANGE processed for player %i\n", playerId);
      break;
    }

    default :
      printf("[JsonReceiver] Unknown player event type %i\n", playerEvent);
      return;

  }

} 

/**
 * WEBSOCKET EVENT HANDLER FUNCTION
*/
void onWebSocketEvent(WStype_t eventType, uint8_t * payload, size_t length) {

  switch(eventType) {

    // If WebSocket disconnected
    case WStype_DISCONNECTED:
      webSocketConnected = false;
      Serial.println("[WebSocket] Disconnected");
      break;

    // If WebSocket connected
    case WStype_CONNECTED:
    {
      webSocketConnected = true;
      printf("[WebSocket] Connected to %s\n", payload);

      // Generate and send JSON of current information
      StaticJsonDocument<1000> doc = generateJson(LOGIN);
      sendJson(doc);

      break;
    }

    // If WebSocket sends text in
    case WStype_TEXT:
    {
      // Receive and interpret/update information based on JSON
      receiveJson(payload);
      break;
    }

    // For all other cases, ignore
    case WStype_BIN:
    case WStype_ERROR:      
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
      break;

  }

}

/**
 * SETUP FUNCTION
*/
void setup() {

  // Launch debugging console
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  delay(100);

  // Generate unique random user ID of all possible long values
  currentPlayerUID = rand() % 2147483647;
  printf("[Setup] Generated player UID %lo\n", currentPlayerUID);

  /**
   * Network setup section
  */
  Serial.print("[Wifi] Connecting to network ");
  Serial.println(ssid);

  // Reset wifi board
  WiFi.disconnect();
  WiFi.begin(ssid, password);

  // Connect to local wifi
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println("[WiFi] Connected successfully");

  // Open WebSocket connection to WebSocket server on host, port, URL
  webSocketClient.beginSSL(host, port, url);
  webSocketClient.setReconnectInterval(500);

  // Add event listener to WebSocket connection
  webSocketClient.onEvent(onWebSocketEvent);

  /**
   * Peripherals setup section
  */
  // Try to initialize screen object under given screen address, block if unsuccessful
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {

    Serial.println("[Setup] Error: Display could not be found");
    for(;;);

  }

  // Clear the display and buffer
  display.clearDisplay();

  // Set encoder pins as input pins
  pinMode(VRX, INPUT);
  pinMode(VRY, INPUT);
  pinMode(SW, INPUT_PULLUP);

  // Define pin modes
  pinMode(LEFT_BUTTON, INPUT_PULLUP);
  pinMode(MIDDLE_BUTTON, INPUT_PULLUP);
  pinMode(RIGHT_BUTTON, INPUT_PULLUP);
  
}

/**
 * LOOP FUNCTION
*/
void loop() {

  /**
   * Networking section
  */
  // Keep WebSocket client connection alive and check for events
  webSocketClient.loop();

  /**
   * Button input section
  */
  // Check if any buttons are currently pressed
  int leftButtonPressed = HIGH;//digitalRead(LEFT_BUTTON);
  int middleButtonPressed = HIGH;//digitalRead(MIDDLE_BUTTON);
  int rightButtonPressed = HIGH;//digitalRead(RIGHT_BUTTON);

  if (millis() - lastButtonPress > 200) {

    // Read the y position of the joystick
    int yValue = analogRead(VRY);
    int btnState = digitalRead(SW);
    bool buttonPressed = false;

    // If walking to right
    if(yValue <= joystickLeftThreshold) {
      rightButtonPressed = LOW;
      buttonPressed = true;
    }

    // If walking to left
    else if(yValue >= joystickRightThreshold) {
      leftButtonPressed = LOW;
      buttonPressed = true;
    }

    // If center button pressed
    else if(btnState == LOW) {
      middleButtonPressed = LOW;
      buttonPressed = true;
    }

    // Remember last button press event
    if(buttonPressed) {
      lastButtonPress = millis();
    }

  }

  /**
   * Game output section
  */
  // Clear the display and buffer
  display.clearDisplay();

  // Draw background of current room
  int currentRoom = currentPlayerLoc[0];
  int currentLoc = currentPlayerLoc[1];
  int currentCameraLoc = -(int)(currentBackgroundLoc/parallaxFactor);

  display.drawRect(currentBackgroundLoc, 0, roomSizes[currentRoom], roomHeight, WHITE);
  //display.drawBitmap(currentBackgroundLoc, 0,  roomBackgrounds[currentRoom], roomWidth, roomHeight, BITMAP_COLOR);

  // Draw all other characters at their respective locations into room
  for (int i = 0; i < nMaxOthers; i++) {

    // Draw only if the avatar is assigned to a real player UID, else skip
    if(currentOthersUIDs[i] == -1) {
      continue;
    }

    // Draw avatar only if in the current room, else skip this one
    if(!currentOthersLocs[i][0] == currentRoom) {
      continue;
    }

    int otherAvatar = currentOthersAvatars[i]; // Select other player's avatar
    int otherAvatarAnimation = currentOthersAnimation[i]; // Select other player's current animation

    // Draw other avatar with respective animation
    int otherAvatarRenderX = -currentCameraLoc+currentOthersLocs[i][1]-(int)(avatarWidth/2);
    //display.drawBitmap(otherAvatarRenderX, othersAvatarYOffset, (*allAnimations[otherAvatarAnimation])[otherAvatar][(int)currentFrame], avatarWidth, avatarHeigth, BITMAP_COLOR);

    // Draw other name initials above the avatar in a blanc rectangle
    String otherName = currentOthersNames[i];
    display.setCursor(otherAvatarRenderX+(int)(avatarWidth/2)+(int)(-nameWidth/2), othersAvatarYOffset);
    display.setTextSize(1);
    display.setTextWrap(false);
    display.setTextColor(WHITE, BLACK);
    display.println(otherName);

    // Determine whether animation will be repeated again or not
    bool othersAvatarAnimationRepeat = repeatTypeAnimations[otherAvatarAnimation];

    // If not repeated, increase the counter and potentially kick out animation
    if(!othersAvatarAnimationRepeat) {

      currentOthersAnimationRepeatCounter[i] += 1; // Increase non-repeat animation counter by 1
      if(currentOthersAnimationRepeatCounter[i]>=nMaxRepetitions*nFrames) {
        currentOthersAnimation[i] = currentOthersPreviousAnimation[i]; // If non-repeat animation has been repeated enough times, loop back to last repeat animation
      }

    }

    // If repeated, potentially assign this animation as he previous one to return to if anything happens
    else {

      if(currentOthersPreviousAnimation[i] != currentOthersAnimation[i]) {
        currentOthersPreviousAnimation[i] = currentOthersAnimation[i];
        currentOthersAnimationRepeatCounter[i] = 0;
      }

    }

  }

  // Draw own player's avatar with animation in current frame
  int playerAvatarRenderX = currentLoc-(int)(avatarWidth/2)-currentCameraLoc;
  //display.drawBitmap(playerAvatarRenderX, playerAvatarYOffset, (*allAnimations[currentPlayerAnimation])[currentPlayerAvatar][(int)currentFrame], avatarWidth, avatarHeigth, BITMAP_COLOR);

  // Draw own player's name initials above the avatar in a blanc rectangle
  display.setCursor(playerAvatarRenderX+(int)(avatarWidth/2)+(int)(-nameWidth/2), playerAvatarYOffset);
  display.setTextSize(1);
  display.setTextWrap(false);
  display.setTextColor(WHITE, BLACK);
  display.println(currentPlayerName);

  // Determine whether animation will be repeated again or not
  bool playerAvatarAnimationRepeat = repeatTypeAnimations[currentPlayerAnimation];

  // If not repeated, increase the counter and potentially kick out animation
  if(!playerAvatarAnimationRepeat) {

    currentPlayerAnimationRepeatCounter += 1; // Increase non-repeat animation counter by 1
    if(currentPlayerAnimationRepeatCounter>=nMaxRepetitions*nFrames) {
      currentPlayerAnimation = currentPlayerPreviousAnimation; // If non-repeat animation has been repeated enough times, loop back to default animation
    }

  }

  // If repeated, potentially assign this animation as he previous one to return to if anything happens
  else {

    if(currentPlayerPreviousAnimation != currentPlayerAnimation) {
      currentPlayerPreviousAnimation = currentPlayerAnimation;
      currentPlayerAnimationRepeatCounter = 0;
    }

  }

  // Draw foreground of current room
  display.fillRect(-currentCameraLoc+60, 44, 20, 20, BLACK);
  display.fillRect(-currentCameraLoc+150, 44, 20, 20, BLACK);
  // display.drawBitmap(0, 0,  roomForegrounds[currentRoom], roomWidth, roomHeight, BITMAP_COLOR);
  
  // Draw section for or action menu
  if(currentActionMenuOpen) {

    // Draw the action menu rectangle
    display.fillRect(0, 0, SCREEN_WIDTH, actionMenuItemLabelHeight*nActionMenuItemLabels, BLACK);
    
    for(int i = 0; i < nActionMenuItemLabels; i++) {
      
      display.setCursor(actionMenuItemLabelMargin, actionMenuItemLabelMargin+i*actionMenuItemLabelHeight);
      display.setTextSize(actionMenuItemLabelFontSize);
      
      // If this menu item is currently selected, highlight it
      if(currentActionMenuItemLabelSelected == i) {
        display.setTextColor(BLACK, WHITE);
      }

      // Otherwise, just keep it as it is
      else {
        display.setTextColor(WHITE);
      }
      
      // Draw the action menu item
      display.println(actionMenuItemLabels[i]);

    }

  }

  // If action menu is currently open, the following will be checked to determine button interpretation
  if(currentActionMenuOpen) {

    // Execute action if middle button pressed
    if(middleButtonPressed == LOW) {

      // Only change animation if not cancel (last) option was chosen and different
      if(currentActionMenuItemLabelSelected < nAnimations && currentActionMenuItemLabelSelected != currentPlayerAnimation) {
        currentPlayerAnimation = currentActionMenuItemLabelSelected;
        
        // Generate and send JSON of changed animation
        StaticJsonDocument<1000> doc = generateJson(ANIMCHANGE);
        sendJson(doc);
      }

      currentActionMenuOpen = false;
    }

    // Move to next or previous menu item if side buttons pressed
    else if(leftButtonPressed == LOW) {
      currentActionMenuItemLabelSelected-=1;
    }

    else if(rightButtonPressed == LOW) {
      currentActionMenuItemLabelSelected+=1;
    }

  }

  // Otherwise, the following will be checked to interpret button clicks
  else {

    // Open menu if middle button pressed
    if(middleButtonPressed == LOW) {
      currentActionMenuOpen = true;
    }

    // Move to next or previous location if side buttons pressed
    else if(leftButtonPressed == LOW) {

      // If player moving beyond left FOV, move background (parallax) layer (unless already at end of room)
      if(currentPlayerLoc[1] < leftFOV+currentCameraLoc && currentBackgroundLoc < 0) {
        currentBackgroundLoc+=(int)(walkingSpeed*parallaxFactor); 
      }

      // Move actual player position
      currentPlayerLoc[1]-=walkingSpeed;

      // Generate and send JSON of changed animation
      StaticJsonDocument<1000> doc = generateJson(LOCCHANGE);
      sendJson(doc);

    }

    else if(rightButtonPressed == LOW) {

      // If player moving beyond right FOV, move background (parallax) layer (unless already at end of room)
      if(currentPlayerLoc[1] > rightFOV+currentCameraLoc && currentBackgroundLoc > SCREEN_WIDTH-roomSizes[currentPlayerLoc[0]]) {
        currentBackgroundLoc-=(int)(walkingSpeed*parallaxFactor);
      }

      // Move actual player position
      currentPlayerLoc[1]+=walkingSpeed;
      
      // Generate and send JSON of changed animation
      StaticJsonDocument<1000> doc = generateJson(LOCCHANGE);
      sendJson(doc);
        
    }
      
  }

  // Display current frame
  display.display();
  delay(animationSpeed);

  // Fix outside bounds for player location
  if(currentPlayerLoc[1] > 2*roomSizes[currentPlayerLoc[0]]-SCREEN_WIDTH) {
    Serial.println("Next room");
    currentPlayerLoc[0]+=1;
    currentPlayerLoc[1] = 0;
    currentBackgroundLoc = 0;
  }

  else if(currentPlayerLoc[1] < 0) {
    Serial.println("Previous room");
    currentPlayerLoc[0]-=1;
    currentPlayerLoc[1] = 2*roomSizes[currentPlayerLoc[0]]-SCREEN_WIDTH;
    currentBackgroundLoc = SCREEN_WIDTH-roomSizes[currentPlayerLoc[0]];
  }

  // Fix outside bounds for player room
  if(currentPlayerLoc[0] >= nRooms) {
    Serial.println("Already last room");
    currentPlayerLoc[0] = nRooms-1;
    currentPlayerLoc[1] = 2*roomSizes[currentPlayerLoc[0]]-SCREEN_WIDTH;
    currentBackgroundLoc = SCREEN_WIDTH-roomSizes[currentPlayerLoc[0]];
  }

  else if(currentPlayerLoc[0] < 0) {
    Serial.println("Already first room");
    currentPlayerLoc[0] = 0;
    currentPlayerLoc[1] = 0;
    currentBackgroundLoc = 0;
  }

  // Fix outside bounds for menu item selected
  if(currentActionMenuItemLabelSelected >= nActionMenuItemLabels) {
    currentActionMenuItemLabelSelected = 0;
  }

  else if(currentActionMenuItemLabelSelected < 0) {
    currentActionMenuItemLabelSelected = nActionMenuItemLabels-1;
  }

  // Increase to next animation frame for next loop cycle (and reset if beyond the maximum frame number)  
  currentFrame+=frameSpeed;

  if(currentFrame>=nFrames) {
    currentFrame = 0;
  }
  
}