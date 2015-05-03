#ifndef _WORKER_CPP
#define _WORKER_CPP

#include "worker.h"

void doRouting(CassSession* session, netmsg::AppRequest* rcv_msg) {
    // Routing Logic
    netmsg::AppRequest_MessageType type = rcv_msg->msg_type();
    std::cout << "message type " << type << " " << rcv_msg->has_create_event() << std::endl;
    std::cout << "seek message type " << netmsg::AppRequest_MessageType_tCreateEvent << std::endl;
    switch (type)
    {
        case netmsg::AppRequest_MessageType_tStatusUpdate :
            if (rcv_msg->has_status_updates())
                HandleRequestStatusUpdate(session, rcv_msg);
            break;

        case netmsg::AppRequest_MessageType_tCreateEvent :
            if (rcv_msg->has_create_event()) {
                std::cout << "correct branching" << std::endl;
                HandleRequestCreateEvent(session, rcv_msg);
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

        case netmsg::AppRequest_MessageType_tPollAccepted :
            if (rcv_msg->has_poll_accepted()) {
                break;
            }
            break;

        case netmsg::AppRequest_MessageType_tPollInvited :
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

        case netmsg::AppRequest_MessageType_tLogout :
            break;

        default :
            break;
    }
}

int main (int argc, char *argv[])
{
    // get connection
    std::cout << "cassandra connection" << std::endl;
    CassCluster* cluster = create_cluster("54.69.204.42");
    if (cluster == NULL) {
        std::cout << "cluster creation failed" << std::endl;
    }
    CassSession* session = cass_session_new();
    std::cout << "connection made" << std::endl;
    
    zmq::context_t context(1);
    zmq::socket_t responder(context, ZMQ_REP);
    responder.connect("tcp://localhost:5560");

    while(1)
    {
        //  Wait for next message from broker
        zmq::message_t reply;
        responder.recv (&reply);

        netmsg::AppRequest rcv_msg;
        rcv_msg.ParseFromArray(reply.data(), reply.size());

        std::cout << rcv_msg.msg_type() << std::endl;
        std::cout << "Routing" << std::endl;
        doRouting(session, &rcv_msg);
        std::cout << "Done Routing" << std::endl;

        // std::cout << "Received request: " << string << std::endl;
        // Do some 'work'
        // sleep (1);
        // Send reply back to client
        // s_send (responder, "1");
    }
}

#endif
