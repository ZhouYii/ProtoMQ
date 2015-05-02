#ifndef _DB_UTIL_H
#define _DB_UTIL_H
#include <cassandra.h>
#include <stdio.h>
#include <ctime>
#include <stdint.h>

// Prints Cassandra error to stderr
void print_error(CassFuture* future);

CassError execute_query(CassSession* session, const char* query);

CassError connect_session(CassSession* session,
                          const CassCluster* cluster);

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
