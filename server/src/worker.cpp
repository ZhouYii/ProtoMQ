#include "includes/zhelpers.hpp"
#include "includes/social.pb.cc"

#include <iostream>
#include <fstream>

void handleRequestRegistration(netmsg::AppRequest* msg)
{
    netmsg::AppRequest_RegisterMessage reg_msg = msg->reg_msg();
    std::cout << reg_msg.username() << "  " <<
                 reg_msg.email() << "  " <<
                 reg_msg.secured_password() << std::endl;

}

void handleRequestStatusUpdate(netmsg::AppRequest* msg)
{
    netmsg::AppRequest_StatusUpdate status_msg = msg->status_updates();
    if (status_msg.has_body()) 
    {
        std::string body = status_msg.body();
        int num_photos = status_msg.photos_size();
        for (int photo_idx = 0; photo_idx < num_photos; photo_idx += 1) {
            std::string output_filename = std::to_string(photo_idx) + ".jpeg";
            std::string photo_bytestr = status_msg.photos(photo_idx);
            std::ofstream out_file;
            out_file.open(output_filename, std::ios::out | std::ios::binary);
            out_file.write(photo_bytestr.c_str(), photo_bytestr.size());
            out_file.close();
            std::cout << "String Size:"
                      << std::to_string(photo_bytestr.size()) << std::endl;
        }
        std::cout << "Recieved Body:" << body << std::endl;
    }
    else
    {
        return;
    }
}

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
