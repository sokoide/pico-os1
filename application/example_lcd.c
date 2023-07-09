#include "examples.h"
#include "usermain.h"
#include <kernel.h>
#include <ssd1306.h>
#include <stdio.h>
#include <time.h>

ssd1306_t disp;
UB buff[128 * 64 + 1];
bool lcd_initialized = false;

void clear_rect(ssd1306_t* p, uint32_t x, uint32_t y, uint32_t width,
                uint32_t height) {
    for (int yy = y; yy < y + height; yy++) {
        for (int xx = x; xx <= x + width; xx++) {
            ssd1306_clear_pixel(&disp, xx, yy);
        }
    }
}

void set_line(char* line, int i) {
    sprintf(line, "STK_BASE%02d:0x%08X\n", i, TASK_STACK_BASE_N(i));
}

void draw_loop(int n, int count) {
    char line[32];
    /* printf("draw_loop...\n"); */
    clear_rect(&disp, 0, 34, 128, 64);
    for (int i = 0; i < count; i++) {
        if (i + n < MAX_TASKS) {
            set_line(line, i + n);
            ssd1306_draw_string(&disp, 0, 34 + i * 10, 1, line);
        }
    }
    ssd1306_show(&disp);
    sk_delay_task(1000);
}

void task_lcd_func(INT stacd, void* exinf) {
    ERR err;
    err = sk_wait_semaphore(sid, 1, 100);
    if (err == E_TMOUT) {
        printf("LCD task is already running\n");
        goto END;
    }

    printf("Got semaphore, task_lcd_func started...\n");
    printf("LCD TASK SP: 0x%08X\n", get_sp());

    if (!lcd_initialized) {
        disp.external_vcc = 0;
        BOOL ret = ssd1306_init(&disp, 128, 64, 0x3C, dd_i2c0, buff);
        if (ret == TRUE) {
            printf("ssd1306_init succeeded.\n");
            ssd1306_poweron(&disp);
            lcd_initialized = true;
        } else {
            printf("ssd1306_init failed.\n");
            goto END;
        }
    }

    char line[32];

    printf("ssd1306_clear...\n");
    ssd1306_clear(&disp);

    printf("ssd1306_draw_string...\n");
    ssd1306_draw_string(&disp, 0, 0, 1, "sokoide OS v.0.0.1");
    printf("ssd1306_draw_line...\n");
    ssd1306_draw_line(&disp, 0, 30, 128, 30);
    ssd1306_draw_line(&disp, 0, 32, 128, 32);
    ssd1306_show(&disp);

    clear_rect(&disp, 0, 34, 128, 64);
    sprintf(line, "SP: 0x%08X", get_sp());
    ssd1306_draw_string(&disp, 0, 10, 1, line);

    time_t t;
    time(&t);
    sprintf(line, "up: %lld seconds", t);
    ssd1306_draw_string(&disp, 0, 20, 1, line);

    int count = 3;
    for (int n = 0; n < MAX_TASKS; n += count) {
        draw_loop(n, count);
    }

    sk_delay_task(1000);
    ssd1306_clear(&disp);
    ssd1306_show(&disp);
    sk_signal_semaphore(sid, 1);

    printf("Semaphore releaed, task_lcd_func completed...\n");

END:
    sk_exit_task();
}
