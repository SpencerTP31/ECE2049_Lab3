/**************** ECE2049 Lab 2 ******************/
/***************  13 March 2019 ******************/
/******  Benjamin Ward, Jonathan Ferreira ********/
/*************************************************/

#include <msp430.h>
#include <stdlib.h>
#include <stdio.h>
#include <inc/song.h>
#include <inc/peripherals.h>
#include <inc/ADC.h>
#include <main.h>
#include <inc/time.h>

// States utilized in main program state machine
typedef enum State
{
    DISPLAY, WAIT_FOR_TRANSITION, EDIT
} State;

typedef enum DisplayState
{
    DATE, TIME, TEMP_C, TEMP_F
} DisplayState;

// Defined in main.h
uint32_t globalClock = monthToSeconds(0) + monthToSeconds(1) + monthToSeconds(2)
        + monthToSeconds(3) + monthToSeconds(4) + monthToSeconds(5)
        + dayToSeconds(4);

uint32_t globalCounter = 0;

void drawDate();
void drawTime();
void drawTempC(ADC*);
void drawTempF(ADC*);

// Program entry point
void main(void)
{
    // Main loop state
    uint32_t startCounter, deltaCounter;
    State state = DISPLAY;
    DisplayState displayState = DATE;

    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

    // Set up and configure peripherals and I/O
    initLeds();
//    configSmolLEDs();
    configDisplay();
//    configKeypad();
//    configButtons();

    ADC* adc = new ADC();

//    deltaTime = globalCounter - startCounter;

    // Main loop
    while (1)
    {
        switch (state)
        {
        case DISPLAY:
            startCounter = globalCounter;
            switch (displayState)
            {
            case DATE:
                drawDate();
                break;
            case TIME:
                drawTime();
                break;
            case TEMP_C:
                drawTempC(adc);
                break;
            case TEMP_F:
                drawTempF(adc);
                break;
            }
            state = WAIT_FOR_TRANSITION;
            break;
        case WAIT_FOR_TRANSITION:
            deltaCounter = globalCounter - startCounter;
            if(deltaCounter >= 2) {
                switch (displayState)
                {
                case DATE:      displayState = TIME;    break;
                case TIME:      displayState = TEMP_C;  break;
                case TEMP_C:    displayState = TEMP_F;  break;
                case TEMP_F:    displayState = DATE;    break;
                }
                state = DISPLAY;
            }
            break;
        case EDIT:
            break;
        }
    }
}

void drawDate() {
    Graphics_clearDisplay(&g_sContext);

    Date date = currentDate();
    const char* month = monthNames[date.month];

    char dBuffer[6];
    dBuffer[0] = month[0];
    dBuffer[1] = month[1];
    dBuffer[2] = month[2];
    dBuffer[3] = ' ';
    dBuffer[4] = date.day / 10 % 10 + '0';
    dBuffer[5] = date.day % 10 + '0';

//    snprintf(dBuffer, 9, "%s %d", monthNames[date.month], date.day);

    Graphics_drawStringCentered(&g_sContext, (uint8_t*)dBuffer, 6, 48, 25, TRANSPARENT_TEXT);

    Graphics_Rectangle box = { .xMin = 5, .xMax = 91, .yMin = 5, .yMax = 91 };
    Graphics_drawRectangle(&g_sContext, &box);
    Graphics_flushBuffer(&g_sContext);
}

void drawTime()
{
    Graphics_clearDisplay(&g_sContext);

    Time time = currentTime();

    char dBuffer[8];
    dBuffer[0] = time.hours / 10 % 10 + '0';
    dBuffer[1] = time.hours % 10 + '0';
    dBuffer[2] = ':';
    dBuffer[3] = time.minutes / 10 % 10 + '0';
    dBuffer[4] = time.minutes % 10 + '0';
    dBuffer[5] = ':';
    dBuffer[6] = time.seconds / 10 % 10 + '0';
    dBuffer[7] = time.seconds % 10 + '0';

    Graphics_drawStringCentered(&g_sContext, (uint8_t*)dBuffer, 8, 48, 25, TRANSPARENT_TEXT);

    Graphics_Rectangle box = { .xMin = 5, .xMax = 91, .yMin = 5, .yMax = 91 };
    Graphics_drawRectangle(&g_sContext, &box);
    Graphics_flushBuffer(&g_sContext);
}


void drawTempC(ADC* adc) {
    Graphics_clearDisplay(&g_sContext);

    float tempC = adc->getCurrentTempC();

    char dBuffer[6];
    dBuffer[0] = (uint8_t)tempC / 10 % 10 + '0';
    dBuffer[1] = (uint8_t)tempC % 10 + '0';
    dBuffer[2] = '.';
    dBuffer[3] = (uint8_t)(tempC * 10) % 10 + '0';
    dBuffer[4] = ' ';
    dBuffer[5] = 'C';

    Graphics_drawStringCentered(&g_sContext, (uint8_t*)dBuffer, 6, 48, 25, TRANSPARENT_TEXT);

    Graphics_Rectangle box = { .xMin = 5, .xMax = 91, .yMin = 5, .yMax = 91 };
    Graphics_drawRectangle(&g_sContext, &box);
    Graphics_flushBuffer(&g_sContext);
}

void drawTempF(ADC* adc) {
    Graphics_clearDisplay(&g_sContext);

    float tempF = adc->getCurrentTempF();

    char dBuffer[6];
    dBuffer[0] = (uint8_t)tempF / 10 % 10 + '0';
    dBuffer[1] = (uint8_t)tempF % 10 + '0';
    dBuffer[2] = '.';
    dBuffer[3] = (uint8_t)(tempF * 10) % 10 + '0';
    dBuffer[4] = ' ';
    dBuffer[5] = 'F';

    Graphics_drawStringCentered(&g_sContext, (uint8_t*)dBuffer, 6, 48, 25, TRANSPARENT_TEXT);

    Graphics_Rectangle box = { .xMin = 5, .xMax = 91, .yMin = 5, .yMax = 91 };
    Graphics_drawRectangle(&g_sContext, &box);
    Graphics_flushBuffer(&g_sContext);
}

void drawWelcome()
{
// *** Intro Screen ***
                    // Clear the display

// Write some text to the display
    Graphics_drawStringCentered(&g_sContext, (uint8_t*) "MSP430",
    AUTO_STRING_LENGTH,
                                48, 25, TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, (uint8_t*) "HERO",
    AUTO_STRING_LENGTH,
                                48, 35,
                                TRANSPARENT_TEXT);

    Graphics_drawStringCentered(&g_sContext, (uint8_t*) "Press *",
    AUTO_STRING_LENGTH,
                                48, 70, TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, (uint8_t*) "to Start",
    AUTO_STRING_LENGTH,
                                48, 80, TRANSPARENT_TEXT);

// Draw a box around everything because it looks nice


// We are now done writing to the display.  However, if we stopped here, we would not
// see any changes on the actual LCD.  This is because we need to send our changes
// to the LCD, which then refreshes the display.
// Since this is a slow operation, it is best to refresh (or "flush") only after
// we are done drawing everything we need.

}

void drawLoss()
{
// *** Intro Screen ***
    Graphics_clearDisplay(&g_sContext);    // Clear the display

// Write some text to the display
    Graphics_drawStringCentered(&g_sContext, (uint8_t*) "You Lost.",
    AUTO_STRING_LENGTH,
                                48, 25, TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, (uint8_t*) ">:(",
    AUTO_STRING_LENGTH,
                                48, 35,
                                TRANSPARENT_TEXT);

    Graphics_drawStringCentered(&g_sContext, (uint8_t*) "Press *",
    AUTO_STRING_LENGTH,
                                48, 70, TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, (uint8_t*) "to Try Again",
    AUTO_STRING_LENGTH,
                                48, 80, TRANSPARENT_TEXT);

// Draw a box around everything because it looks nice
    Graphics_Rectangle box = { .xMin = 5, .xMax = 91, .yMin = 5, .yMax = 91 };
    Graphics_drawRectangle(&g_sContext, &box);

    Graphics_flushBuffer(&g_sContext);
}

void drawWin()
{
// *** Intro Screen ***
    Graphics_clearDisplay(&g_sContext);    // Clear the display

// Write some text to the display
    Graphics_drawStringCentered(&g_sContext, (uint8_t*) "You Win!",
    AUTO_STRING_LENGTH,
                                48, 25, TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, (uint8_t*) ":D",
    AUTO_STRING_LENGTH,
                                48, 35,
                                TRANSPARENT_TEXT);

    Graphics_drawStringCentered(&g_sContext, (uint8_t*) "Press *",
    AUTO_STRING_LENGTH,
                                48, 70, TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, (uint8_t*) "to Try Again",
    AUTO_STRING_LENGTH,
                                48, 80, TRANSPARENT_TEXT);

// Draw a box around everything because it looks nice
    Graphics_Rectangle box = { .xMin = 5, .xMax = 91, .yMin = 5, .yMax = 91 };
    Graphics_drawRectangle(&g_sContext, &box);

    Graphics_flushBuffer(&g_sContext);
}

void drawNextLevel(int level)
{

    Graphics_clearDisplay(&g_sContext);

    char buffer[10];
    snprintf(buffer, 9, "Level %d", level);

// Write some text to the display
    Graphics_drawStringCentered(&g_sContext, (uint8_t*) buffer,
    AUTO_STRING_LENGTH,
                                48, 35,
                                TRANSPARENT_TEXT);

    Graphics_drawStringCentered(&g_sContext, (uint8_t*) "Press *",
    AUTO_STRING_LENGTH,
                                48, 70, TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, (uint8_t*) "to Begin",
    AUTO_STRING_LENGTH,
                                48, 80, TRANSPARENT_TEXT);

// Draw a box around everything because it looks nice
    Graphics_Rectangle box = { .xMin = 5, .xMax = 91, .yMin = 5, .yMax = 91 };
    Graphics_drawRectangle(&g_sContext, &box);

    Graphics_flushBuffer(&g_sContext);
}
