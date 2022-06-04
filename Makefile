NAME = webserv

SRC = main.cpp Networking/Server.cpp Networking/ServerGroup.cpp HTTP/Cgi.cpp HTTP/Cgi_setters.cpp HTTP/Conf.cpp HTTP/Conf_getters.cpp HTTP/Conf_setters.cpp HTTP/HttpRequest.cpp HTTP/Location.cpp HTTP/Location_setters.cpp HTTP/Parse_server.cpp HTTP/Response.cpp HTTP/Servers.cpp HTTP/get_filetype.cpp      

CC = clang++  -std=c++98

FLAGS = -Wall -Wextra -Werror

CONF= conf

all : $(NAME)

$(NAME) : $(SRC)
	$(CC) $(FLAGS) $(SRC) -o $(NAME)

clean :
	rm -rf $(NAME)
run :
	./$(NAME) 	$(CONF)
fclean : clean

re : clean all