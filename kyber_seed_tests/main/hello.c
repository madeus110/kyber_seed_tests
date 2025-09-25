// main/hello_gather.c
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_timer.h"
#include "driver/gpio.h"
#include "esp_random.h"
#include "fips202.h"  // shake256

#define TRIGGER_GPIO    2
#define RUNS            10000      // change if you want fewer/more
#define DELAY_MS        20         // delay between runs (ms)
#define START_DELAY_MS  5000       // time to start after reset (ms)

static inline void trigger_init(void){
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << TRIGGER_GPIO,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
    gpio_set_level(TRIGGER_GPIO, 0);
}
static inline void trigger_high(void){ gpio_set_level(TRIGGER_GPIO, 1); }
static inline void trigger_low(void){ gpio_set_level(TRIGGER_GPIO, 0); }

static void print_hex_raw(const uint8_t *buf, size_t len){
    for(size_t i=0;i<len;i++) printf("%02X", buf[i]);
}

static void gen_seed(uint8_t *seed, size_t len){
    uint8_t entropy[32];
    for (int i=0;i<32;i+=4){
        uint32_t r = esp_random();
        memcpy(entropy + i, &r, 4);
    }
    keccak_state st;
    shake256_init(&st);
    shake256_absorb(&st, entropy, sizeof(entropy));
    shake256_finalize(&st);
    shake256_squeeze(seed, len, &st);
}

void app_main(void){
    trigger_init();
    vTaskDelay(pdMS_TO_TICKS(100)); // small settle

    printf("BEGIN_RUNS,mode=RANDOM,runs=%d\n", RUNS);
    printf("START_DELAY_MS=%d\n", START_DELAY_MS);
    fflush(stdout);

    // Give you time to start PC capture and arm scope
    vTaskDelay(pdMS_TO_TICKS(START_DELAY_MS));

    uint8_t seed[32];

    for (uint32_t i = 0; i < RUNS; i++){
        int64_t t_start = esp_timer_get_time();
        trigger_high();
        gen_seed(seed, sizeof(seed));
        trigger_low();
        int64_t t_gen_us = esp_timer_get_time() - t_start;

        // CSV: idx,start_us,gen_us,seedhex
        printf("%05" PRIu32 ",%" PRId64 ",%" PRId64 ",", i, t_start, t_gen_us);

        print_hex_raw(seed, sizeof(seed));
        printf("\n");
        fflush(stdout);

        vTaskDelay(pdMS_TO_TICKS(DELAY_MS));
    }

    printf("END_RUNS\n");
    fflush(stdout);

    while(1) vTaskDelay(pdMS_TO_TICKS(1000)); // keep alive for scope
}

