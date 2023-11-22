#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Server.hpp"


class Channel{
    private:
        std::string name;
        std::vector<Client> _operators;
        std::string _pwd;
        std::vector<Client> _users;
        std::vector<Client> _invites;
        std::string i , t, k, o , l;
        int limit;
        bool invite_only ;
    public:
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
        
        void broadcast_message(Client &client)
        {
            std::string cmd, target, message, to_send;
            std::istringstream iss(client.getMessage());
            iss>>cmd>>target>>std::ws;
            std::getline(iss, message);
            to_send = user_forma(client.get_nickname(), client.get_user() , inet_ntoa(client.getAddress().sin_addr));
            to_send += " " + message + "\r\n";
            std::vector<Client>::iterator it = _users.begin();
            if (it !=_users.end())
            {
                for(;it!= _users.end();it++)
                {
                    if ( client.get_nickname() != it->get_nickname())
                        send(it->getSocket(), to_send.c_str(), to_send.length(), 0);
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