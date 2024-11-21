#pragma once
#ifndef	REQUEST_HPP
# define REQUEST_HPP
# include "main.hpp"

class   request{
	public:

		// con/destructor
		request(std::fstream    &reqfile);
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

        void check_save_request_line(std::string line);
        void fix_spaces_in_line(std::string &line);
        void is_valid_method(std::string line);
        void is_valid_uri(std::string &line);
        void is_valid_httpv(std::string line);
        void check_save_headers(std::fstream &reqfile, std::string line);
		void is_valid_header(std::string &line);
		void is_empty(std::string &line);
		int	size_in_bytes(std::map<std::string, std::string> mp);
};

#endif