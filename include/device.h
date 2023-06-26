#ifndef _DEVICE_H_
#define _DEVICE_H_

#include "typedef.h"

#define DEVICE_NAME_LEN 8
typedef struct {
    UB name[DEVICE_NAME_LEN];
    UINT unit;
    UINT open_mode;
    UINT open_count;
    ERR (*fn_open)();
    ERR (*fn_sync_read)();
    ERR (*fn_sync_write)();
} DeviceInfo;

#define DEVICE_NUM 3
extern DeviceInfo dev_tbl[];

// I2C
extern ERR dev_i2c_open(UW unit, UINT omode);
extern ERR dev_i2c_read(UW unit, UW sadr, UB* buf, SZ size, SZ* asize);
extern ERR dev_i2c_write(UW unit, UW sadr, UB* buf, SZ size, SZ* asize);

#endif
