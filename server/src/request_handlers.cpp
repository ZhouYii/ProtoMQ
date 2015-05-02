#ifndef _REQ_H_CPP
#define _REQ_H_CPP

#include "includes/zhelpers.hpp"
#include "includes/social.pb.cc"

#include <iostream>
#include <fstream>
#include <cassandra.h>

// Request handlers are responsible for unpacking message contents

void HandleRequestLogin(netmsg::AppRequest* msg) 
{
    netmsg::AppRequest_LoginMessage login_msg = msg->login_msg();
    if (login_msg.has_secured_password()) 
    {
        std::string password = login_msg.secured_password();
        if (login_msg.has_email()) 
        {
            std::string email = login_msg.email();
            // Login with email
            std::cout << "Login Email:" << email ;
            std::cout << " Pass:" << password << std::endl;
        } else {
            int64_t phone_id = msg->phone_id();
            // Login with phone id. Currently only way.
            std::cout << "Login Phone:" << phone_id ;
            std::cout << " Pass:" << password << std::endl;
        }

    } else {
        // Login failure
        return;
    }
}

void HandleRequestRegistration(netmsg::AppRequest* msg)
{
    netmsg::AppRequest_RegisterMessage reg_msg = msg->reg_msg();
    std::cout << reg_msg.username() << "  " <<
                 reg_msg.email() << "  " <<
                 reg_msg.secured_password() << std::endl;

    // Validate email or something + check if it's zero-length string.

}

// Request handler for event creation
void handleRequestCreateEvent(CassSession* session, netmsg::AppRequest* msg) {
    netmsg::AppRequest_CreateEvent create_event = msg->create_event();
    CassError casserr;
    int64_t host_id;
    int64_t posix_time;
    int64_t user_id;
    std::string title;
    std::string location;
    int num_invited_users;

    // Validate message
    if (create_event.has_host_id() && create_event.has_title() &&
            create_event.has_location() && create_event.has_time()) 
    {
        host_id = create_event.host_id();
        posix_time = create_event.time();
        title = create_event.title();
        location = create_event.location();
        casserr = DbCreateNewEvent(session, host_id, posix_time, title, location);
        // TODO:Logging

        /*
        num_invited_users = create_event.invited_users_size();
        if (num_invited_users > 0) {
            for (int user_idx = 0; user_idx < num_invited_users; user_idx += 1) {
                user_id = create_event.invited_users(user_idx);
            }
        }
        */
    } else {
        std::cout << "Ill formed new event query" << std::endl;
        // Input were not correct
        // Send error response
    }
}

void handleRequestStatusUpdate(netmsg::AppRequest* msg)
{
    netmsg::AppRequest_StatusUpdate status_msg = msg->status_updates();
    if (status_msg.has_body()) 
    {
        std::string body = status_msg.body();
        int num_photos = status_msg.photos_size();
        for (int photo_idx = 0; photo_idx < num_photos; photo_idx += 1) {
            std::string output_filename = "tmp/" + std::to_string(photo_idx) + ".jpeg";
            std::string photo_bytestr = status_msg.photos(photo_idx);
            std::ofstream out_file;
            out_file.open(output_filename, std::ios::out | std::ios::binary);
            out_file.write(photo_bytestr.c_str(), photo_bytestr.size());
            out_file.close();
        }
    }
    else
    {
        return;
    }
}


#endif
