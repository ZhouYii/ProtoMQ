#include "includes/zhelpers.hpp"
#include "includes/social.pb.cc"

#include <iostream>

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

        netmsg::AppRequest_RegisterMessage reg_msg = rcv_msg.reg_msg();
        std::cout << reg_msg.username() << "  " <<
                     reg_msg.email() << "  " <<
                     reg_msg.secured_password() << std::endl;
        
        // std::cout << "Received request: " << string << std::endl;
        
        // Do some 'work'
        // sleep (1);
        
        //  Send reply back to client
         s_send (responder, "1");
        
    }
}
