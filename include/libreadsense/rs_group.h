#ifndef RS_GROUP_H_
#define RS_GROUP_H_

#include "rs_def.h"
#include "rs_person.h"

STRUCT(rs_group) {
    int     group_id;
    char    group_uuid[UUID_LEN];

    char    name[STRING_LEN];
    char    type[STRING_LEN];
    char    description[BIG_STRING_LEN];

    int     people_num;
    int     faces_num;

    // ...
    RESERVED_FIELD(8)
};

STRUCT(rs_groupid) {
    int     group_id;
    char    group_uuid[UUID_LEN];
};

STRUCT(rs_group_node) {
    int     group_id;
    char    group_uuid[UUID_LEN];
    /// people: rs_person_node array pointer type
    /// alloc and free by caller
    rs_person_node* people;
    int     people_num;
};

STRUCT(rs_group_list) {
    /// groups: rs_group_node array pointer
    /// alloc and free by caller
    rs_group_node*  groups;
    int             groups_num;
};

#endif // RS_GROUP_H_

