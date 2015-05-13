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
#include "db_utils.h"

/* DB abstraction for sending event invitations to multiple users.
 *
 * Input :
 *  session - Cassandra database session
 *  uuid - uuid string representing the event
 *  invited_arr - pointer to allocated contiguous region of memory containing
 *                user_id elements (phone numbers)
 *  num_invited - number of users invited in this batch
 */
CassError DbBatchInviteUsers(CassSession* session,
                             const CassUuid* uuid,
                             int64_t* invited_arr,
                             int num_invited);

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
CassUuid DbCreateNewEvent(CassSession* session,
                              const int64_t host_id,
                              const int64_t time,
                              const char* title,
                              const char* location);

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

/* Generate connection to Cassandra Cluster
 */
CassError connect_session(CassSession* session, const CassCluster* cluster);

/* Create cluster object with seed node host address hints
 *
 * Input :
 *  host_address = comma-separated list of host address values which are the seed hosts
 *                  for cassandra connection
 * Output : 
 *  Pointer to allocated cassandra cluster object
 */
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
