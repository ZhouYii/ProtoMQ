#ifndef _DB_LIB_H
#define _DB_LIB_H

#include <cassandra.h>
#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <vector>

#include <assert.h>

#include "query_structs.h"
#include "db_utils.cpp"

/*
 * Create new event 
 * Input : 
 *      host_id - event creator's phone number
 *      time - posix timestamp, for when event start
 *      title - title of new event
 *      location - location of event
 * Output :
 *      CassError
 */
CassError DbCreateNewEvent(CassSession* session,
                              const int64_t host_id,
                              const int64_t time,
                              const std::string title,
                              const std::string location);

CassError db_create_new_user(CassSession* session,
                             const int64_t phone_id,
                             const char* password,
                             const char* email);

CassError insert_into_basic(CassSession* session,
                            const char* key,
                            const Basic* basic);

CassError select_from_friend_by_phoneid(CassSession* session,
                                        const CassPrepared* prepared,
                                        const int64_t phone_id,
                                        std::vector<int64_t>* response);

CassError select_from_user_by_phone(CassSession* session,
                                    const CassPrepared * prepared,
                                    const int64_t phone_id,
                                    UserQueryByPhone* response);

CassCluster* create_cluster(const char* host_address = "127.0.0.1");

CassError prepare_query(CassSession* session,
                        const CassPrepared** prepared,
                        const char* query_string);

CassError prepare_select_from_blacklist_by_phoneid(CassSession* session,
                                                 const CassPrepared** prepared);

CassError prepare_select_from_friend_by_phoneid(CassSession* session,
                                               const CassPrepared** prepared);

CassError prepare_select_from_user_by_phoneid(CassSession* session,
                                            const CassPrepared** prepared);

CassError prepare_select_from_basic(CassSession* session, const CassPrepared** prepared);
#endif
