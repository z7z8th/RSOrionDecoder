#ifndef RS_DEVICE_H_
#define RS_DEVICE_H_

#include "rs_def.h"

STRUCT(rs_device) {
    char    mac_address[STRING_LEN];

    char    name[STRING_LEN];
    char    type[STRING_LEN];
    char    description[STRING_LEN];
    char    version[STRING_LEN];

    // ...
    RESERVED_FIELD(8)
};

#endif // RS_DEVICE_H_

