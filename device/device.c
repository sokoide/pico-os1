#include <kernel.h>

DeviceInfo device_table[DEVICE_NUM] = {
    {"iica", 0, 0, 0, dev_i2c_open, dev_i2c_read, dev_i2c_write}, // I2C0
    {"iicb", 1, 0, 0, dev_i2c_open, dev_i2c_read, dev_i2c_write}, // I2C1
};

static BOOL nm_cmp(const UB* nm1, const UB* nm2) {
    for (INT i = 0; i < DEVICE_NAME_LEN; i++, nm1++, nm2++) {
        if (*nm2 == 0)
            break;
        if (*nm1 != *nm2)
            return FALSE;
    }
    return (*nm1 == 0) ? TRUE : FALSE;
}

ID sk_open_device(const UB* name, UINT open_mode) {
    UINT interrupt_status;
    INT i;
    ERR err;

    for (i = 0; i < DEVICE_NUM; i++) {
        if (nm_cmp(device_table[i].name, name) == TRUE)
            break;
    }
    if (i >= DEVICE_NUM)
        return E_NOEXS;

    DI(interrupt_status);
    if (device_table[i].open_count == 0) {
        err = (*device_table[i].fn_open)(device_table[i].unit, open_mode);
        if (err == E_OK) {
            device_table[i].open_count++;
            device_table[i].open_mode = open_mode;
            err = i;
        }
    }
    EI(interrupt_status);
    return err;
}

ERR sk_sync_read_device(ID dd, W start, void* buf, SZ size, SZ* asize) {
    ERR err;

    if (dd >= DEVICE_NUM)
        return E_ID;
    if (device_table[dd].open_count == 0)
        return E_ID;
    if ((device_table[dd].open_mode & TD_READ) == 0)
        return E_OACV;

    err = (*device_table[dd].fn_sync_read)(device_table[dd].unit, start, buf,
                                           size, asize);

    return err;
}

ERR sk_sync_write_device(ID dd, W start, const void* buf, SZ size, SZ* asize) {
    ERR err;

    if (dd >= DEVICE_NUM)
        return E_ID;
    if (device_table[dd].open_count == 0)
        return E_ID;
    if ((device_table[dd].open_mode & TD_WRITE) == 0)
        return E_OACV;

    err = (*device_table[dd].fn_sync_write)(device_table[dd].unit, start, buf,
                                            size, asize);

    return err;
}
