/**
 * Buddi Sketch, V 1.0
 * Compatible with Arduino Nano 33 BLE
 * 
 * (c) Cedric-Pascal Sommer, 2023
*/

/**
 * MANDATORY TODO SECTION
 * TODO: Two display drivers: Change included libraries & change which type of object is created as display! and where the display is first instantiated
 * TODO: Make JSON smaller (not 1000 bytes, we need much less!)
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
 * TODO: Send data via UDP (WebRTC) instead of TCP (WebSocket)
 * TODO: Write your name screen
 * TODO: Choose your avatar screen
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
//#include <Adafruit_SSD1306.h> // Display library for SSD1306 drivers
#include <Adafruit_SH1106.h> // Display library for SH1106 drivers
#include <ctime>

// Define OLED display configuration
#define MAX_LONG 2147483647 // maximum value of a long variable
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

#define LEFT 27
#define RIGHT 26
#define MIDDLE 14

/**
 * BITMAP (IMAGE) DECLARATION SECTION
*/
// 'avt2_trumpet1', 24x24px
const unsigned char  avt2_trumpet1 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 
	0x00, 0x40, 0x00, 0x00, 0xf0, 0x00, 0x0f, 0xe0, 0x00, 0x10, 0x10, 0x00, 0x22, 0x08, 0x30, 0x24, 
	0x08, 0x28, 0xe3, 0x08, 0x27, 0xfe, 0x08, 0x20, 0x29, 0x08, 0x27, 0xfc, 0x08, 0x29, 0x20, 0x08, 
	0x30, 0xf8, 0x08, 0x00, 0x07, 0xf0, 0x00, 0x01, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'avt2_trumpet0', 24x24px
const unsigned char  avt2_trumpet0 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 
	0x00, 0xa8, 0x00, 0x00, 0x70, 0x00, 0x07, 0xf0, 0x00, 0x08, 0x08, 0x00, 0x10, 0x04, 0x18, 0x12, 
	0x04, 0x14, 0x70, 0x04, 0x13, 0xfe, 0x04, 0x10, 0x14, 0x04, 0x13, 0xfe, 0x04, 0x14, 0x90, 0x04, 
	0x18, 0x7c, 0x08, 0x00, 0x07, 0xf0, 0x00, 0x01, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'avt2_work1', 24x24px
const unsigned char  avt2_work1 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0xa8, 0x00, 0x00, 
	0x70, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x80, 0x80, 0x01, 0x00, 0x40, 0x05, 0x10, 0xc0, 0x05, 0x00, 
	0x40, 0x07, 0x1f, 0xc0, 0x09, 0x09, 0x60, 0xff, 0xff, 0xdf, 0x05, 0xaa, 0xf0, 0x04, 0x08, 0x10, 
	0x05, 0x6b, 0xb0, 0x04, 0x08, 0x10, 0x07, 0xeb, 0xf0, 0x04, 0x1c, 0x10, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'avt2_work0', 24x24px
const unsigned char  avt2_work0 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x0a, 0x80, 0x00, 
	0x07, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x80, 0x80, 0x01, 0x00, 0x40, 0x21, 0x84, 0x40, 0x11, 0x00, 
	0x40, 0x0d, 0xfc, 0x40, 0x13, 0x48, 0x40, 0xff, 0xff, 0x7f, 0x05, 0xaa, 0xd0, 0x04, 0x08, 0x10, 
	0x05, 0x6b, 0xb0, 0x04, 0x08, 0x10, 0x07, 0xeb, 0xf0, 0x04, 0x1c, 0x10, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'avt2_music1', 24x24px
const unsigned char  avt2_music1 [] PROGMEM = {
	0x00, 0x00, 0x3c, 0x00, 0x01, 0x6a, 0x00, 0x00, 0xf1, 0x00, 0x0f, 0xfd, 0x00, 0x10, 0x0d, 0x00, 
	0x2c, 0xdb, 0x00, 0x28, 0x5b, 0x00, 0x20, 0x1a, 0x00, 0x7f, 0x8e, 0x00, 0xa9, 0xc2, 0x7f, 0x69, 
	0xcc, 0x61, 0x3f, 0xc8, 0x61, 0x1f, 0x88, 0x7f, 0x28, 0x14, 0x6d, 0x37, 0xec, 0x73, 0x28, 0x04, 
	0x73, 0xf0, 0x00, 0x6d, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'avt2_music0', 24x24px
const unsigned char  avt2_music0 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 
	0x02, 0xd4, 0x00, 0x01, 0xe2, 0x00, 0x0f, 0xfa, 0x00, 0x10, 0x1a, 0x00, 0x20, 0x36, 0x7f, 0x30, 
	0xb6, 0x61, 0x20, 0x34, 0x61, 0x3f, 0x9c, 0x7f, 0x29, 0x24, 0x6d, 0x29, 0x18, 0x73, 0x3f, 0x08, 
	0x73, 0x90, 0x10, 0x6d, 0x6f, 0xe0, 0x7f, 0x18, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'avt2_eating1', 24x24px
const unsigned char  avt2_eating1 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x15, 0x00, 0x00, 0x0e, 0x00, 0x00, 
	0xfe, 0x00, 0x01, 0x01, 0x00, 0x02, 0xcc, 0x80, 0x02, 0x84, 0x80, 0x02, 0x00, 0xc0, 0x03, 0xf8, 
	0x20, 0x04, 0x8c, 0x20, 0x09, 0x1c, 0xc0, 0x0f, 0xfc, 0x80, 0x01, 0xf8, 0x80, 0x00, 0x81, 0x00, 
	0x01, 0x7e, 0x80, 0x03, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'avt2_eating0', 24x24px
const unsigned char  avt2_eating0 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 
	0x15, 0x00, 0x00, 0x0e, 0x00, 0x00, 0xfe, 0x00, 0x01, 0x01, 0x00, 0x02, 0x00, 0x80, 0x03, 0x18, 
	0x80, 0x02, 0x04, 0x80, 0x02, 0x10, 0x80, 0x03, 0xf8, 0x40, 0x02, 0x92, 0x40, 0x03, 0xf1, 0x80, 
	0x00, 0xfe, 0x00, 0x00, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'avt2_default1', 24x24px
const unsigned char  avt2_default1 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 
	0x54, 0x00, 0x00, 0x38, 0x00, 0x03, 0xf8, 0x00, 0x04, 0x04, 0x00, 0x08, 0x02, 0x00, 0x0c, 0x22, 
	0x00, 0x08, 0x02, 0x00, 0x0f, 0xe1, 0x00, 0x0a, 0x49, 0x00, 0x0a, 0x46, 0x80, 0x0f, 0xc0, 0x80, 
	0x01, 0x01, 0x00, 0x00, 0xfe, 0x00, 0x00, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'avt2_default0', 24x24px
const unsigned char  avt2_default0 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 
	0x15, 0x00, 0x00, 0x0e, 0x00, 0x00, 0xfe, 0x00, 0x01, 0x01, 0x00, 0x02, 0x00, 0x80, 0x03, 0x08, 
	0x80, 0x02, 0x00, 0x80, 0x03, 0xf8, 0x40, 0x02, 0x92, 0x40, 0x02, 0x91, 0x80, 0x03, 0xf0, 0x80, 
	0x01, 0x01, 0x00, 0x00, 0xfe, 0x00, 0x01, 0x83, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'avt1_work1', 24x24px
const unsigned char  avt1_work1 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 0x07, 0xc0, 0xf0, 0x07, 0xc1, 0xf8, 0x07, 
	0xc3, 0xf8, 0x01, 0xc3, 0xc0, 0x01, 0xff, 0x80, 0x01, 0x00, 0x80, 0x12, 0x00, 0x40, 0x15, 0x86, 
	0x20, 0x1c, 0x00, 0x20, 0x24, 0x78, 0x20, 0x1e, 0x00, 0x70, 0xff, 0xff, 0x9f, 0x0b, 0x55, 0x90, 
	0x08, 0x10, 0x60, 0x08, 0x10, 0x20, 0x0d, 0xd6, 0xa0, 0x08, 0x10, 0x20, 0x0f, 0xd7, 0xe0, 0x08, 
	0x38, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'avt1_work0', 24x24px
const unsigned char  avt1_work0 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x07, 0x81, 0xf0, 0x0f, 0xc1, 0xf0, 0x0f, 
	0xe1, 0xf0, 0x01, 0xe1, 0xc0, 0x00, 0xff, 0xc0, 0x00, 0x80, 0x40, 0x41, 0x00, 0x20, 0x22, 0x61, 
	0x90, 0x1a, 0x00, 0x10, 0x26, 0x1e, 0x10, 0x1f, 0x00, 0x38, 0xff, 0xff, 0xcf, 0x05, 0xaa, 0xc8, 
	0x04, 0x08, 0x30, 0x04, 0x08, 0x10, 0x06, 0xeb, 0x50, 0x04, 0x08, 0x10, 0x07, 0xeb, 0xf0, 0x04, 
	0x1c, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'avt1_trumpet1', 24x24px
const unsigned char  avt1_trumpet1 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x01, 0xe0, 0x00, 0x03, 0xdc, 0x00, 0x07, 0xbe, 0x00, 0x07, 0xfe, 0x30, 0x08, 0x18, 0x28, 0x13, 
	0x08, 0x27, 0xe0, 0x04, 0x20, 0x1b, 0x04, 0x27, 0xf4, 0x08, 0x29, 0x1b, 0x90, 0x30, 0xe8, 0x10, 
	0x00, 0x04, 0x08, 0x00, 0x02, 0xe8, 0x00, 0x03, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'avt1_trumpet0', 24x24px
const unsigned char  avt1_trumpet0 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x00, 
	0x01, 0xc0, 0x00, 0x03, 0xdc, 0x00, 0x07, 0xbe, 0x00, 0x07, 0xfe, 0x30, 0x08, 0x18, 0x28, 0x11, 
	0x08, 0x27, 0xe0, 0x04, 0x20, 0x1d, 0x04, 0x27, 0xf2, 0x88, 0x29, 0x1a, 0x10, 0x30, 0xe9, 0xd0, 
	0x00, 0x08, 0x10, 0x00, 0x05, 0xa0, 0x00, 0x06, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'avt1_music1', 24x24px
const unsigned char  avt1_music1 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x78, 0x00, 0x7f, 0xf8, 0x00, 
	0x3e, 0xf0, 0x00, 0x5f, 0xf8, 0x00, 0xf0, 0x3c, 0x01, 0x2c, 0xd2, 0x01, 0x48, 0x4a, 0x01, 0x43, 
	0x0a, 0xff, 0xf3, 0x3c, 0xc2, 0xc8, 0x48, 0xfe, 0xb0, 0x30, 0xda, 0x90, 0x20, 0xe6, 0x97, 0xa0, 
	0xdb, 0x2c, 0xa0, 0xfe, 0x30, 0xa0, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'avt1_music0', 24x24px
const unsigned char  avt1_music0 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x60, 0x00, 
	0x1e, 0xf0, 0x00, 0x1f, 0xf0, 0x00, 0x3e, 0xf0, 0x00, 0x5f, 0xf8, 0x00, 0xf0, 0x3c, 0x01, 0x24, 
	0x92, 0xff, 0x58, 0x6a, 0xc3, 0x44, 0x8a, 0xff, 0xf3, 0x3c, 0xda, 0xc8, 0x48, 0xe6, 0xb0, 0x30, 
	0xdb, 0x50, 0x20, 0xfe, 0x8b, 0xa0, 0x00, 0x0c, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'avt1_eating1', 24x24px
const unsigned char  avt1_eating1 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 
	0x7c, 0xe0, 0x01, 0xfb, 0xf0, 0x02, 0x07, 0xf0, 0x05, 0xe1, 0x90, 0x05, 0xf2, 0x80, 0x05, 0xf0, 
	0x80, 0x02, 0xe0, 0x80, 0x01, 0x01, 0x00, 0x01, 0x0a, 0x00, 0x01, 0x0a, 0x00, 0x01, 0x06, 0x00, 
	0x01, 0x02, 0x00, 0x00, 0xba, 0x00, 0x00, 0xaa, 0x00, 0x00, 0xc6, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'avt1_eating0', 24x24px
const unsigned char  avt1_eating0 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 
	0x38, 0x00, 0x00, 0x7b, 0x80, 0x00, 0xf7, 0xc0, 0x00, 0xff, 0xc0, 0x01, 0x03, 0x00, 0x02, 0x21, 
	0x00, 0x04, 0x00, 0x80, 0x07, 0x00, 0x80, 0x02, 0x15, 0x00, 0x01, 0x16, 0x00, 0x01, 0x0a, 0x00, 
	0x01, 0x02, 0x00, 0x00, 0xba, 0x00, 0x00, 0xaa, 0x00, 0x00, 0xc6, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'avt1_default0', 24x24px
const unsigned char  avt1_default0 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x87, 0x80, 0x03, 
	0xc7, 0x80, 0x03, 0xe7, 0x80, 0x01, 0xff, 0x80, 0x00, 0x81, 0x80, 0x01, 0x66, 0x80, 0x02, 0x22, 
	0x40, 0x02, 0x00, 0x40, 0x01, 0x19, 0x80, 0x02, 0x82, 0x40, 0x01, 0x81, 0x80, 0x00, 0x81, 0x00, 
	0x00, 0x5c, 0x80, 0x00, 0x53, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'avt1_default1', 24x24px
const unsigned char  avt1_default1 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x01, 0xe1, 0xc0, 0x01, 
	0xe3, 0xc0, 0x01, 0xe7, 0xc0, 0x01, 0xff, 0x80, 0x01, 0x81, 0x00, 0x01, 0x66, 0x80, 0x02, 0x44, 
	0x40, 0x02, 0x00, 0x40, 0x01, 0x98, 0x80, 0x02, 0x41, 0x40, 0x01, 0x81, 0x80, 0x00, 0x81, 0x00, 
	0x01, 0x3a, 0x00, 0x00, 0xca, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'avt0_trumpet1', 24x24px
const unsigned char  avt0_trumpet1 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x15, 0x00, 0x00, 0x1b, 0x00, 0x01, 0xd1, 0x70, 0x01, 0x3f, 0x90, 0x19, 0x41, 0x50, 0x15, 0xa2, 
	0x20, 0x13, 0xf9, 0x20, 0x10, 0x04, 0x20, 0x13, 0xf8, 0x60, 0x14, 0x87, 0xc0, 0x18, 0x88, 0x40, 
	0x00, 0x7c, 0x40, 0x00, 0x10, 0x40, 0x00, 0x17, 0x20, 0x00, 0x08, 0xc0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'avt0_music1', 24x24px
const unsigned char  avt0_music1 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0xf8, 0x00, 0x0d, 0x84, 0x00, 0x08, 0x9a, 0x00, 
	0x0f, 0xfa, 0x00, 0x10, 0x36, 0x00, 0x21, 0xb4, 0x00, 0x20, 0x9c, 0x00, 0x2c, 0x08, 0x3f, 0xb0, 
	0x1c, 0x30, 0xaf, 0xe4, 0x30, 0x90, 0x08, 0x3f, 0x8c, 0x10, 0x36, 0x88, 0x10, 0x39, 0x9b, 0xc8, 
	0x39, 0xe4, 0x30, 0x36, 0x80, 0x00, 0x3f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'avt0_music0', 24x24px
const unsigned char  avt0_music0 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0xf8, 0x00, 
	0x0d, 0x84, 0x00, 0x08, 0x9a, 0x00, 0x0f, 0xfa, 0x00, 0x10, 0x36, 0x00, 0x20, 0x34, 0x3f, 0xa0, 
	0x9c, 0x30, 0xac, 0x08, 0x30, 0xb0, 0x18, 0x3f, 0x8f, 0xe8, 0x36, 0x84, 0x04, 0x39, 0x84, 0x1c, 
	0x39, 0xcc, 0x10, 0x36, 0xb5, 0xd0, 0x3f, 0x82, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'avt0_work1', 24x24px
const unsigned char  avt0_work1 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xa0, 0x0e, 0x03, 0x60, 0x12, 
	0x02, 0x3c, 0x16, 0x07, 0xe4, 0x36, 0x08, 0x17, 0x36, 0x10, 0xcd, 0x56, 0x16, 0x4f, 0x56, 0x16, 
	0x0d, 0x52, 0x08, 0x1f, 0x9e, 0x07, 0xed, 0x85, 0xe2, 0x17, 0x86, 0x1e, 0x25, 0xff, 0xf0, 0x27, 
	0xff, 0xff, 0xfd, 0xc0, 0x35, 0x57, 0xc0, 0x37, 0xfc, 0xc0, 0x30, 0x60, 0xc0, 0x33, 0xfc, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'avt0_trumpet0', 24x24px
const unsigned char  avt0_trumpet0 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x15, 0x00, 0x00, 0x1b, 0x00, 0x01, 0xd1, 0x70, 0x01, 0x3f, 0x90, 0x19, 0x40, 0x50, 0x15, 0xa1, 
	0x20, 0x13, 0xf8, 0x20, 0x10, 0x04, 0x20, 0x13, 0xf8, 0x60, 0x14, 0x87, 0xc0, 0x18, 0x88, 0x80, 
	0x00, 0x7c, 0x80, 0x00, 0x20, 0x80, 0x00, 0x2e, 0x80, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'avt0_work0', 24x24px
const unsigned char  avt0_work0 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xa0, 0x0e, 0x03, 0x60, 0x12, 
	0x02, 0x3c, 0x16, 0x07, 0xe4, 0x36, 0x08, 0x17, 0x36, 0x10, 0x0d, 0x56, 0x10, 0x4f, 0x56, 0x16, 
	0x0d, 0x52, 0x08, 0x17, 0x9e, 0x07, 0xf5, 0x85, 0xe2, 0x17, 0x86, 0x16, 0x55, 0xff, 0xf8, 0x27, 
	0xff, 0xfb, 0xfd, 0xc0, 0x35, 0x57, 0xc0, 0x37, 0xfc, 0xc0, 0x30, 0x60, 0xc0, 0x33, 0xfc, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'avt0_eating1', 24x24px
const unsigned char  avt0_eating1 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0x00, 0x00, 
	0x6c, 0x00, 0x00, 0x44, 0x00, 0x00, 0xff, 0xc0, 0x01, 0x01, 0x40, 0x02, 0x00, 0xc0, 0x02, 0xc4, 
	0x80, 0x02, 0xc0, 0x80, 0x03, 0x01, 0xc0, 0x01, 0xfe, 0x40, 0x01, 0x00, 0x80, 0x01, 0x41, 0x00, 
	0x00, 0x81, 0x00, 0x00, 0xbc, 0x80, 0x00, 0x43, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'avt0_eating0', 24x24px
const unsigned char  avt0_eating0 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa8, 0x00, 0x00, 
	0xd8, 0x00, 0x00, 0x89, 0xc0, 0x00, 0xfe, 0x40, 0x01, 0x01, 0x40, 0x02, 0x00, 0x80, 0x02, 0x08, 
	0x80, 0x02, 0xc0, 0x80, 0x03, 0x01, 0x80, 0x00, 0xfe, 0x80, 0x00, 0x40, 0x40, 0x00, 0x41, 0xc0, 
	0x00, 0x81, 0x00, 0x00, 0xbc, 0x80, 0x00, 0x43, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'avt0_default1', 24x24px
const unsigned char  avt0_default1 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2a, 0x00, 0x00, 
	0x36, 0x00, 0x03, 0xa2, 0xe0, 0x02, 0x7f, 0x20, 0x02, 0x80, 0xa0, 0x01, 0x00, 0x40, 0x01, 0x41, 
	0x40, 0x01, 0x0c, 0x40, 0x03, 0x80, 0xc0, 0x02, 0x7f, 0x00, 0x01, 0x00, 0x80, 0x00, 0x82, 0x80, 
	0x01, 0x81, 0x80, 0x01, 0x3d, 0x00, 0x00, 0xc2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'avt0_default0', 24x24px
const unsigned char  avt0_default0 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0x00, 0x00, 
	0x6c, 0x00, 0x07, 0x45, 0xc0, 0x04, 0xfe, 0x40, 0x05, 0x01, 0x40, 0x02, 0x00, 0x80, 0x02, 0x82, 
	0x80, 0x02, 0x30, 0x80, 0x03, 0x01, 0xc0, 0x00, 0xfe, 0x40, 0x01, 0x00, 0x80, 0x01, 0x41, 0x00, 
	0x01, 0x81, 0x00, 0x00, 0xbc, 0x80, 0x00, 0x43, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/**
 * CONSTANT DECLARATION SECTION
*/
// Create networking constants
const char* ssid = "Harvard University";
const char* password = "";

const char* host = "socketsbay.com";
const char* url = "/wss/v2/1/478740e1078143056611c72c1383792c/";
const int port = 443;
//const char* host = "s8934.nyc1.piesocket.com";
//const char* url = "/v3/1?api_key=fkfI6GvBmoc9i4iAzNUlEHJFVlKv0kXXBPEPBrit";
//const int port = 443;

// Create joystick constants
const bool joystickActive = true; // Whether to use joystick values
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
const int roomSizes[nRooms] = {160, 128, 128}; // Sizes (x-dimensions) of rooms, for parallax scrolling across the background (use sizes larger than the screen size)
const int roomNameWidth = 20; // Width of the room name
const String roomNames[nRooms] = {"Gund 123", "Gund Trays", "Garden"}; // Names of the rooms
const unsigned char* roomBackgrounds[nRooms] = {NULL, NULL, NULL}; // Define backgrounds for each room (drawn behind player), because these are actually pointers at this point
const unsigned char* roomForegrounds[nRooms] = {NULL, NULL, NULL}; // Define foregrounds for each room (drawn on top of player), because these are actually pointers at this point

const int roomWidth = SCREEN_WIDTH; // Width of a room frame
const int roomHeight = SCREEN_HEIGHT; // Heigth of a room frame
const int nameWidth = 20; // Width of the name
const int avatarNameSpacing = 5; // Height of the name

const int nAvatars = 3; // Number of avatars possible in the game
const int playerAvatarYOffset = 16; // Offset in frame from y axis of own player
const int othersAvatarYOffset = 28; // Offset in frame from y axis of other players
const int avatarWidth = 24; // Width of an avatar frame
const int avatarHeigth = 24; // Height of an avatar frame

const int animationSpeed = 50; // Screen is refreshed every x milliseconds (implemented as a delay at the end of the main loop)
const float frameSpeed = 0.1; // Relative frame speed (sub-stepping of animation speed), so every loop, the frame counter is increased by x. This makes it possible to advance the frame counter slowly while allowing for faster animations (like walking or not blocking the menu)
const float walkingSpeed = 8; // Relative walking speed while holding down button. NOTE: walkingSpeed*parallaxFactor MUST be a full (int) number
const int nFrames = 2; // Number of frames an animation consists of
const int nAnimations = 5; // Number of animations implemented in current version
const int nMaxRepetitions = 5; // How many times non-repetitive animations are being repeated
const unsigned char* defaultAnimation[nAvatars][nFrames] = {{avt0_default0, avt0_default1},{avt1_default0, avt1_default1},{avt2_default0, avt2_default1}}; // Frames of working animation of all avatars
const unsigned char* workingAnimation[nAvatars][nFrames] = {{avt0_work0, avt0_work1},{avt1_work0, avt1_work1},{avt2_work0, avt2_work1}}; // Frames of working animation of all avatars
const unsigned char* musicAnimation[nAvatars][nFrames] = {{avt0_music0, avt0_music1},{avt1_music0, avt1_music1},{avt2_music0, avt2_music1}}; // Frames of giving flowers animation of all avatars
const unsigned char* eatingAnimation[nAvatars][nFrames] = {{avt0_eating0, avt0_eating1},{avt1_eating0, avt1_eating1},{avt2_eating0, avt2_eating1}}; // Frames of giving flowers animation of all avatars
const unsigned char* trumpetAnimation[nAvatars][nFrames] = {{avt0_trumpet0, avt0_trumpet1},{avt1_trumpet0, avt1_trumpet1},{avt2_trumpet0, avt2_trumpet1}}; // Frames of giving flowers animation of all avatars
const unsigned char* (*allAnimations[nAnimations])[nAvatars][nFrames] = {&defaultAnimation, &workingAnimation, &musicAnimation, &eatingAnimation, &trumpetAnimation}; // Animation look up array
const bool repeatTypeAnimations[nAnimations] = {true, true, true, true, true}; // Whether the type of animation is a repeat one or execute once

const int nMaxOthers = 4; // Maximum number of other players

const int nActionMenuItemLabels = 6; // Total number of menu items in the action menu
const String actionMenuItemLabels[nActionMenuItemLabels] = {"Walk", "Work", "Music", "Eat", "Trumpet", "Cancel"}; // String labels for each action item
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
//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_SH1106 display(OLED_RESET);
int imgSelector = 0; /// TODO: To be removed
bool currentPause = false; // Whether the player is currently in the start screen or in an active session
bool webSocketConnected = false; // Whether the WebSocket is currently connected

float currentFrame = 0.0; // Counts what animation frame is currently presented, increases after every frame
int currentPlayerAnimationRepeatCounter = 0; // Counts how many times the current animation has already been repeated for this player
int currentOthersAnimationRepeatCounter[nMaxOthers] = {0, 0, 0, 0}; // Counts how many times the current animation has already been repeated for other players

int currentPlayerLoc[nAxes] = {0, (int)(avatarWidth/2)}; // Which room the player is currently in and what his real and rendered location is. Possible values are 0 (left office), 1 (right office), 2 (garden)
int currentOthersLocs[nMaxOthers][nAxes] = {{0, 150}, {0, 60}, {0, 90}, {1, 20}};

long currentPlayerUID = -1; // Unique user ID, generated randomly or assigned by server
long currentOthersUIDs[nMaxOthers] = {100, 200, 300, 0}; // Unique user ids of other players joined

int currentPlayerAvatar = 1; // Which avatar the player has currently selected
int currentOthersAvatars[nMaxOthers] = {2, 0, 0, 1}; // Which avatars the other players have selected

String currentPlayerName = "Cedi"; // Name initials of the current player
String currentOthersNames[nMaxOthers] = {"Dishi", "Ana", "Aqdas", "Jon"}; // Name initials of the current player

int currentBackgroundLoc = 0; // Current location of the background the player is currently in (for rendering)
int currentPlayerAnimation = 0; // Which animation the player is currently executing
int currentPlayerPreviousAnimation = 0; // Which looped animation was executing before

int currentOthersAnimation[nMaxOthers] = {2, 1, 1, 0}; // Which animation the other players are executing at the moment
int currentOthersPreviousAnimation[nMaxOthers] = {0, 0, 0, 0}; // Which non-loop animation the other players are executing before

bool currentActionMenuOpen = false; // Whether or not the menu is currently open or not (has effects on LEFT and RIGHT button functionality)
int currentActionMenuItemLabelSelected = 0; // Which option in the menu is selected from default

/**
 * Enum for different event types
*/
enum PlayerEvent 
{
  LOGIN = 1,
  LOGIN_REPLY = 2,
  LOGOUT = 3,
  LOCCHANGE = 4,
  ANIMCHANGE = 5
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

    // When player sends own login information for the first time to WebSocket server
    case LOGIN:
    case LOGIN_REPLY:
    { 

      // Adjust event type based on initial login or reply to another player's login
      if(playerEvent == LOGIN_REPLY) {
        doc[jsonEvent] = LOGIN_REPLY;
      }
      else {
        doc[jsonEvent] = LOGIN;
      }

      doc[jsonName] = currentPlayerName;
      doc[jsonAvatar] = currentPlayerAvatar;

      JsonArray jsonLocArray = doc.createNestedArray(jsonLoc);
      jsonLocArray.add(currentPlayerLoc[0]);
      jsonLocArray.add(currentPlayerLoc[1]);

      doc[jsonAnim] = currentPlayerAnimation;
      break;
    }

    // When player logs out, just send a removal event
    case LOGOUT:
    {
      doc[jsonEvent] = LOGOUT;
      break;
    }

    // When player changes location
    case LOCCHANGE:
    {
      doc[jsonEvent] = LOCCHANGE;

      JsonArray jsonLocArray = doc.createNestedArray(jsonLoc);
      jsonLocArray.add(currentPlayerLoc[0]);
      jsonLocArray.add(currentPlayerLoc[1]);
      break;
    }

    // When player changes animation action
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
  Serial.print("[WebSocket] SENT: ");
  serializeJson(doc, Serial);

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
    printf("[JsonReceiver] New player recognized with UID %lo\n", playerUID);
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
    case LOGIN_REPLY:
    {
      // If no playerId assigned, the player hasn't actually joined / was in the waiting room
      if(playerId != -1) {
        printf("[JsonReceiver] Player already known with UID %lo\n", playerUID);
        return;
      }

      playerId = assignPlayerId(playerUID);

      // If still no playerId assigned, there are too many players
      if(playerId == -1) {
        printf("[JsonReceiver] No more space for player with UID %lo\n", playerUID);
        return;
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

      // If the event was a first login, send back own login information for new player to process
      // Basically, generate an artificial login event for the new player
      if(playerEvent == LOGIN) {

        StaticJsonDocument<1000> doc = generateJson(LOGIN_REPLY);
        sendJson(doc);

      }

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
  srand(time(NULL)); // Set seed
  currentPlayerUID = rand() % MAX_LONG; // Get random number
  printf("[Setup] Generated player UID %lo\n", currentPlayerUID);

  /**
   * Network setup section
  */
  // Print MAC address
  Serial.print("[Wifi] Device MAC address is: ");
  Serial.println(WiFi.macAddress());

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
  webSocketClient.setReconnectInterval(1000);

  // Add event listener to WebSocket connection
  webSocketClient.onEvent(onWebSocketEvent);

  /**
   * Peripherals setup section
  */
  // Try to initialize screen object under given screen address, block if unsuccessful
  /*if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {

    Serial.println("[Setup] Error: Display could not be found");
    for(;;);

  }*/
  display.begin(SH1106_SWITCHCAPVCC, SCREEN_ADDRESS);

  // Show image buffer after initialization
  display.display();
  delay(2000);

  // Clear the display and buffer
  display.clearDisplay();

  // Set encoder pins as input pins
  pinMode(VRX, INPUT);
  pinMode(VRY, INPUT);
  pinMode(SW, INPUT_PULLUP);

  pinMode(LEFT, INPUT_PULLUP);
  pinMode(RIGHT, INPUT_PULLUP);
  pinMode(MIDDLE, INPUT_PULLUP);
  
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

    bool buttonPressed = false;

    if(joystickActive) {

      // Read the y position of the joystick
      int yValue = analogRead(VRY);
      int btnState = digitalRead(SW);

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

    }

    // Read button states
    if(digitalRead(RIGHT) == LOW) {
      rightButtonPressed = LOW;
      buttonPressed = true;
    }

    else if(digitalRead(LEFT) == LOW) {
      leftButtonPressed = LOW;
      buttonPressed = true;
    }

    else if(digitalRead(MIDDLE) == LOW) {
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

  String roomName = roomNames[currentRoom];
  display.setCursor(-currentCameraLoc+(int)(roomWidth/2)+(int)(-roomNameWidth/2), 0);
  display.setTextSize(1);
  display.setTextWrap(false);
  display.setTextColor(WHITE, BLACK);
  display.println(roomName);

  display.drawRect(currentBackgroundLoc, 12, roomSizes[currentRoom], roomHeight-12, WHITE);
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
    display.drawBitmap(otherAvatarRenderX, othersAvatarYOffset+avatarNameSpacing, (*allAnimations[otherAvatarAnimation])[otherAvatar][(int)currentFrame], avatarWidth, avatarHeigth, BITMAP_COLOR);

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
  display.drawBitmap(playerAvatarRenderX, playerAvatarYOffset+avatarNameSpacing, (*allAnimations[currentPlayerAnimation])[currentPlayerAvatar][(int)currentFrame], avatarWidth, avatarHeigth, BITMAP_COLOR);

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
      Serial.println("Repeating");
      Serial.println(currentPlayerAnimationRepeatCounter);
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
  //display.fillRect(-currentCameraLoc+60, 44, 20, 20, WHITE);
  //display.fillRect(-currentCameraLoc+150, 44, 20, 20, WHITE);
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