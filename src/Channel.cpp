#include "../headers/Channel.hpp"

Channel::Channel(){
    
}
 void Channel::set_topic_protected(bool state)
        {
            this->is_topic_protected = state;
        };
std::string Channel::get_list_of_users(){
    std::string users;
    std::vector<Client>::iterator it = _users.begin();
    
    for(;it!=_users.end();it++){
        if (is_operator(*it))
            users += "@"+it->get_nickname() + " ";
        else
            users+=it->get_nickname() + " ";
        }
        return users;
}
void Channel::set_rest(bool rs)
        {
            rest= rs;
        };
bool Channel::get_rest()
        {
            return rest;
        };
void Channel::set_invite_only(bool state)
        {
            this->invite_only = state;
        };
bool Channel::is_invite_only() const {
        return invite_only;
    };

bool Channel::is_full() const {
        return _users.size() >= limit;
    }

bool Channel::has_password() const {
        return !_pwd.empty();
    }
Channel::Channel(std::string name, std::string pass): name(name) {
            _pwd = pass;
            invite_only = false;
            rest = false;
            is_limited = false;
            limit = MAX_CLIENTS;
            i = "-i";
            t = "-t";
            k = "-k";
            l = "-l";
        };
std::string Channel::get_modes()
{
    return ( i + t + k + o + l);
}
void Channel::set_mode(std::string mode)
{
    if (mode[1] == 'i')
        i = mode;
    else if (mode[1] == 't')
        t = mode;
    else if (mode[1] == 'k')
        k = mode;
    else if (mode[1] == 'l')
        l = mode;
};

int Channel::channel_size()
{
    return _users.size();
};
void Channel::add_client_to_channnel(Client &client)
{
    _users.push_back(client);
};
void Channel::add_operator(Client &client)
{
    _operators.push_back(client);
};
void Channel::remove_operator(Client &client)
{
    std::vector<Client>::iterator it = _operators.begin();
    if (it != _operators.end())
    {
        for (; it != _operators.end(); it++)
        {
            if (it->get_nickname() == client.get_nickname())
            {
                _operators.erase(it);
                break;
            }
        }
    }
}
bool Channel::is_invited(Client &client)
{
    std::vector<Client>::iterator it = _invites.begin();
    if (it != _invites.end())
    {
        for (; it != _invites.end(); it++)
        {
            if (it->get_nickname() == client.get_nickname())
                return true;
        }
    }
    return false;
}
bool Channel::is_operator(Client &client)
{
    std::vector<Client>::iterator it = _operators.begin();
    if (it != _operators.end())
    {
        for (; it != _operators.end(); it++)
        {
            if (it->get_nickname() == client.get_nickname())
                return true;
        }
    }
    return false;
}

void Channel::broadcast_message(Client &client, std::string message, int flag)
{
    std::vector<Client>::iterator it = _users.begin();
    if (it != _users.end())
    {
        for (; it != _users.end(); it++)
        {
            if (flag == 0)
                send(it->getSocket(), message.c_str(), message.length(), 0);
            else
            {
                if (client.get_nickname() != it->get_nickname())
                    send(it->getSocket(), message.c_str(), message.length(), 0);
            }
        }
    }
}
bool Channel::in_channel(Client &client)
{
    std::vector<Client>::iterator it = _users.begin();
    if (it != _users.end())
    {
        for (; it != _users.end(); it++)
        {
            if (it->get_nickname() == client.get_nickname())
                return true;
        }
    }
    return false;
};
std::string Channel::get_name()
{
    return this->name;
};
std::string Channel::get_pass()
{
    return this->_pwd;
};
void Channel::set_name(std::string nm)
{
    this->name = nm;
};
void Channel::set_pass(std::string pwd)
{
    this->_pwd = pwd;
};
void Channel::set_limit(int lmt)
{
    this->limit = lmt;
};
int Channel::get_limit()
{
    return this->limit;
}
void Channel::set_lim_state(bool state)
{
    this->is_limited = state;
}

bool Channel::get_lim_state()
{
    return this->is_limited ;
}

void Channel::add_invited(Client &client)
{
    _invites.push_back(client);
}

void Channel::remove_client_from_channel(Client &client)
{
    std::vector<Client>::iterator it = _users.begin();
    if (it!=_users.end())
    {
        for(;it!=_users.end();it++)
        {
            if (it->get_nickname()==client.get_nickname())
            {
                _users.erase(it);
                break;
            }
        }
    }
}