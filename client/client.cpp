#include <iostream>
#include <inttypes.h>
#include "includes/zhelpers.hpp"
#include "includes/social.pb.h"

void createRegistrationMessage(netmsg::AppRequest* msg,
                               int64_t phone_num,
                               std::string username,
                               std::string password,
                               std::string email) 
{
    // Set message key
    msg->set_phone_id(phone_num);

    // Set type
    netmsg::AppRequest_MessageType type =
        netmsg::AppRequest_MessageType_tRegistration;
    msg->set_msg_type(type);

    // Set payload
    netmsg::AppRequest_RegisterMessage* r_msg =
        new netmsg::AppRequest_RegisterMessage();
    r_msg->set_username(username);
    r_msg->set_secured_password(password);
    r_msg->set_email(email);
    msg->set_allocated_reg_msg(r_msg);
}

void sendMessage(netmsg::AppRequest* msg, zmq::socket_t* requester)
{
    std::string req_serialized;
    msg->SerializeToString(&req_serialized);
    zmq::message_t msg_req (req_serialized.size());
    memcpy ((void *) msg_req.data(),
                    req_serialized.c_str(),
                    req_serialized.size());

    requester->send(msg_req);
}

int main (int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    zmq::context_t context(1);
    zmq::socket_t requester(context, ZMQ_REQ);
    requester.connect("tcp://localhost:5559");

    // Build a registration message
    netmsg::AppRequest msg;
    createRegistrationMessage(&msg,
                                6505758649,
                                "zhouyi",
                                "password",
                                "yi@email.com");

    netmsg::AppRequest msg2;
    createRegistrationMessage(&msg2,
                                6505758648,
                                "zhou yi2",
                                "password",
                                "yi@email.com");

    //s_send (requester, "Hello");
    sendMessage(&msg, &requester);
    std::string str = s_recv (requester);
    std::cout << "Received reply [" << str << "]" << std::endl;

    sendMessage(&msg2, &requester);
    str = s_recv (requester);
    std::cout << "Received reply [" << str << "]" << std::endl;

    // Cleanup
    google::protobuf::ShutdownProtobufLibrary();
    requester.disconnect("tcp://localhost:5559");
    return 0;
}
