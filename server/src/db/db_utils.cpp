#ifndef _DB_UTIL_CPP
#define _DB_UTIL_CPP

#include "db_utils.h"

void print_error(CassFuture* future) {
  const char* message;
  size_t message_length;
  cass_future_error_message(future, &message, &message_length);
  fprintf(stderr, "Error: %.*s\n", (int)message_length, message);
}

CassError execute_query(CassSession* session, const char* query) {
  CassError rc = CASS_OK;
  CassStatement* statement = cass_statement_new(query, 0);
  CassFuture* future = cass_session_execute(session, statement);

  cass_future_wait(future);

  rc = cass_future_error_code(future);
  if (rc != CASS_OK) {
    print_error(future);
  }

  cass_future_free(future);
  cass_statement_free(statement);

  return rc;
}

void GenerateV1Uuid(CassUuid* cass_uuid) {
#ifndef _DB_UTIL_H
#define _DB_UTIL_H
#include <cassandra.h>
#include <stdio.h>
#include <ctime>
#include <stdint.h>

// Prints Cassandra error to stderr
void print_error(CassFuture* future);

CassError execute_query(CassSession* session, const char* query);

/* Generate sortable V1 UUIDs for Cassandra Events.
 * Input :
 *  cass_uuid - pointer to allocated structure
 * Output :
 *  None
 * Side Effect :
 *  Initializes contents of cass_uuid structure to be a V1 timestamp
 */
void GenerateV1Uuid(CassUuid* cass_uuid);
#endif
    cass_uint64_t cass_time = std::time(NULL);
    cass_uuid_min_from_time(cass_time, cass_uuid);
}
#endif
