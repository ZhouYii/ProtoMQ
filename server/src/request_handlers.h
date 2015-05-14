#ifndef _REQ_H_H
#define _REQ_H_H

// Request handlers are responsible for unpacking message contents
//
#include "includes/zhelpers.hpp"
#include "includes/social.pb.h"

#include <iostream>
#include <fstream>
#include <cassandra.h>

void HandleRequestLogin(CassSession* session,
                        netmsg::AppRequest* msg);

void HandleRequestRegistration(CassSession* session,
                              netmsg::AppRequest* msg);

void HandleRequestCreateEvent(CassSession* session,
                              netmsg::AppRequest* msg,
                              int64_t host_id);

void HandleRequestUpdateProfile(CassSession* session,
                              netmsg::AppRequest* msg,
                              int64_t host_id);

void HandleRequestStatusUpdate(CassSession* session, 
                               netmsg::AppRequest* msg);

void HandleRequestEventInvite(CassSession* session,
                               netmsg::AppRequest* msg);

netmsg::AppReply* HandleRequestGetUserInfo(CassSession* session,
                               netmsg::AppRequest* msg,
                               int64_t host_id,
                               zmq::socket_t* responder);
#endif


