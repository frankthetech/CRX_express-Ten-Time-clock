// =================================================
// CRX Ten-Time Clock
// Adafruit Circuit Playground
//
// Decimal / Ten-Time display:
//
// Red    = decimal hour
// Green  = decimal minute
// Blue   = decimal second
//
// Dim white:
//     fast hundredths indicator
//
// Bright white:
//     advances one position each time
//     the fast white indicator completes a lap
//
// Decimal day:
//
// 10 hours per day
// 10 minutes per hour
// 10 seconds per minute
// 100 hundredths per second
//
// Total:
// 100,000 decimal hundredths per day
//
// -------------------------------------------------
// Normal time to starting decimal hundredths:
//
// ((hour * 3600) +
//  (minute * 60) +
//  second) * 1000 / 864
//
// Example:
//
// 11:36:00
//
// ((11 * 3600) +
//  (36 * 60)) * 1000 / 864
//
// = 48333
//
// =================================================


#include <Adafruit_CircuitPlayground.h>


// -------------------------------------------------
// LED colors
// -------------------------------------------------

#define HOUR_RED            50
#define MINUTE_GREEN        50
#define SECOND_BLUE         50

#define ROTATE_LEVEL        20
#define SLOW_ROTATE_LEVEL   35


// -------------------------------------------------
// Global NeoPixel brightness
//
// Range:
// 0   = off
// 255 = maximum
// -------------------------------------------------

#define LED_BRIGHTNESS      50


// -------------------------------------------------
// One normal 24-hour day in milliseconds
// -------------------------------------------------

#define REAL_DAY_MS         86400000ULL


// -------------------------------------------------
// Starting decimal time
//
// Convert normal 24-hour time using:
//
// ((hour * 3600) +
//  (minute * 60) +
//  second) * 1000 / 864
//
// Example:
// 11:36:00 = 48333
// -------------------------------------------------

uint64_t START_DECIMAL_HUNDREDTHS =
    48333;


// -------------------------------------------------
// LED mapping
//
// Circuit Playground is mounted upside down,
// with USB at the 6 o'clock position.
//
// Logical decimal positions 0 through 9
// are mapped to physical NeoPixels here.
// -------------------------------------------------

const uint8_t clockwiseLed[10] =
{
    4, 3, 2, 1, 0, 9, 8, 7, 6, 5
};


// -------------------------------------------------
// Convert compiler time to decimal time
//
// __TIME__ format:
//
// HH:MM:SS
//
// This may be used instead of manually setting
// START_DECIMAL_HUNDREDTHS.
// -------------------------------------------------

uint64_t getCompileDecimalTime()
{
    const char *compileTime =
        __TIME__;


    uint8_t hours =
        (compileTime[0] - '0') * 10 +
        (compileTime[1] - '0');


    uint8_t minutes =
        (compileTime[3] - '0') * 10 +
        (compileTime[4] - '0');


    uint8_t seconds =
        (compileTime[6] - '0') * 10 +
        (compileTime[7] - '0');


    uint32_t realSeconds =
        (hours * 3600UL) +
        (minutes * 60UL) +
        seconds;


    return
        ((uint64_t)realSeconds *
        100000ULL) /
        86400ULL;
}


// -------------------------------------------------
// Setup
// -------------------------------------------------

void setup()
{
    CircuitPlayground.begin();


    CircuitPlayground.strip.setBrightness(
        LED_BRIGHTNESS
    );


    // -------------------------------------------------
    // Optional:
    //
    // Start clock using computer compile time.
    //
    // Uncomment this line to use it.
    // -------------------------------------------------

    // START_DECIMAL_HUNDREDTHS =
    //     getCompileDecimalTime();
}


// -------------------------------------------------
// Main loop
// -------------------------------------------------

void loop()
{
    uint32_t currentMillis =
        millis();


    // -------------------------------------------------
    // Convert elapsed normal milliseconds into
    // elapsed decimal hundredths.
    // -------------------------------------------------

    uint64_t elapsedHundredths =
        ((uint64_t)currentMillis *
        100000ULL) /
        REAL_DAY_MS;


    // -------------------------------------------------
    // Add elapsed time to starting time
    // -------------------------------------------------

    uint64_t decimalHundredths =
        START_DECIMAL_HUNDREDTHS +
        elapsedHundredths;


    // -------------------------------------------------
    // Keep clock within one decimal day
    // -------------------------------------------------

    decimalHundredths %=
        100000ULL;


    // -------------------------------------------------
    // Get total decimal seconds
    // -------------------------------------------------

    uint16_t decimalSeconds =
        decimalHundredths /
        100;


    // -------------------------------------------------
    // Split into decimal time
    //
    // H : M : S
    // -------------------------------------------------

    uint8_t hours =
        decimalSeconds /
        100;


    uint8_t minutes =
        (decimalSeconds /
        10) %
        10;


    uint8_t seconds =
        decimalSeconds %
        10;


    // -------------------------------------------------
    // Get hundredths within current decimal second
    // -------------------------------------------------

    uint8_t hundredths =
        decimalHundredths %
        100;


    // -------------------------------------------------
    // Fast white indicator
    //
    // Cycles:
    //
    // 0 1 2 3 4 5 6 7 8 9
    //
    // ten times per decimal second.
    // -------------------------------------------------

    uint8_t rotateLed =
        hundredths %
        10;


    // -------------------------------------------------
    // Slow white indicator
    //
    // Advances one position every time
    // the fast white indicator completes a lap.
    // -------------------------------------------------

    uint8_t slowRotateLed =
        hundredths /
        10;


    // -------------------------------------------------
    // Display clock
    // -------------------------------------------------

    showTime(
        hours,
        minutes,
        seconds,
        rotateLed,
        slowRotateLed
    );
}


// -------------------------------------------------
// Display decimal time
// -------------------------------------------------

void showTime(
    uint8_t hourLed,
    uint8_t minuteLed,
    uint8_t secondLed,
    uint8_t rotateLed,
    uint8_t slowRotateLed)
{
    uint8_t red[10] =
        {0};

    uint8_t green[10] =
        {0};

    uint8_t blue[10] =
        {0};


    // -------------------------------------------------
    // Convert logical decimal positions
    // into physical NeoPixel positions
    // -------------------------------------------------

    uint8_t hourDisplay =
        clockwiseLed[hourLed];


    uint8_t minuteDisplay =
        clockwiseLed[minuteLed];


    uint8_t secondDisplay =
        clockwiseLed[secondLed];


    uint8_t rotateDisplay =
        clockwiseLed[rotateLed];


    uint8_t slowRotateDisplay =
        clockwiseLed[slowRotateLed];


    // -------------------------------------------------
    // Decimal hour
    // -------------------------------------------------

    red[hourDisplay] =
        HOUR_RED;


    // -------------------------------------------------
    // Decimal minute
    // -------------------------------------------------

    green[minuteDisplay] =
        MINUTE_GREEN;


    // -------------------------------------------------
    // Decimal second
    // -------------------------------------------------

    blue[secondDisplay] =
        SECOND_BLUE;


    // -------------------------------------------------
    // Slow white indicator
    //
    // Add RGB values so it can share an LED
    // with hour, minute, second, or fast white.
    // -------------------------------------------------

    red[slowRotateDisplay] +=
        SLOW_ROTATE_LEVEL;

    green[slowRotateDisplay] +=
        SLOW_ROTATE_LEVEL;

    blue[slowRotateDisplay] +=
        SLOW_ROTATE_LEVEL;


    // -------------------------------------------------
    // Fast white hundredths indicator
    // -------------------------------------------------

    red[rotateDisplay] +=
        ROTATE_LEVEL;

    green[rotateDisplay] +=
        ROTATE_LEVEL;

    blue[rotateDisplay] +=
        ROTATE_LEVEL;


    // -------------------------------------------------
    // Update NeoPixels
    // -------------------------------------------------

    for (
        uint8_t i = 0;
        i < 10;
        i++)
    {
        CircuitPlayground.setPixelColor(
            i,
            red[i],
            green[i],
            blue[i]
        );
    }
}
