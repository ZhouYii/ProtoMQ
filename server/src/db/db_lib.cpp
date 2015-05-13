#ifndef _DB_LIB_C
#define _DB_LIB_C

#include "db_lib.h"

CassError DbSingleUserAttendingEvent(CassSession* session,
                                     const CassUuid* uuid,
                                     int64_t invited_user)
{
    CassError rc = CASS_OK;
    CassStatement* statement = NULL;
    CassFuture* future = NULL;

    if (invited_user == 0)
        return CASS_OK;
    
    const std::string userid_string = std::to_string(invited_user);
    const std::string query = "UPDATE social.events SET attending_userids = attending_userids + { " + userid_string + "  } \
                               WHERE event_id = ?;";
    statement = cass_statement_new(query.c_str(), 1);

    // Prepare UUID
    std::cout << "bind 0 :" << cass_statement_bind_uuid(statement, 0, *(uuid)) << std::endl;

    future = cass_session_execute(session, statement);
    cass_future_wait(future);

    rc = cass_future_error_code(future);
    if (rc != CASS_OK) {
        std::cout << "###error cassandnra" << rc <<  std::endl;
        print_error(future);
    }

    // Free alloc'd mem.
    cass_future_free(future);
    cass_statement_free(statement);
    return rc;
}

/* DB abstraction for sending event invitations to multiple users.
 *
 * Input :
 *  session - Cassandra database session
 *  uuid - uuid string representing the event
 *  invited_arr - pointer to allocated contiguous region of memory containing
 *                user_id elements (phone numbers)
 *  num_invited - number of users invited in this batch
 */
// TODO : input is message, no need for array
CassError DbBatchInviteUsers(CassSession* session,
                             const CassUuid* uuid,
                             int64_t* invited_arr,
                             int num_invited)
{
    CassError rc = CASS_OK;
    CassStatement* statement = NULL;
    CassFuture* future = NULL;
    CassCollection* collection = NULL;

    if (num_invited == 0 || invited_arr == NULL)
        return CASS_OK;
    
    const char* query = "UPDATE social.events SET attending_userids = attending_userids + (?) \
                         WHERE event_id = (?);";
    statement = cass_statement_new(query, 2);

    // Prepare collection
    collection = cass_collection_new(CASS_COLLECTION_TYPE_SET, num_invited);
    for (int usr_idx = 0; usr_idx < num_invited; usr_idx += 1) {
        cass_collection_append_int64(collection, invited_arr[usr_idx]);
    }

    // Prepare UUID
    // TODO : Check error?

    std::cout << "bind 0 :" << cass_statement_bind_collection(statement, 0, collection) << std::endl;
    std::cout << "bind 1 :" << cass_statement_bind_uuid(statement, 1, *(uuid)) << std::endl;

    // Pipeline memory free
    future = cass_session_execute(session, statement);
    cass_collection_free(collection);
    cass_future_wait(future);

    rc = cass_future_error_code(future);
    if (rc != CASS_OK) {
        std::cout << "###error cassandnra" << rc <<  std::endl;
        print_error(future);
    }

    // Free alloc'd mem.
    cass_future_free(future);
    cass_statement_free(statement);
    return rc;
}


CassUuid DbCreateNewEvent(CassSession* session,
                              const int64_t host_id,
                              const int64_t time,
                              const char* title,
                              const char* location,
                              const char* uuid)
{
    CassError rc = CASS_OK;
    CassStatement* statement = NULL;
    CassFuture* future = NULL;

    CassCollection* collection = NULL;
    collection = cass_collection_new(CASS_COLLECTION_TYPE_SET, 1);
    std::cout << cass_collection_append_int64(collection, host_id) << std::endl;

    CassUuid cass_uuid1;
    cass_uuid_from_string(uuid, &cass_uuid1);

    const char* query = "INSERT INTO social.events (event_id, title, location, \
                         begin_time, attending_userids, public) VALUES (?, ?, ?, ?, ?)";
    statement = cass_statement_new(query, 6);
    cass_statement_bind_uuid(statement, 0, cass_uuid1);
    cass_statement_bind_string(statement, 1, title);
    cass_statement_bind_string(statement, 2, location);
    cass_statement_bind_int64(statement, 3, time);
    cass_statement_bind_collection(statement, 4, collection);
    cass_statement_bind_bool(statement, 5, cass_true);

    future = cass_session_execute(session, statement);
    cass_collection_free(collection);
    cass_future_wait(future);

    rc = cass_future_error_code(future);
    if (rc != CASS_OK) {
        print_error(future);
    }

    // Free alloc'd mem.
    cass_future_free(future);
    cass_statement_free(statement);
    return cass_uuid1;
}

CassError db_create_new_user(CassSession* session,
                             const int64_t phone_id,
                             const char* password,
                             const char* email)
{
    CassError rc = CASS_OK;
    CassStatement* statement = NULL;
    CassFuture* future = NULL;
    const char* query = "INSERT INTO social.user (phone_number, password) VALUES (?, ?)";

    statement = cass_statement_new(query, 2);
    cass_statement_bind_int64(statement, 0, phone_id);
    cass_statement_bind_string(statement, 1, password);

    future = cass_session_execute(session, statement);
    cass_future_wait(future);

    rc = cass_future_error_code(future);
    if (rc != CASS_OK) {
        print_error(future);
    }

    cass_future_free(future);
    cass_statement_free(statement);

    return rc;
}

CassError insert_into_basic(CassSession* session,
                            const char* key,
                            const Basic* basic) 
{
  CassError rc = CASS_OK;
  CassStatement* statement = NULL;
  CassFuture* future = NULL;
  const char* query = "INSERT INTO examples.basic (key, bln, flt, dbl, i32, i64) VALUES (?, ?, ?, ?, ?, ?);";

  statement = cass_statement_new(query, 6);

  cass_statement_bind_string(statement, 0, key);
  cass_statement_bind_bool(statement, 1, basic->bln);
  cass_statement_bind_float(statement, 2, basic->flt);
  cass_statement_bind_double(statement, 3, basic->dbl);
  cass_statement_bind_int32(statement, 4, basic->i32);
  cass_statement_bind_int64(statement, 5, basic->i64);

  future = cass_session_execute(session, statement);

  cass_future_wait(future);

  rc = cass_future_error_code(future);
  if (rc != CASS_OK) {
    print_error(future);
  }

  cass_future_free(future);
  cass_statement_free(statement);

  return rc;
}


CassError select_from_friend_by_phoneid(CassSession* session,
                                        const CassPrepared* prepared,
                                        const int64_t phone_id,
                                        std::vector<int64_t>* response)
{
    CassError rc = CASS_OK;
    CassStatement* statement = NULL;
    CassFuture* future = NULL;
    statement = cass_prepared_bind(prepared);
    cass_statement_bind_int64(statement, 0, phone_id);
    future = cass_session_execute(session, statement);
    cass_future_wait(future);

    rc = cass_future_error_code(future);
    if (rc != CASS_OK) 
    {
        print_error(future);
    } else
    {
        const CassResult* result = cass_future_get_result(future);
        CassIterator* iterator = cass_iterator_from_result(result);
        int64_t friend_id;

        while (cass_iterator_next(iterator)) 
        {
            const CassRow* row = cass_iterator_get_row(iterator);
            cass_value_get_int64(cass_row_get_column(row, 1), &friend_id);
            response->push_back(friend_id);
        }

        cass_result_free(result);
        cass_iterator_free(iterator);
    }

    cass_future_free(future);
    cass_statement_free(statement);

    return rc;
}

CassError select_from_user_by_phone(CassSession* session,
                                    const CassPrepared * prepared,
                                    const int64_t phone_id,
                                    UserQueryByPhone* response)
{
  CassError rc = CASS_OK;
  CassStatement* statement = NULL;
  CassFuture* future = NULL;
  statement = cass_prepared_bind(prepared);
  cass_statement_bind_int64(statement, 0, phone_id);
  future = cass_session_execute(session, statement);
  cass_future_wait(future);

  size_t password_length;

  rc = cass_future_error_code(future);
  if (rc != CASS_OK) {
    print_error(future);
  } else {
    const CassResult* result = cass_future_get_result(future);
    CassIterator* iterator = cass_iterator_from_result(result);

    if (cass_iterator_next(iterator)) {
      const CassRow* row = cass_iterator_get_row(iterator);
      response->phone_id = phone_id;
      cass_value_get_string(cass_row_get_column(row, 1),
                            &response->password,
                            &response->size);
    }

    cass_result_free(result);
    cass_iterator_free(iterator);
  }

  cass_future_free(future);
  cass_statement_free(statement);

  return rc;
}
CassError connect_session(CassSession* session, const CassCluster* cluster) {
    CassError rc = CASS_OK;
    CassFuture* future = cass_session_connect(session, cluster);
    cass_future_wait(future);
    rc = cass_future_error_code(future);
    if (rc != CASS_OK)
        print_error(future);
    cass_future_free(future);

    return rc;
}

CassCluster* create_cluster(const char* host_address) {
    CassCluster* cluster = cass_cluster_new();
    CassError result = cass_cluster_set_contact_points(cluster, host_address);
    if (result != CASS_OK) {
        cluster = NULL;
    }
    return cluster;
}

/*
 *  Prepared Queries
 */
CassError prepare_query(CassSession* session,
                        const CassPrepared** prepared,
                        const char* query_string)
{
    CassError rc = CASS_OK;
    CassFuture* future = NULL;
    const char* query = query_string;

    future = cass_session_prepare(session, query);
    cass_future_wait(future);

    rc = cass_future_error_code(future);
    if (rc != CASS_OK) {
        print_error(future);
    } else {
        *prepared = cass_future_get_prepared(future);
    }

    cass_future_free(future);

    return rc;
}

CassError prepare_select_from_blacklist_by_phoneid(CassSession* session,
                                                 const CassPrepared** prepared)
{
    const char* query = "SELECT * FROM social.blacklist WHERE userid1=?";
    return prepare_query(session, prepared, query);
}

CassError prepare_select_from_friend_by_phoneid(CassSession* session,
                                               const CassPrepared** prepared)
{
    const char* query = "SELECT * FROM social.friend WHERE userid1=?";
    return prepare_query(session, prepared, query);
}

CassError prepare_select_from_user_by_phoneid(CassSession* session,
                                            const CassPrepared** prepared)
{
    CassError rc = CASS_OK;
    CassFuture* future = NULL;
    const char* query = "SELECT * FROM social.user WHERE phone_number =?";

    future = cass_session_prepare(session, query);
    cass_future_wait(future);

    rc = cass_future_error_code(future);
    if (rc != CASS_OK) {
        print_error(future);
    } else {
        *prepared = cass_future_get_prepared(future);
    }

    cass_future_free(future);

    return rc;
}

CassError prepare_select_from_basic(CassSession* session, const CassPrepared** prepared) {
  CassError rc = CASS_OK;
  CassFuture* future = NULL;
  const char* query = "SELECT * FROM examples.basic WHERE key = ?";

  future = cass_session_prepare(session, query);
  cass_future_wait(future);

  rc = cass_future_error_code(future);
  if (rc != CASS_OK) {
    print_error(future);
  } else {
    *prepared = cass_future_get_prepared(future);
  }

  cass_future_free(future);

  return rc;
}

/*
 * Query abstractions for database
 */
/*
int main() {

    // get connection
    CassCluster* cluster = create_cluster();
    CassSession* session = cass_session_new();
    CassFuture* close_future = NULL;
    Basic input = { cass_true, 0.001f, 0.0002, 1, 2 };
    UserQueryByPhone output;
    const CassPrepared* prepared = NULL;

    if (connect_session(session, cluster) != CASS_OK) {
        cass_cluster_free(cluster);
        cass_session_free(session);
        return -1;
    }

    db_create_new_user(session, 3333333333, "password", NULL);

    if (prepare_select_from_user_by_phoneid(session, &prepared) == CASS_OK)
    {
        select_from_user_by_phone(session, prepared, 3333333333, &output);
        cass_prepared_free(prepared);
        std::cout << "Retrieved password:" << output.password << std::endl;
    }

    if (prepare_select_from_friend_by_phoneid(session, &prepared) == CASS_OK)
    {
        std::vector<int64_t> friends;
        select_from_friend_by_phoneid(session,prepared,6505758649,&friends);
        cass_prepared_free(prepared);

        for (int i=0; i < friends.size(); i+= 1) {
            std::cout << "Friend:" << friends.at(i) << std::endl;
        }
    }

    // Cleanup
    close_future = cass_session_close(session);
    cass_future_wait(close_future);
    cass_future_free(close_future);

    cass_cluster_free(cluster);
    cass_session_free(session);

    return 0;
}
*/
#endif
