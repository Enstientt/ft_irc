#include "../headers/Client.hpp"

   Client::Client(int socket, const sockaddr_in& address) : socket(socket), address(address) {
    }

    int Client::getSocket() const {
        return socket;
    }

    const sockaddr_in& Client::Client::getAddress() const {
        return address;
    }

    void Client::setMessage(std::string message){
        this->message = message;
    }
    const std::string Client::getMessage(){
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
    void Client::set_user(std::string username, std::string hostName,std::string serverName, std::string realName)
    {
        this->realname = realName;
        this->userName = username;
        this->hostName = hostName;

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