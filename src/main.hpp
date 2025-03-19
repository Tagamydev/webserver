#pragma once
#ifndef	MAIN_HPP
# define MAIN_HPP

// Color codes (add on header?)
# define RESET   "\033[0m"
# define BLUE "\033[34m"
# define GREEN "\033[32m"
# define RED "\033[31m"
# define YELLOW "\033[0;33m"
# define PURPLE "\033[0;35m"
# define CYAN "\033[0;36m"

# include <string>
# include <iostream>
# include <fstream>
# include <sstream> 
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <fcntl.h>
# include <cstdio>    
# include <map>
# include <list>
# include <vector>
# include <dirent.h>
# include <poll.h>
# include <cstring>
# include <cstdlib>
# include <sys/socket.h>
# include <netinet/in.h>
# include <sys/stat.h>
# include <signal.h>
# include "cgi.hpp"
# include "client.hpp"
# include "location.hpp"
# include "server.hpp"
# include "request.hpp"
# include "utils.hpp"
# include "response.hpp"
# include "serverFd.hpp"
# include "webserver.hpp"
# include "defaults.hpp"
# include "client.hpp"
# include "loopHandler.hpp"
# include <algorithm>    // std::count
# include <set>
# include <sys/wait.h>

std::string get_actual_date();

#endif
