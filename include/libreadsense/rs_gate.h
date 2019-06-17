#ifndef RS_GATE_H_
#define RS_GATE_H_

#include "rs_def.h"

STRUCT(rs_gate_instruction) {
    int     instruct;
    int     duration;

    char    reason[BIG_STRING_LEN];
    char    text[BIG_STRING_LEN];
    char    voice[BIG_STRING_LEN];

    char    name[STRING_LEN];
    char    uuid[UUID_LEN];

    // ...
    RESERVED_FIELD(8)
};

STRUCT(rs_gate_state) {
    int     state;

    //...
    //RESERVED_FIELD(8)
};

#endif // RS_GATE_H_

