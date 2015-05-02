#ifndef _WORKER_H
#define _WORKER_H

#include "includes/zhelpers.hpp"
#include "db/db_lib.h"
#include "request_handlers.h"
#include <cassandra.h>

#include <iostream>
#include <fstream>

void doRouting(CassSession* session, netmsg::AppRequest* rcv_msg);
int main (int argc, char *argv[]);

#endif
