#include <iostream>
#include <inttypes.h>
#include <ctime>
#include <fstream>
#include <vector>

#include "includes/zhelpers.hpp"
#include "includes/social.pb.h"

void attachPhotoToMessage(netmsg::AppRequest_UpdateProfile* msg,
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

        msg->set_profile_photo(buf,size);
        delete[] buf;
        std::cout << "Client: Attached 1 photo" << std::endl;
        std::cout << "Photo Size " << std::to_string(size) << std::endl;
    }
    else
    {
        return;
    }
}

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

void CreateGetUserInfoMessage(netmsg::AppRequest* msg,
                               int64_t host_phone_number)
{
    // Set message key
    msg->set_phone_id(host_phone_number);
    msg->add_request_user_info(host_phone_number);
    msg->add_request_user_info(6505758650);

    // Set type
    netmsg::AppRequest_MessageType type =
        netmsg::AppRequest_MessageType_tUserInfoRequest;
    msg->set_msg_type(type);
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
        netmsg::AppRequest_MessageType_tEventCreate;
    msg->set_msg_type(type);

    // Set payload
    netmsg::AppRequest_CreateEvent* r_msg =
        new netmsg::AppRequest_CreateEvent();
    r_msg->set_title(title);
    r_msg->set_event_uuid1("5552b80b-0000-1000-8080-808080808081");
    r_msg->set_location(location);
    r_msg->set_time(time);
    // Remove... host phone number is not required
    r_msg->add_invited_users(host_phone_number);
    r_msg->add_invited_users(8888888888);
    r_msg->add_invited_users(9999999999);
    r_msg->set_is_public(true);

    msg->set_allocated_create_event(r_msg);
}

void createProfileUpdate(netmsg::AppRequest* msg,
                               int64_t phone_num,
                               std::string** photos,
                               int num_photos)
{
    // Set message key
    msg->set_phone_id(phone_num);

    // Set type
    netmsg::AppRequest_MessageType type =
        netmsg::AppRequest_MessageType_tUpdateProfile;
    msg->set_msg_type(type);

    netmsg::AppRequest_UpdateProfile* r_msg =
        new netmsg::AppRequest_UpdateProfile();
    r_msg->set_email("mygreatemail@aol.com");

    // Set photo payload
    for (int path_idx = 0; path_idx < num_photos; path_idx += 1)
        attachPhotoToMessage(r_msg, *(photos+path_idx));

    msg->set_allocated_update_profile(r_msg);
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

    /*
    // For event testing
    CreateMessageEventCreation(&msg,
                               6505758649,
                               "adfa;sdfjkalsdjfmylocation1213",
                               "event_titleasdfjasdfsdkfl",
                               currtime);
    */

    /*
    Test for updating profile picture
    std::string* photo_paths[1];
    std::string path1 = "img.jpeg";
    photo_paths[0] = &path1;
    netmsg::AppRequest msg3;
    createProfileUpdate(&msg3, 6505758649, photo_paths, 1);
    */

    // Test for retrieveing user information
    netmsg::AppRequest msg3;
    CreateGetUserInfoMessage(&msg3, 6505758649);

    SendMessage(&msg3, &requester);
    std::string str = s_recv (requester);
    netmsg::AppReply* reply = new netmsg::AppReply();
    reply->ParseFromString(str);

    std::cout << "Received reply [" << str << "]" << std::endl;
    std::cout << "has response type " << reply->has_response_type() << std::endl;
    std::cout << "recieved response type " << reply->response_type() << std::endl;
    std::cout << "has event uuid " << reply->has_event_uuid() << std::endl;
    std::cout << "users size " << reply->users_size() << std::endl;
    std::cout << "friend requests size " << reply->friend_requests_size() << std::endl;
    std::cout << "event info size" << reply->event_info_size() << std::endl;
    std::cout << "num new friends: " << reply->accepted_friends_size() << std::endl;

    netmsg::AppReply_User usr = reply->users(0);
    std::cout << "user info" << std::endl;
    std::cout << "has phone num : " << usr.has_phone_number() << std::endl;
    std::cout << "has phone num : " << usr.phone_number() << std::endl;
    std::cout << "has nickname: " << usr.has_nickname() << std::endl;
    std::cout << "has nickname: " << usr.nickname() << std::endl;
    std::cout << "has gender: " << usr.has_is_male() << std::endl;
    std::cout << "has gender: " << usr.is_male() << std::endl;
    std::cout << "has photo: " << usr.has_profile_photo() << std::endl;
    std::cout << "has photo: " << usr.profile_photo().length() << std::endl;
    std::cout << "has email: " << usr.has_email() << std::endl;
    std::cout << "has email: " << usr.email() << std::endl;
    std::cout << "has_desc: " << usr.has_description() << std::endl;
    std::cout << "has_desc: " << usr.description() << std::endl;
    std::cout << "has_loc: " << usr.has_location() << std::endl;
    std::cout << "has_loc: " << usr.location() << std::endl;
    
    std::string output_filename = "tmp/retrieved_profile_pic.jpeg";
    std::string photo_bytestr = usr.profile_photo();
    std::ofstream out_file;
    out_file.open(output_filename, std::ios::out | std::ios::binary);
    out_file.write(photo_bytestr.c_str(), photo_bytestr.size());
    out_file.close();

    // Cleanup
    google::protobuf::ShutdownProtobufLibrary();
    requester.disconnect("tcp://localhost:5559");
    return 0;
}
