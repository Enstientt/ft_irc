NAME = ircserv

SRCS = main.cpp ./src/Server.cpp ./src/Channel.cpp ./src/Client.cpp 

SRCS_B = ./bonus/bot.cpp

HEADERS = ./headers/Server.hpp  ./headers/Channel.hpp ./headers/head.hpp ./headers/Client.hpp  

OBJS = $(SRCS:.cpp=.o)

OBJS_B = $(SRCS_B:.cpp=.o)

FLAGS = -Wall -Wextra -Werror -std=c++98

CC = c++

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp $(HEADERS)
	$(CC) $(FLAGS) -c $< -o $@

bonus:$(OBJS_B)
	$(CC) $(FLAGS) bonus/bot.cpp -o bot

clean:
	rm -rf $(OBJS) $(OBJS_B)

fclean: clean
	rm -rf $(NAME) bot

re: fclean all