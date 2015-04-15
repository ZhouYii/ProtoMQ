#ifndef _WORKER_CPP
#define _WORKER_CPP

#include "includes/zhelpers.hpp"
#include "request_handlers.cpp"

#include <iostream>
#include <fstream>
int main (int argc, char *argv[])
{
    zmq::context_t context(1);

    zmq::socket_t responder(context, ZMQ_REP);
    responder.connect("tcp://localhost:5560");

    while(1)
    {
        //  Wait for next request from client
        // std::string string = s_recv (responder);
        zmq::message_t reply;
        responder.recv (&reply);

        netmsg::AppRequest rcv_msg;
        rcv_msg.ParseFromArray(reply.data(), reply.size());

        std::cout << rcv_msg.msg_type() << std::endl;
        netmsg::AppRequest_MessageType type = rcv_msg.msg_type();
        switch (type)
        {
            case netmsg::AppRequest_MessageType_tStatusUpdate :
                if (rcv_msg.has_status_updates())
                    handleRequestStatusUpdate(&rcv_msg);
                break;

            case netmsg::AppRequest_MessageType_tCreateRoom :
                break;

            case netmsg::AppRequest_MessageType_tCreatePost :
                break;

            case netmsg::AppRequest_MessageType_tRegistration :
                if (rcv_msg.has_reg_msg())
                    handleRequestRegistration(&rcv_msg);
                break;

            case netmsg::AppRequest_MessageType_tLogin :
                if (rcv_msg.has_login_msg())
                    handleRequestLogin(&rcv_msg);
                break;

            case netmsg::AppRequest_MessageType_tLogout :
                break;

            default :
                break;
        }

       
        // std::cout << "Received request: " << string << std::endl;
        
        // Do some 'work'
        // sleep (1);
        
        //  Send reply back to client
         s_send (responder, "1");
        
    }
}

#endif
