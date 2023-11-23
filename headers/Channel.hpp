#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Server.hpp"
#include <algorithm>


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
    public:
        std::string get_list_of_users(){
            std::string users;
            std::vector<Client>::iterator it = _users.begin();

            for(;it!=_users.end();it++)
            {
                if (is_operator(*it))
                {
                    users += "@"+it->get_nickname() + " ";
                }
                else
                    users+=it->get_nickname() + " ";
            }
            return users;
        };
        void set_rest(bool rs)
        {
            rest= rs;
        };
        bool get_rest()
        {
            return rest;
        };
        void set_invite_only(bool st)
        {
            this->invite_only = st;
        };
        void remove_pass(){
            _pwd = "";
        };

         bool is_invite_only() const {
        return invite_only;
    };

    bool is_full() const {
        return _users.size() >= limit;
    }

    bool has_password() const {
        return !_pwd.empty();
    }
        Channel(std::string name, std::string pass): name(name) {
            _pwd = pass;
            invite_only = false;
            rest = false;
            is_limited = false;
            limit = INT_MAX;
            i = "-i";
            t = "-t";
            k = "-k";
            l = "-l";
        };
        std::string get_modes()
        {
            return ( i + t + k + o + l);
        }
        void set_mode(std::string mode)
        {
            if (mode[1] =='i')
                i = mode;
            else if(mode[1] == 't' )
                t =mode;
            else if (mode[1] =='k')
                k = mode;
            else if (mode[1]=='l')
                l = mode;
        };

        int channel_size(){
            return _users.size();
        };
        void add_client_to_channnel(Client &client){
            _users.push_back(client);
        };
        void add_operator(Client &client)
        {
            _operators.push_back(client);
        };
        void remove_operator(Client &client)
    {
        std::vector<Client>::iterator it = _operators.begin();
        if (it != _operators.end())
        {
            for(;it!=_operators.end();it++)
            {
                if (it->get_nickname()== client.get_nickname())
                    _operators.erase(it);
            }
        }
    }
        bool is_invited(Client &client)
        {
            std::vector<Client>::iterator it = _invites.begin();
            if (it!=_invites.end())
            {
                for(;it!=_invites.end(); it++)
                {
                    if (it->get_nickname() == client.get_nickname())
                        return true;
                }
            }
            return false;
        }
         bool is_operator(Client &client)
        {
            std::vector<Client>::iterator it = _operators.begin();
            if (it!=_operators.end())
            {
                for(;it!=_operators.end(); it++)
                {
                    if (it->get_nickname() == client.get_nickname())
                        return true;
                }
            }
            return false;
        }
        
        void broadcast_message(Client &client, std::string message,int flag)
        {
            // std::string cmd, target, message, to_send;
            // std::istringstream iss(client.getMessage());
            // iss>>cmd>>target>>std::ws;
            // std::getline(iss, message);
            // to_send = RPL_PRIVMSG(client.get_nickname(), client.get_user(), this->get_name(), message);
            std::vector<Client>::iterator it = _users.begin();
            if (it !=_users.end())
            {
                for(;it!= _users.end();it++)
                {
                    if (flag == 0)
                        send(it->getSocket(), message.c_str(), message.length(), 0);
                    else{
                        if (client.get_nickname() != it->get_nickname())
                            send(it->getSocket(), message.c_str(), message.length(), 0);
                    }
                }
            }
        }
        bool in_channel(Client & client)
        {
            std::vector<Client>::iterator it = _users.begin();
            if (it!= _users.end())
            {
                for(; it!=_users.end(); it++)
                {
                    if (it->get_nickname() == client.get_nickname())
                        return true;
                }
            }
            return false;
        };
        std::string get_name(){
            return this->name;
        };
        std::string get_pass()
        {
            return this->_pwd;
        };
        void set_name(std::string nm)
        {
            this->name = nm;
        };
        void set_pass(std::string pwd)
        {
            this->_pwd = pwd;
        };
        void set_limit(int lmt)
        {
            this->limit = lmt;
        };
        void remove_limit()
        {
            limit = INT_MAX;
        }
        int get_limit()
        {
            return this->limit;
        }
        // void kick();
        // void invite();
        // void topic();
        // void mode();
};
#endif