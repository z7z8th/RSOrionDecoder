#ifndef RS_PERSON_H_
#define RS_PERSON_H_

#include "rs_def.h"
#include "rs_face.h"

#define PERSON_FACES_MAXNUM     10

STRUCT(rs_person) {
    int     person_id;
    char    person_uuid[UUID_LEN];

    rs_face faces[PERSON_FACES_MAXNUM];
    int     faces_num;

    int     group_id;
    char    group_uuid[UUID_LEN];

    int     age;
    int     gender;

    char    name[STRING_LEN];
    char    phone[STRING_LEN];
    char    email[STRING_LEN];

    // ...
    RESERVED_FIELD(8)
};

STRUCT(rs_personid) {
    int     person_id;
    char    person_uuid[UUID_LEN];
};

STRUCT(rs_person_node) {
    int     person_id;
    char    person_uuid[UUID_LEN];
    rs_faceid   faces[PERSON_FACES_MAXNUM];
    int     faces_num;
};

STRUCT(rs_person_list) {
    /// people: rs_person_node array pointer
    /// alloc and free by caller
    rs_person_node* people;
    int             people_num;
};

#endif // RS_PERSON_H_

