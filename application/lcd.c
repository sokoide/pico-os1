#include "ssd1306.h"
#include "usermain.h"
#include <kernel.h>
#include <stdio.h>

void task_lcd_func(INT stacd, void* exinf);

ID tid_lcd;
TaskInfo task_lcd = {.task_attr = TA_HLNG | TA_RNG3,
                     .task = task_lcd_func,
                     .task_pri = 10,
                     .stack_size = 0,
                     .stack = NULL};

ssd1306_t disp;
UB buff[128 * 64 + 1];

void clear_rect(ssd1306_t* p, uint32_t x, uint32_t y, uint32_t width,
                uint32_t height) {
    for (int yy = y; yy < y + height; yy++) {
        for (int xx = x; xx <= x + width; xx++) {
            ssd1306_clear_pixel(&disp, xx, yy);
        }
    }
}

void set_line(char* line, int i) {
    sprintf(line, "STK_BASE%02d: 0x%08X\r\n", i, TASK_STACK_BASE_N(i));
}

void draw_loop(int n, int count) {
    char line[32];
    printf("draw_loop...");
    clear_rect(&disp, 0, 34, 128, 64);
    for (int i = 0; i < count; i++) {
        if (i + n < MAX_TASKS) {
            set_line(line, i + n);
            ssd1306_draw_string(&disp, 0, 34 + i * 10, 1, line);
        }
    }
    ssd1306_show(&disp);
    sk_delay_task(3000);
}
void task_lcd_func(INT stacd, void* exinf) {
    printf("task_lcd_func...\r\n");

    printf("LCD TASK SP: 0x%08X\r\n", get_sp());

    disp.external_vcc = 0;
    BOOL ret = ssd1306_init(&disp, 128, 64, 0x3C, dd_i2c0, buff);
    if (ret == TRUE) {
        printf("ssd1306_init succeeded.\r\n");
    } else {
        printf("ssd1306_init failed.\r\n");
    }
    ssd1306_poweron(&disp);

    printf("TASK_STACK_UPPER_LIMIT: 0x%08X\r\n", TASK_STACK_UPPER_LIMIT);
    printf("MAIN_TASK_STACK_BASE: 0x%08X\r\n", MAIN_TASK_STACK_BASE);
    printf("MAIN_TASK SP: 0x%08X\r\n", get_sp());
    for (int i = 0; i < MAX_TASKS; i++) {
        printf("TASK_STACK_BASE_N[%02d]: 0x%08X\r\n", i, TASK_STACK_BASE_N(i));
    }

    char line[32];

    printf("ssd1306_clear...\r\n");
    ssd1306_clear(&disp);

    printf("ssd1306_draw_string...\r\n");
    ssd1306_draw_string(&disp, 0, 0, 1, "sokoide OS v.0.0.1");
    printf("ssd1306_draw_line...\r\n");
    ssd1306_draw_line(&disp, 0, 30, 128, 30);
    ssd1306_draw_line(&disp, 0, 32, 128, 32);

    uint32_t x, y;
    while (1) {
        clear_rect(&disp, 0, 34, 128, 64);
        sprintf(line, "SP_LIMIT: 0x%08X", TASK_STACK_UPPER_LIMIT);
        ssd1306_draw_string(&disp, 0, 10, 1, line);
        sprintf(line, "SP: 0x%08X\r\n", get_sp());
        ssd1306_draw_string(&disp, 0, 20, 1, line);

        int count = 3;
        for (int n = 0; n < MAX_TASKS; n += count) {
            draw_loop(n, count);
        }
    }
}
