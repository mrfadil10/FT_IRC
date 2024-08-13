NAME = ircserv

CC = c++

FLAGS = -Wall -Wextra -Werror -std=c++98

SRCS = ./server/server.cpp \
	   ./server/pass.cpp \
	   ./server/user.cpp \
	   ./server/nick.cpp \
	   ./main.cpp

DSYM = ./ircserv.dSYM

$(NAME):
		$(CC) $(FLAGS) $(SRCS) -o $(NAME)

all: $(NAME)

clean:

fclean: clean
	@rm -rf $(NAME)
	@rm -rf $(DSYM)

re: fclean all

