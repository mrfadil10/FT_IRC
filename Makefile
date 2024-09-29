NAME = ircserv

CC = c++

FLAGS = -Wall -Wextra -Werror -std=c++98 -g -fsanitize=address

SRCS = ./server/server.cpp \
	   ./server/pass.cpp \
	   ./server/user.cpp \
	   ./server/nick.cpp \
	   ./server/mode.cpp \
	   ./server/join.cpp \
	   ./server/topic.cpp \
	   ./server/kick.cpp \
	   ./server/part.cpp \
	   ./server/invite.cpp \
	   ./server/pm.cpp \
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

