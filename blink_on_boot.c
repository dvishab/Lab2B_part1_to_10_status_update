#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"
#include "C:\Users\12153\pico\pico-sdk\src\rp2_common\hardware_gpio\include\hardware\gpio.h"
#include "C:\Users\12153\pico\pico-sdk\src\boards\include\boards\adafruit_qtpy_rp2040.h"
#include "registers.h"

#define gpio_out_pinn 23
#define gpio_out_pinn2 22
//#define QTPY_BOOT_PIN 21

#define IS_RGBW true
#define NUM_PIXELS 150
#define PICO_DEFAULT_WS2812_POWER_PIN 11

#define QTPY_BOOT_PIN_NUM 21

#ifdef PICO_DEFAULT_WS2812_PIN
#define WS2812_PIN PICO_DEFAULT_WS2812_PIN
#else
// default to pin 2 if the board doesn't have a default WS2812 pin defined
#define WS2812_PIN 2
#endif

void set_neopixel_color(uint32_t color_num){
    //
    uint32_t r_num = color_num >> 16;
    uint32_t g_num = color_num << 16;
    g_num = g_num >> 24;
    uint32_t b_num = color_num << 24;
    b_num = b_num >> 24;
    uint32_t color_final = ((r_num << 8) |
                            (g_num << 16)|
                            (b_num));
    pio_sm_put_blocking(pio0, 0, color_final << 8u);
}


int main() {
    //set_sys_clock_48();
    // Initialize the GPIO Pins
    const uint POWER_PIN_NAME = PICO_DEFAULT_WS2812_POWER_PIN;
    gpio_init(POWER_PIN_NAME);
    gpio_init(QTPY_BOOT_PIN_NUM);
    gpio_set_dir(POWER_PIN_NAME, GPIO_OUT);
    gpio_set_dir(QTPY_BOOT_PIN_NUM, GPIO_IN);
    gpio_put(POWER_PIN_NAME, 1);
    stdio_init_all();

    // todo get free sm
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);
    
    volatile uint32_t QTPY_BOOT_PIN_REG = IO_BANK0_BASE + 0x0A8;
    /* volatile uint32_t QTPY_BOOT_PIN_REG_PIN_NAME = */ 
    int getcount[5] = {0};
    int i = 0;
    while(1){
        /* int user_inp = getchar_timeout_us(5000); */
        
        
        uint32_t button_status = register_read(QTPY_BOOT_PIN_REG);

        printf("0x%08x\n", button_status);
        
        int counter = 0;
        int num_boot_press = 0;
        while(num_boot_press < 5){
        uint32_t button_status = register_read(QTPY_BOOT_PIN_REG);
        if(button_status != 0x00000000){
        counter = counter + 1;

        // set_neopixel_color(0xffafaf);
        // sleep_ms(500);
        // set_neopixel_color(0);

        }
        else{
            
            getcount[i] = counter;
            sleep_ms(1000);
            counter = 0;
            num_boot_press = num_boot_press+1;
            i=i+1;
        }
        for (i=0; i<5; i++){
          set_neopixel_color(0xffafaf);
          sleep_ms(500);
          set_neopixel_color(0);
          sleep_ms(getcount[i]/5);
        }
        }



        /* if(user_inp != 1){ */
        /*     set_neopixel_color(0xffafaf); */
        /*     sleep_ms(500); */
        /*     set_neopixel_color(0); */
        /* } */
        sleep_ms(1000);
    }
}
