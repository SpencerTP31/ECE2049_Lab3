/**************** ECE2049 Lab 3 ******************/
/*************  25 September 2019 ****************/
/******  Benjamin Ward, Jonathan Ferreira ********/
/*************************************************/

#include <msp430.h>
#include <stdlib.h>
#include <stdio.h>
#include <inc/peripherals.h>
#include <inc/ADC.h>
#include <main.h>
#include <inc/time.h>

// States utilized in main program state machine
typedef enum State
{
    DISPLAY, WAIT_FOR_TRANSITION, EDIT, EDIT_VALUES
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

// Function prototypes
void drawDate();
void drawTime();
void drawTempC(ADC*);
void drawTempF(ADC*);
void configButtons();
unsigned char getButtonState();
void drawDateTime();
void drawEditScreen(ADC* adc, int pos);

// Program entry point
void main(void)
{
    // Main loop state
    uint32_t startCounter, deltaCounter;
    State state = DISPLAY;
    DisplayState displayState = DATE;
    unsigned char buttonPressed, keyPressed = 0;

    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

    // Set up and configure peripherals and I/O
    initLeds();
    configDisplay();
    configButtons();
    configKeypad();

    ADC* adc = new ADC();

//    deltaTime = globalCounter - startCounter;

    // Main loop
    while (1)
    {
        buttonPressed = getButtonState();
        keyPressed = getKey();

        if (buttonPressed & BIT3)
        {
            state = EDIT;
        }

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
            drawDateTime();
            state = EDIT_VALUES;
            break;
        case EDIT_VALUES:
//            drawEditScreen(adc);
            uint32_t currentPot = 0, deltaPot = 0;
            currentPot = adc->getCurrentPot();

            while(!buttonPressed) {
                deltaPot = currentPot - adc->getCurrentPot()
                if (deltaPot > 100 && date.month != 11) {
                    date.month += 1;
                    deltaPot = 0;
                }
                if (deltaPot < 100 && date.month != 0) {
                    date.month -= 1;
                    deltaPot = 0;
                }
            }

            state = EDIT_VALUES;
//            while (1) {
//                uint32_t deltaPot = currentPot - adc->getCurrentPot();
//            }
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

void drawDateTime() {
    Graphics_clearDisplay(&g_sContext);

    Date date = currentDate();
    Time time = currentTime();
    const char* month = monthNames[date.month];

    char dBuffer[6];
    dBuffer[0] = month[0];
    dBuffer[1] = month[1];
    dBuffer[2] = month[2];
    dBuffer[3] = ' ';
    dBuffer[4] = date.day / 10 % 10 + '0';
    dBuffer[5] = date.day % 10 + '0';

    char tBuffer[8];
    tBuffer[0] = time.hours / 10 % 10 + '0';
    tBuffer[1] = time.hours % 10 + '0';
    tBuffer[2] = ':';
    tBuffer[3] = time.minutes / 10 % 10 + '0';
    tBuffer[4] = time.minutes % 10 + '0';
    tBuffer[5] = ':';
    tBuffer[6] = time.seconds / 10 % 10 + '0';
    tBuffer[7] = time.seconds % 10 + '0';

    Graphics_drawStringCentered(&g_sContext, (uint8_t*)dBuffer, 6, 48, 25, TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, (uint8_t*)tBuffer, 8, 48, 55, TRANSPARENT_TEXT);

    // Draw selection line (EVENTUALLY MOVE TO EDIT SCREEN FUNCTION)
    Graphics_drawLineH(&g_sContext, 30, 46, 32);
    Graphics_drawLineH(&g_sContext, 52, 66, 32);

    Graphics_drawLineH(&g_sContext, 23, 35, 62);
    Graphics_drawLineH(&g_sContext, 43, 55, 62);

    Graphics_Rectangle box = { .xMin = 5, .xMax = 91, .yMin = 5, .yMax = 91 };
    Graphics_drawRectangle(&g_sContext, &box);
    Graphics_flushBuffer(&g_sContext);
}

void drawEditScreen(ADC* adc, int pos) {
//    int currentPot = adc->getCurrentPot();
//    Graphics_drawLineH(&g_sContext, 28, 48, 32);
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

// Configure lab board buttons
void configButtons()
{
    // P7.0, P3.6, P2.2, P7.4
    // Configure P2.2
    P2SEL &= ~(BIT2);    // Select pin for DI/O
    P2DIR &= ~(BIT2);    // Set pin as input
    P2REN |= (BIT2);    // Enable pull-up resistor
    P2OUT |= (BIT2);

    // Configure P3.6
    P3SEL &= ~(BIT6);    // Select pin for DI/O
    P3DIR &= ~(BIT6);    // Set pin as input
    P3REN |= (BIT6);    // Enable pull-up resistor
    P3OUT |= (BIT6);

    // Configure P7.0 and P7.4
    P7SEL &= ~(BIT4 | BIT0);    // Select pins for DI/O
    P7DIR &= ~(BIT4 | BIT0);    // Set pins as input
    P7REN |= (BIT4 | BIT0);    // Enable pull-up resistors
    P7OUT |= (BIT4 | BIT0);
}

// Get the state of the lab board buttons
unsigned char getButtonState()
{
    unsigned char ret = 0x00;
    // P2.2
    if (~P2IN & BIT2)
        ret |= BIT1; // Button 2
    // P3.6
    if (~P3IN & BIT6)
        ret |= BIT2;    // Button 1
    // P7.0
    if (~P7IN & BIT0)
        ret |= BIT3;    // Button 0
    // P7.4
    if (~P7IN & BIT4)
        ret |= BIT0;    // Button 3
    return ret;
}
