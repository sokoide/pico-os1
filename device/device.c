#include <kernel.h>

DeviceInfo dev_tbl[DEVICE_NUM] = {
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

ID sk_opepn_device(const UB* name, UINT open_mode) {
    UINT intsts;
    INT i;
    ERR err;

    for (i = 0; i < DEVICE_NUM; i++) {
        if (nm_cmp(dev_tbl[i].name, name) == TRUE)
            break;
    }
    if (i >= DEVICE_NUM)
        return E_NOEXS;

    DI(intsts);
    if (dev_tbl[i].open_count == 0) {
        err = (*dev_tbl[i].fn_open)(dev_tbl[i].unit, open_mode);
        if (err == E_OK) {
            dev_tbl[i].open_count++;
            dev_tbl[i].open_mode = open_mode;
            err = i;
        }
    }
    EI(intsts);
    return err;
}

ERR sk_sync_read_device(ID dd, W start, void* buf, SZ size, SZ* asize) {
    ERR err;

    if (dd >= DEVICE_NUM)
        return E_ID;
    if (dev_tbl[dd].open_count == 0)
        return E_ID;
    if ((dev_tbl[dd].open_mode & TD_READ) == 0)
        return E_OACV;

    err =
        (*dev_tbl[dd].fn_sync_read)(dev_tbl[dd].unit, start, buf, size, asize);

    return err;
}

ERR sk_sync_write_device(ID dd, W start, const void* buf, SZ size, SZ* asize) {
    ERR err;

    if (dd >= DEVICE_NUM)
        return E_ID;
    if (dev_tbl[dd].open_count == 0)
        return E_ID;
    if ((dev_tbl[dd].open_mode & TD_WRITE) == 0)
        return E_OACV;

    err =
        (*dev_tbl[dd].fn_sync_write)(dev_tbl[dd].unit, start, buf, size, asize);

    return err;
}
