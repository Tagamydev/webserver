#pragma once
#ifndef	MAIN_HPP
# define MAIN_HPP

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
# include "cgi.hpp"
# include "location.hpp"
# include "server.hpp"
# include "request.hpp"
# include "response.hpp"
# include "serverFd.hpp"
# include "webserver.hpp"
# include "utils.hpp"
# include "defaults.hpp"
# include "loopHandler.hpp"
# include <algorithm>    // std::count
# include <set>

std::string get_actual_date();

#endif