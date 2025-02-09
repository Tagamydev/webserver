
# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: samusanc <samusanc@student.42madrid>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/06/24 19:28:25 by samusanc          #+#    #+#              #
#    Updated: 2025/01/28 17:23:10 by samusanc         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		=	result
INC_DIR		=	-I ./src/ \
				-I ./src/server/ \
				-I ./src/client/ \
				-I ./src/cgi/ \
				-I ./src/request/ \
				-I ./src/utils/ \
				-I ./src/loop/ \
				-I ./src/response/ 

CXXFLAGS	=	-g3 -fsanitize=address -g3#-Wall -Wextra -Werror -I ./
CXXFLAGS	+= $(INC_DIR)
CXXFLAGS	+=	-std=c++98
CXX		=	c++ $(CXXFLAGS)
MAIN		=	./main.cpp
INC		= ./src/main.hpp \
		./src/cgi/cgi.hpp \
		./src/client/client.hpp \
		./src/utils/utils.hpp \
		./src/loop/loopHandler.hpp \
		./src/response/response.hpp \
		./src/request/request.hpp \
		./src/server/location.hpp \
		./src/server/server.hpp \
		./src/server/webserver.hpp \
		./src/server/serverFd.hpp \

SRCS		= ./src/response/response.cpp \
		./src/request/request.cpp \
		./src/client/client.cpp \
		./src/loop/loopHandler.cpp \
		./src/cgi/cgi.cpp \
		./src/utils/utils.cpp \
		./src/server/server.cpp \
		./src/server/webserver.cpp \
		./src/server/serverFd.cpp \
		./src/server/location.cpp \
		./src/main.cpp 

O_DIR		=	./objects/
OBJS		=	$(addprefix $(O_DIR)/, $(SRCS:.cpp=.o))

$(O_DIR)/%.o: %.cpp
	mkdir -p $(@D)
	$(CXX) -c $< -o $(O_DIR)/$(<:.cpp=.o)

all: $(NAME) $(SRCS)

$(NAME): $(OBJS) $(INC)
	$(CXX) $(OBJS) -o $(NAME)

re: fclean all

fclean: clean
	@rm -f $(NAME)
	@rm -rf objects

clean:
	@rm -f $(OBJS)

.PHONY: all
