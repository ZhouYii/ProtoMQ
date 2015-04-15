#ifndef _DB_LIB_C
#define _DB_LIB_C

#include <cassandra.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>

#include <assert.h>

#include "query_structs.h"
#include "db_utils.cpp"

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


CassCluster* create_cluster() {
    CassCluster* cluster = cass_cluster_new();
    cass_cluster_set_contact_points(cluster, "127.0.0.1");
    return cluster;
}

CassError prepare_select_from_user_by_phone(CassSession* session,
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

    std::cout  << 1 << std::endl;
    if (prepare_select_from_user_by_phone(session, &prepared) == CASS_OK)
    {
        std::cout << 2 << std::endl;
        select_from_user_by_phone(session, prepared, 3333333333, &output);
        std::cout << 3 << std::endl;
        cass_prepared_free(prepared);
        std::cout << "Retrieved password:" << output.password << std::endl;
    }


    // Cleanup
    close_future = cass_session_close(session);
    cass_future_wait(close_future);
    cass_future_free(close_future);

    cass_cluster_free(cluster);
    cass_session_free(session);

    return 0;
}

#endif
