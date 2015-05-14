#ifndef _WORKER_CPP
#define _WORKER_CPP

#include "worker.h"

void doRouting(CassSession* session, netmsg::AppRequest* rcv_msg) {
    // Routing Logic
    int64_t origin_phone_id;
    netmsg::AppRequest_MessageType type = rcv_msg->msg_type();
    std::cout << "message type " << type << " " << rcv_msg->has_create_event() << std::endl;
    std::cout << "update routing" << std::endl;
    // Ignore the invalid query
    if (!rcv_msg->has_phone_id()) {
        return;
    }
    origin_phone_id  = rcv_msg->phone_id();

    switch (type)
    {
        case netmsg::AppRequest_MessageType_tUpdateProfile :
            if (rcv_msg->has_update_profile()) {
                HandleRequestUpdateProfile(session, rcv_msg, origin_phone_id);
            }
            break;
        case netmsg::AppRequest_MessageType_tEventCreate :
            if (rcv_msg->has_create_event()) {
                std::cout << "correct branching" << std::endl;
                HandleRequestCreateEvent(session, rcv_msg, origin_phone_id);
            }
            break;

        case netmsg::AppRequest_MessageType_tEventAccept :
            if (rcv_msg->has_accept_event()) {
                break;
            }
            break;

        case netmsg::AppRequest_MessageType_tEventReject :
            if (rcv_msg->has_reject_event()) {
                break;
            }
            break;

        case netmsg::AppRequest_MessageType_tEventInvite :
            if (rcv_msg->has_invite_event()) {
                break;
            }
            break;

        case netmsg::AppRequest_MessageType_tPollAcceptedEvents :
            if (rcv_msg->has_poll_accepted()) {
                break;
            }
            break;

        case netmsg::AppRequest_MessageType_tPollInvitedEvents :
            if (rcv_msg->has_poll_invited()) {
                break;
            }
            break;

        case netmsg::AppRequest_MessageType_tRegistration :
            if (rcv_msg->has_reg_msg())
                HandleRequestRegistration(session, rcv_msg);
            break;

        case netmsg::AppRequest_MessageType_tLogin :
            if (rcv_msg->has_login_msg())
                HandleRequestLogin(session, rcv_msg);
            break;

        default :
            break;
    }
}

int main (int argc, char *argv[])
{
    // get connection
    std::cout << "update main" << std::endl;
    std::cout << "cassandra connection" << std::endl;
    //CassCluster* cluster = create_cluster("localhost");
    CassCluster* cluster = create_cluster("ec2-54-69-204-42.us-west-2.compute.amazonaws.com");
    CassSession* session = cass_session_new();
    CassFuture* close_future = NULL;

    if (cluster == NULL || connect_session(session, cluster) != CASS_OK) {
        cass_cluster_free(cluster);
        cass_session_free(session);
        return -1;
    }

    std::cout << "connection made" << std::endl;

    zmq::context_t context(1);
    zmq::socket_t responder(context, ZMQ_REP);
    responder.connect("tcp://localhost:5560");

    while(1)
    {
        //  Wait for next message from broker
        std::cout << "about to die?" << std::endl;
        zmq::message_t reply;
        responder.recv (&reply);
        std::cout << "no" << std::endl;

        netmsg::AppRequest rcv_msg;
        rcv_msg.ParseFromArray(reply.data(), reply.size());

        std::cout << rcv_msg.msg_type() << std::endl;
        std::cout << "Routing" << std::endl;
        doRouting(session, &rcv_msg);
        std::cout << "Done Routing" << std::endl;

        s_send (responder, "World");
        // std::cout << "Received request: " << string << std::endl;
        // Do some 'work'
        // sleep (1);
        // Send reply back to client
        // s_send (responder, "1");
    }
}

#endif
