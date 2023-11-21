#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Server.hpp"


class Channel{
    private:
        std::vector<Client> _operators;
        std::string _pwd;
        std::vector<Client> _regular_users;
        bool invite_mode;
        bool topic_rest;
        bool pass;
        bool limit;
    public:
        void join();
        void kick();
        void invite();
        void topic();
        void mode();
};
#endif