#ifndef CHANNEL_HPP
#define CHANNEL_HPP
#include "Client.hpp"
#include <algorithm>

#define MAX_CLIENTS 100


class Channel{
    private:
        std::string _topic;
        std::string name;
        std::vector<Client> _operators;
        std::string _pwd;
        std::vector<Client> _users;
        std::vector<Client> _invites;
        std::string i , t, k, o , l;
        int limit;
        bool is_limited;
        bool invite_only;
        bool rest;
        bool is_topic_protected;
    public:
        Channel();
        Channel(std::string name, std::string pass);
        void set_topic_protected(bool state);
        bool get_topic_state();
        void set_topic(std::string topic);
        std::string get_topic();
        std::string get_list_of_users();
        void set_rest(bool rs);
        bool get_rest();
        void set_invite_only(bool state);
        bool is_invite_only() const ;
        bool is_full() const ;
        bool has_password() const ;
        std::string get_modes();
        void set_mode(std::string mode);
        int channel_size();
        void add_client_to_channnel(Client &client);
        void remove_client_from_channel(Client &client);
        void add_operator(Client &client);
        void remove_operator(Client &client);
        void add_invited(Client &client);
        bool is_invited(Client &client);
        bool is_operator(Client &client);
        void broadcast_message(Client &client, std::string message,int flag);
        bool in_channel(Client & client);
        std::string get_name();
        std::string get_pass();
        void set_name(std::string nm);
        void set_pass(std::string pwd);
        void set_limit(int lmt);
        int get_limit();
        void set_lim_state(bool state);
        bool get_lim_state();
};
#endif