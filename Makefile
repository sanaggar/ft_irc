SRCS =	Server.cpp \
		serverValidate.cpp \
		functions.cpp \
		main.cpp \
		User.cpp \
		Channel.cpp \
		send_codes.cpp \
		commands/pong.cpp \
		commands/mode.cpp \
		commands/part.cpp \
		commands/topic.cpp \
		commands/kick.cpp \
		commands/invit.cpp \
		commands/join.cpp \
		commands/privmsg.cpp \
		commands/nick.cpp \


NAME = ircserv

CC = c++

FLAGS = -Wall -Werror -Wextra -std=c++98

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(FLAGS) -o $(NAME) $(OBJS)

%.o: %.cpp
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all re clean fclean