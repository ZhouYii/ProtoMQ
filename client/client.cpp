#include <iostream>
#include "includes/zhelpers.hpp"
#include "includes/social.pb.h"

int main (int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // Build a registration message
    netmsg::AppRequest msg;
    msg.set_phone_id(6505758649);
    netmsg::AppRequest_MessageType type = 
        netmsg::AppRequest_MessageType_tRegistration;
    msg.set_msg_type(type);
    netmsg::AppRequest_RegisterMessage r_msg;
    r_msg.set_secured_password("password");
    r_msg.set_username("zhouyi");
    r_msg.set_email("yizhou4@illinois.edu");
    msg.set_allocated_reg_msg(&r_msg);
    // Msg Initialized
    //
    std::string req_serialized;
    msg.SerializeToString(&req_serialized);

    zmq::context_t context(1);

    zmq::socket_t requester(context, ZMQ_REQ);
    requester.connect("tcp://localhost:5559");

    for( int request = 0 ; request < 1 ; request++) {

        zmq::message_t msg_req (req_serialized.size());
        memcpy ((void *) msg_req.data(),
                        req_serialized.c_str(),
                        req_serialized.size());

        requester.send(msg_req);
        //s_send (requester, "Hello");
        //std::string string = s_recv (requester);
        //std::cout << "Received reply " << request
        //        << " [" << string << "]" << std::endl;
    }
}
