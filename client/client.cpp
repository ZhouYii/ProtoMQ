#include <iostream>
#include <inttypes.h>
#include <ctime>
#include <fstream>
#include <vector>

#include "includes/zhelpers.hpp"
#include "includes/social.pb.h"

void SendMessage(netmsg::AppRequest* msg, zmq::socket_t* requester)
{
    std::string req_serialized;
    msg->SerializeToString(&req_serialized);
    zmq::message_t msg_req (req_serialized.size());
    memcpy ((void *) msg_req.data(),
                    req_serialized.c_str(),
                    req_serialized.size());

    requester->send(msg_req);
}

void CreateMessageEventCreation(netmsg::AppRequest* msg,
                               int64_t host_phone_number,
                               std::string location,
                               std::string title,
                               int64_t time)
{
    // Set message key
    msg->set_phone_id(host_phone_number);

    // Set type
    netmsg::AppRequest_MessageType type =
        netmsg::AppRequest_MessageType_tCreateEvent;
    msg->set_msg_type(type);

    // Set payload
    netmsg::AppRequest_CreateEvent* r_msg =
        new netmsg::AppRequest_CreateEvent();
    r_msg->set_host_id(host_phone_number);
    r_msg->set_title(title);
    r_msg->set_location(location);
    r_msg->set_time(time);
    // Remove... host phone number is not required
    r_msg->add_invited_users(host_phone_number);

    msg->set_allocated_create_event(r_msg);
}

int main (int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    zmq::context_t context(1);
    zmq::socket_t requester(context, ZMQ_REQ);
    requester.connect("tcp://localhost:5559");

    std::cout << "Connection made" << std::endl;

    // Build a registration message
    int64_t currtime = std::time(NULL);
    netmsg::AppRequest msg;
    CreateMessageEventCreation(&msg,
                               6505758649,
                               "mylocation1213",
                               "event_title",
                               currtime);

    SendMessage(&msg, &requester);
    std::string str = s_recv (requester);
    std::cout << "Received reply [" << str << "]" << std::endl;

    // Cleanup
    google::protobuf::ShutdownProtobufLibrary();
    requester.disconnect("tcp://localhost:5559");
    return 0;
}
