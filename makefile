NAME = ircserv

SRCS = main.cpp ./src/Server.cpp ./src/Channel.cpp ./src/Client.cpp 

HEADERS = ./headers/Server.hpp  ./headers/Channel.hpp ./headers/head.hpp ./headers/Client.hpp  
OBJS = $(SRCS:.cpp=.o)

FLAGS = -Wall -Wextra -Werror -std=c++98

CC = g++

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp $(HEADERS)
	$(CC) $(FLAGS) -c $< -o $@
clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all