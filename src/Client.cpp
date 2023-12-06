#include "../headers/Client.hpp"

Client::Client(){};

Client::Client(int socket, const sockaddr_in &address) : socket(socket), address(address)
{
}

int Client::getSocket() const
{
    return socket;
}

const sockaddr_in &Client::Client::getAddress() const
{
    return address;
}

void Client::addtosetMessage(std::string message)
{
    this->message += message;
}

void Client::setMessage(std::string message)
{
    this->message = message;
}
const std::string Client::getMessage()
{
    return this->message;
}
bool Client::auth()
{
    return is_authenticate;
}
void Client::setAuth(bool at)
{
    this->is_authenticate = at;
}
int Client::getSocket()
{
    return this->socket;
}
void Client::set_nickName(std::string nick)
{
    this->nickName = nick;
}
std::string Client::get_nickname()
{
    return this->nickName;
}
void Client::set_reg(bool reg){
    this->is_registered = reg;
}
bool Client::get_reg() const{
    return this->is_registered;
}
void Client::set_username(std::string username)
{
    this->userName = username;
}
void Client::set_realname(std::string realName)
{
    this->realname = realName;
}

std::string Client::get_realname() const{
    return this->realname;
}
std::string Client::get_username() const{
    return this->userName;
}
std::string Client::get_user()
{
    return this->realname;
}
std::string Client::get_pwd()
{
    return this->pwd;
}

void Client::set_pwd(std::string pwd)
{
    this->pwd = pwd;
}

void Client::set_channels(std::string channel)
{
    _channels.push_back(channel);
}

std::vector<std::string> &Client::get_channels()
{
    return this->_channels;
}