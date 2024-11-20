#pragma once
#ifndef	REQUEST_HPP
# define REQUEST_HPP
# include "main.hpp"

class   request{
	public:

		// con/destructor
		request(int fd);
		~request(void);

	// FIRST LINE

		// QUERY
		std::map<std::string, std::string>	query_str;	
		// key, value

		// METHOD: GET, POST, DELETE
		std::string	method;

		std::string	uri;
		std::string	http_version;

	// HEADERS
		std::map<std::string, std::string>	headers;	
		// header, content

	// BODY
		std::string	body;

        int check_startline(std::string line);
        void fix_spaces_in_line(std::string &line);
};

#endif