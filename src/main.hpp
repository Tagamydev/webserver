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
# include <sys/stat.h>
# include "location.hpp"
# include "server.hpp"
# include "request.hpp"
# include "response.hpp"

std::string get_actual_date();
#endif