#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include <sdkconfig.h>
#include <stdbool.h>


#define greenLED_PIN    5         
#define redLED_PIN      6      
#define ignitionButton  7
#define driveSeatBelt   15
#define passengerSeatBelt   16
#define driveSeatSense  1
#define passengerSeatSense 2
#define Alarm 37

bool dSense = false;
bool dsbelt = false;
bool pSense = false;
bool psbelt = false;


/**
 * returns a boolean determining whether all of the car alarms systems have been satisifed ie:
 * driver seat belt, driver is seated etc.
 */
bool enable(void){

    bool dslvl = gpio_get_level(driveSeatSense);
    bool dsbeltlvl = gpio_get_level(driveSeatBelt);
    bool pslvl = gpio_get_level(passengerSeatSense);
    bool psbltlvl = gpio_get_level(passengerSeatBelt);

    if (!dslvl){
        dSense = true; //Driver sensor
    }
    else{dSense = false;}

    if (!dsbeltlvl){
        dsbelt = true; // driver seatbelt sensor
    }
    else{dsbelt = false;}

    if (!pslvl){
        pSense = true; // passenger seat level
    }
    else{pSense = false;}

    if (!psbltlvl){
        psbelt = true; // passenger seatbelt level
    }
    else{psbelt = false;}

    bool IgnitReady = dSense && dsbelt && pSense && psbelt;
    return IgnitReady;
    }
    


void app_main(void) {

    gpio_reset_pin(greenLED_PIN);
    gpio_reset_pin(redLED_PIN);
    gpio_reset_pin(ignitionButton);
    gpio_reset_pin(driveSeatBelt);
    gpio_reset_pin(passengerSeatBelt);
    gpio_reset_pin(driveSeatSense);
    gpio_reset_pin(passengerSeatSense);
    gpio_reset_pin(Alarm);

    gpio_set_direction(greenLED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(redLED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(Alarm, GPIO_MODE_OUTPUT);
    gpio_set_direction(ignitionButton, GPIO_MODE_INPUT);
    gpio_set_direction(driveSeatBelt, GPIO_MODE_INPUT);
    gpio_set_direction(passengerSeatBelt, GPIO_MODE_INPUT);
    gpio_set_direction(driveSeatSense, GPIO_MODE_INPUT);
    gpio_set_direction(passengerSeatSense, GPIO_MODE_INPUT);

    gpio_pullup_en(ignitionButton);
    gpio_pullup_en(driveSeatBelt);
    gpio_pullup_en(driveSeatSense);
    gpio_pullup_en(passengerSeatBelt);
    gpio_pullup_en(passengerSeatSense);

    gpio_set_level (greenLED_PIN, 0);
    gpio_set_level (redLED_PIN, 0);
    gpio_set_level (Alarm, 0);

    bool initial_message = true;

    while(1){
        bool ignitEn = !gpio_get_level(ignitionButton);
        bool ready = enable();

        if (dSense && initial_message){
            printf("Welcome to enhanced Alarm system model 218 -W25")
            initial_message = false;

        }

        if(ready){
            gpio_set_level(greenLED_PIN, 1);
        }
        else {
            gpio_set_level(greenLED_PIN, 0);
        }

        if(ignitEn){
            // If the ignition Button is pressed...
            if (ready) {
                //Start the engine if the green LED is on
                printf("engine starting...");
                gpio_set_level(greenLED_PIN, 0);
                gpio_set_level(redLED_PIN, 1);
                return;
            }

            else {
                //Start the alarm and display the Error message otherwise.
                gpio_set_level (Alarm, 1);

                if (!dSense){
                    printf("Driver seat not occupied\n");
                }
                
                if (!dsbelt){
                    printf("Driver seatbelt not fastened\n");
                }

                if (!pSense){
                    printf("Passenger seat nmot occupied\n");
                }
                if (!psbelt){
                    printf("Passenger seatbelt not fastened\n");
                }
                vTaskDelay (3000/ portTICK_PERIOD_MS);
            }
        }
        else {
            gpio_set_level (Alarm, 0);
        }

        //Debounce time for the check
        vTaskDelay(25 / portTICK_PERIOD_MS);

    }
}