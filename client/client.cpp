#include <iostream>
#include <inttypes.h>
#include <fstream>
#include <vector>

#include "includes/zhelpers.hpp"
#include "includes/social.pb.h"

void attachPhotoToMessage(netmsg::AppRequest_StatusUpdate* msg,
                          std::string* file_path)
{
    char* buf;
    std::ifstream file (*(file_path),
                            std::ios::in | std::ios::binary | std::ios::ate);
    if (file.is_open())
    {
        unsigned long long size = file.tellg();
        buf = new char [size];
        file.seekg(0, std::ios::beg);
        file.read(buf, size);
        file.close();

        msg->add_photos(buf);
        delete[] buf;
        std::cout << "Client: Attached 1 photo" << std::endl;
    }
    else
    {
        return;
    }
}

void createStatusMessage(netmsg::AppRequest* msg,
                               int64_t phone_num,
                               std::string body,
                               std::string** photos,
                               int num_photos)
{
    // Set message key
    msg->set_phone_id(phone_num);

    // Set type
    netmsg::AppRequest_MessageType type =
        netmsg::AppRequest_MessageType_tUpdateStatus;
    msg->set_msg_type(type);

    // Set text payload
    netmsg::AppRequest_StatusUpdate* r_msg =
        new netmsg::AppRequest_StatusUpdate();
    r_msg->set_body(body);


    // Set photo payload
    for (int path_idx = 0; path_idx < num_photos; path_idx += 1)
        attachPhotoToMessage(r_msg, *(photos+path_idx));

    // Attach message
    msg->set_allocated_status_updates(r_msg);
    std::cout << "Client: Attached " << r_msg->photos_size() << std::endl;
}

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

    std::string* photo_paths[1];
    std::string path1 = "img.jpeg";
    photo_paths[0] = &path1;
    netmsg::AppRequest msg3;
    createStatusMessage(&msg3,
                            6505758649,
                            "New status message here.",
                            photo_paths,
                            1);

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
