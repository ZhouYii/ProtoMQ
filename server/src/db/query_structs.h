#ifndef _QUERY_STRUCT_H
#define _QUERY_STRUCT_H

#include <cassandra.h>

struct _Basic {
    cass_bool_t bln;
    cass_float_t flt;
    cass_double_t dbl;
    cass_int32_t i32;
    cass_int64_t i64;
};

typedef struct _Basic Basic;


struct _UserQueryByPhone {
    cass_int64_t phone_id;
    const char* password;
    size_t size;
};
typedef struct _UserQueryByPhone UserQueryByPhone;

#endif
