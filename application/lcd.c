#include "ssd1306.h"
#include "usermain.h"
#include <kernel.h>

void task_lcd_func(INT stacd, void* exinf);

ID tid_lcd;
TaskInfo task_lcd = {
    .task_attr = TA_HLNG | TA_RNG3 | TA_USERBUF,
    .task = task_lcd_func,
    .task_pri = 10,
    .stack_size = TASK_STACK_SIZE,
    .stack = TASK_STACK_BASE_N(3),
};

ssd1306_t disp;
UB buff[128 * 64 + 1];

void task_lcd_func(INT stacd, void* exinf) {
    tm_putstring("task_lcd_func...\r\n");

    printf("LCD TASK SP: 0x%08X\r\n", get_sp());

    disp.external_vcc = 0;
    BOOL ret = ssd1306_init(&disp, 128, 64, 0x3C, dd_i2c0, buff);
    if (ret == TRUE) {
        tm_putstring("ssd1306_init succeeded.\r\n");
    } else {
        tm_putstring("ssd1306_init failed.\r\n");
    }
    ssd1306_poweron(&disp);

    while (1) {
        tm_putstring("ssd1306_clear...\r\n");
        ssd1306_clear(&disp);
        sk_delay_task(1000);
        ssd1306_show(&disp);

        tm_putstring("ssd1306_draw_string...\r\n");
        ssd1306_draw_string(&disp, 0, 0, 2, "sokoide OS");
        ssd1306_show(&disp);
        sk_delay_task(1000);

        ssd1306_draw_string(&disp, 0, 20, 1, "ver 0.0.1");
        ssd1306_show(&disp);
        sk_delay_task(1000);

        tm_putstring("ssd1306_draw_line...\r\n");
        ssd1306_draw_line(&disp, 0, 30, 128, 30);
        ssd1306_show(&disp);
        sk_delay_task(1000);
    }
}
