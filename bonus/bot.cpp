#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <fcntl.h>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <ctime> 
#include <arpa/inet.h>
#include <netdb.h>

void handleQuotes(int socket)
{
    const char* quotes[] = {
        "The only way to do great work is to love what you do. - Steve Jobs",
        "Success is not final, failure is not fatal: It is the courage to continue that counts. - Winston Churchill",
        "Life is what happens when you're busy making other plans. - John Lennon",
        "In three words I can sum up everything I've learned about life: it goes on. - Robert Frost",
        "The future belongs to those who believe in the beauty of their dreams. - Eleanor Roosevelt",
        "It always seems impossible until it's done. - Nelson Mandela",
        "Don't count the days, make the days count. - Muhammad Ali",
        "Life is really simple, but we insist on making it complicated. - Confucius",
        "The only limit to our realization of tomorrow will be our doubts of today. - Franklin D. Roosevelt",
        "The purpose of our lives is to be happy. - Dalai Lama",
        "Life is 10% what happens to us and 90% how we react to it. - Charles R. Swindoll",
        "Believe you can and you're halfway there. - Theodore Roosevelt",
        "In the end, we will remember not the words of our enemies, but the silence of our friends. - Martin Luther King Jr.",
        "To be yourself in a world that is constantly trying to make you something else is the greatest accomplishment. - Ralph Waldo Emerson",
        "The only person you are destined to become is the person you decide to be. - Ralph Waldo Emerson",
        "It is our choices that show what we truly are, far more than our abilities. - J.K. Rowling",
        "The best way to predict the future is to create it. - Peter Drucker",
        "The only impossible journey is the one you never begin. - Tony Robbins",
        "Happiness can exist only in acceptance. - George Orwell",
        "Life is short, and it's up to you to make it sweet. - Sarah Louise Delany",
        "The purpose of our lives is to be happy. - Dalai Lama",
        "I can't change the direction of the wind, but I can adjust my sails to always reach my destination. - Jimmy Dean",
        "Life is 10% what happens to us and 90% how we react to it. - Charles R. Swindoll",
        "You are never too old to set another goal or to dream a new dream. - C.S. Lewis",
        "The only way to do great work is to love what you do. - Steve Jobs",
        "Success is not final, failure is not fatal: It is the courage to continue that counts. - Winston Churchill",
        "Life is what happens when you're busy making other plans. - John Lennon",
        "In three words I can sum up everything I've learned about life: it goes on. - Robert Frost",
        "The future belongs to those who believe in the beauty of their dreams. - Eleanor Roosevelt",
        "It always seems impossible until it's done. - Nelson Mandela",
        "Don't count the days, make the days count. - Muhammad Ali",
        "Life is really simple, but we insist on making it complicated. - Confucius",
        "The only limit to our realization of tomorrow will be our doubts of today. - Franklin D. Roosevelt",
        "The purpose of our lives is to be happy. - Dalai Lama",
        "Life is 10% what happens to us and 90% how we react to it. - Charles R. Swindoll",
        "Believe you can and you're halfway there. - Theodore Roosevelt",
        "In the end, we will remember not the words of our enemies, but the silence of our friends. - Martin Luther King Jr.",
        "To be yourself in a world that is constantly trying to make you something else is the greatest accomplishment. - Ralph Waldo Emerson",
        "The only person you are destined to become is the person you decide to be. - Ralph Waldo Emerson",
        "It is our choices that show what we truly are, far more than our abilities. - J.K. Rowling",
        "The best way to predict the future is to create it. - Peter Drucker",
        "The only impossible journey is the one you never begin. - Tony Robbins",
        "Happiness can exist only in acceptance. - George Orwell",
        "Life is short, and it's up to you to make it sweet. - Sarah Louise Delany",
        "The purpose of our lives is to be happy. - Dalai Lama",
        "I can't change the direction of the wind, but I can adjust my sails to always reach my destination. - Jimmy Dean",
        "Life is 10% what happens to us and 90% how we react to it. - Charles R. Swindoll",
        "You are never too old to set another goal or to dream a new dream. - C.S. Lewis",
        "The only way to do great work is to love what you do. - Steve Jobs",
        "Success is not final, failure is not fatal: It is the courage to continue that counts. - Winston Churchill",
        "Life is what happens when you're busy making other plans. - John Lennon",
        "In three words I can sum up everything I've learned about life: it goes on. - Robert Frost",
        "The future belongs to those who believe in the beauty of their dreams. - Eleanor Roosevelt",
        "It always seems impossible until it's done. - Nelson Mandela",
        "Don't count the days, make the days count. - Muhammad Ali",
        "Life is really simple, but we insist on making it complicated. - Confucius",
        "The only limit to our realization of tomorrow will be our doubts of today. - Franklin D. Roosevelt",
        "The purpose of our lives is to be happy. - Dalai Lama",
        "Life is 10% what happens to us and 90% how we react to it. - Charles R. Swindoll",
        "Believe you can and you're halfway there. - Theodore Roosevelt",
        "In the end, we will remember not the words of our enemies, but the silence of our friends. - Martin Luther King Jr.",
        "To be yourself in a world that is constantly trying to make you something else is the greatest accomplishment. - Ralph Waldo Emerson",
        "The only person you are destined to become is the person you decide to be. - Ralph Waldo Emerson",
        "It is our choices that show what we truly are, far more than our abilities. - J.K. Rowling",
        "The best way to predict the future is to create it. - Peter Drucker",
        "The only impossible journey is the one you never begin. - Tony Robbins",
        "Happiness can exist only in acceptance. - George Orwell",
        "Life is short, and it's up to you to make it sweet. - Sarah Louise Delany",
    };
    std::srand(std::time(0));
    int random = std::rand() % 68;
    std::string to_send(quotes[random]);
    to_send +="\r\n";
    send(socket, to_send.c_str(), to_send.length(), 0);
}
int main(int argc, char** argv)
{
  if(argc == 4)
    {
        struct addrinfo *res, hints;
        int status;
        int bot_socket = socket(AF_INET, SOCK_STREAM, 0);
        if(bot_socket == -1)
        {
            std::cerr << "Failed to create socket. errno: " << errno << std::endl;
            exit(EXIT_FAILURE);
        }
        int opt = 1;
        if (setsockopt(bot_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))==-1)
		{
			perror("setsockopt");
			exit(EXIT_FAILURE);
		}
        if (fcntl(bot_socket, F_SETFD, O_NONBLOCK) ==-1)
		{
			perror("fcntl");
			exit(EXIT_FAILURE);
		}
        sockaddr_in sockaddr;
        memset(&hints, 0, sizeof(sockaddr));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
     if((status = getaddrinfo(argv[1], argv[2], &hints, &res)) != 0)
{
    std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
    exit(EXIT_FAILURE);
}

        if(connect(bot_socket, res->ai_addr, res->ai_addrlen) == -1)
        {
            std::cerr << "connect() failed. errno: " << errno << std::endl;
            exit(EXIT_FAILURE);
        }

        std::cout << "Connected to the server!" << std::endl;
        //authenticated to the server
        std::string pass(argv[3]);
        std::string nick("BOTE");
        std::string user("bote 0 * boted");
        pass = "PASS "+ pass + "\r\n";
        pass += "NICK " + nick + "\r\n";
        pass += "USER " + user + "\r\n";
        send(bot_socket, pass.c_str(), pass.length(), 0);
        char buffer[256];
        while(true)
        {
            memset(buffer, 0, sizeof(buffer));
            int byte = recv(bot_socket, buffer, sizeof(buffer), 0);
            if (byte==-1){
                std::cout<<"failed"<<std::endl;
                break;
            }
            else if(byte == 0)
            {
                std::cout<<"server disconnected"<<std::endl;
                break;
            }
            else
            {
                buffer[byte] = '\0';
                std::string str(buffer);
                if (str == "KNOCK")
                    handleQuotes(bot_socket);
            }
        }
        freeaddrinfo(res);
        res = NULL;
    }
    else
    {
        std::cerr << "Usage: " << argv[0] << " <IP> <PORT>" << std::endl;
    }

    return 0;
}