#ifndef _REQ_H_CPP
#define _REQ_H_CPP
#include "request_handlers.h"
#include "db/db_lib.h"

void HandleRequestLogin(CassSession* session, 
                        netmsg::AppRequest* msg) 
{
    netmsg::AppRequest_LoginMessage login_msg = msg->login_msg();
    if (login_msg.has_password_hash()) 
    {
        std::string password = login_msg.password_hash();
        int64_t phone_id = msg->phone_id();
        // Login with phone id. Currently only way.
        std::cout << "Login Phone:" << phone_id ;
        std::cout << " Pass:" << password << std::endl;
    } else {
        // Login failure
        return;
    }
}


void HandleRequestEventInvite(CassSession* session,
                               netmsg::AppRequest* msg)
{
    netmsg::AppRequest_EventInvite invite_req = msg->invite_event();
    int num_invited = invite_req.invited_users_size();
    CassUuid cass_uuid;

    if (invite_req.has_event_uuid1() && num_invited > 0) {
        const std::string& uuid1 = invite_req.event_uuid1();
        cass_uuid_from_string(uuid1.c_str(), &cass_uuid);

        // Initialize array of invited ids
        int64_t invited_id;
        for (int usr_idx = 0; usr_idx < num_invited; usr_idx += 1) {
            invited_id = invite_req.invited_users(usr_idx);
            std::cout << "invite id" << invited_id << std::endl;
            DbSingleUserAttendingEvent(session, &cass_uuid, invited_id);
            std::cout << "here" << std::endl;
        }
    } // TODO failed case prepare message to send back?
}


void HandleRequestRegistration(CassSession* session,
                               netmsg::AppRequest* msg)
{
    netmsg::AppRequest_RegisterMessage reg_msg = msg->reg_msg();
    std::cout << reg_msg.password_hash() << "  " <<
                 reg_msg.nickname() << std::endl;

    // Validate email or something + check if it's zero-length string.

}

netmsg::AppReply* HandleRequestGetUserInfo(CassSession* session,
                               netmsg::AppRequest* msg,
                               int64_t host_id,
                               zmq::socket_t* responder)
{
    netmsg::AppReply* reply = new netmsg::AppReply;
    reply->set_response_type(netmsg::AppReply_ResponseType_tSuccess);
    for (int user_idx = 0;
         user_idx < msg->request_user_info_size();
         user_idx += 1)
    {
        int64_t phone_id = msg->request_user_info(user_idx);
        netmsg::AppReply_User* new_user = reply->add_users();
        bool success = db_populate_reply_user_object(session, phone_id, new_user);
        if (!success) {
            new_user->set_phone_number(-1);
        }
    }

    return reply;
}

// Request handler for event creation
void HandleRequestUpdateProfile(CassSession* session, 
                               netmsg::AppRequest* msg,
                               int64_t host_id) 
{
    netmsg::AppRequest_UpdateProfile prof_msg = msg->update_profile();
    std::cout << "update profile handler" << std::endl;
    // Update profile picture
    if (prof_msg.has_profile_photo()) {
        std::string bytestr = prof_msg.profile_photo();
        size_t num_char = bytestr.length();
        db_update_profile_pic(session, host_id, bytestr.c_str(), num_char);
    }

}

// Request handler for event creation
void HandleRequestCreateEvent(CassSession* session, 
                              netmsg::AppRequest* msg,
                              int64_t host_id) {
    std::cout << "entered" << std::endl;
    netmsg::AppRequest_CreateEvent create_event = msg->create_event();
    CassUuid cass_uuid;
    int64_t posix_time;
    int64_t user_id;
    const char* title;
    const char* uuid1;
    const char* location;
    char uuid_holder[CASS_UUID_STRING_LENGTH];
    int num_invited_users;

    std::cout << "entered" << std::endl;
    // Validate message
    if (create_event.has_title() &&
            create_event.has_location() && create_event.has_time()) 
    {
        std::cout << "entered2" << std::endl;
        posix_time = create_event.time();
        title = create_event.title().c_str();
        location = create_event.location().c_str();
        uuid1 = create_event.event_uuid1().c_str();
        cass_uuid = DbCreateNewEvent(session, host_id, posix_time, 
                                     title, location, uuid1);
        // TODO:Logging ?

        // TODO : this is actually an error, the collection is updated when the event request is accepted
        std::cout << "cass uuid " << uuid1 << std::endl;
        num_invited_users = create_event.invited_users_size();
        if (num_invited_users > 0) {
            int64_t invited_id;
            for (int user_idx = 0; user_idx < num_invited_users; user_idx += 1) {
                invited_id = create_event.invited_users(user_idx);
                std::cout << "invite id" << invited_id << std::endl;
                DbSingleUserAttendingEvent(session, &cass_uuid, invited_id);
                std::cout << "invite id" << invited_id << std::endl;
            }
        }
    } else {
        std::cout << "Ill formed new event query" << std::endl;
        // Input were not corret
        // Send error response
    }
}

/*
void HandleRequestStatusUpdate(CassSession* session, 
                               netmsg::AppRequest* msg)
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
*/

#endif
