#ifndef _REQ_H_CPP
#define _REQ_H_CPP

#include "includes/zhelpers.hpp"
#include "includes/social.pb.cc"

#include <iostream>
#include <fstream>

void handleRequestLogin(netmsg::AppRequest* msg) 
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

void handleRequestRegistration(netmsg::AppRequest* msg)
{
    netmsg::AppRequest_RegisterMessage reg_msg = msg->reg_msg();
    std::cout << reg_msg.username() << "  " <<
                 reg_msg.email() << "  " <<
                 reg_msg.secured_password() << std::endl;

    // Validate email or something + check if it's zero-length string.

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
