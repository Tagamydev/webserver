#pragma once
#ifndef	REQUEST_HPP
# define REQUEST_HPP
# include "main.hpp"

#define MAX_URI_LENGTH 4096
#define MAX_CONTENT_LENGTH 30000000

enum ParsingState
{

};

class   request{
	public:

		// con/destructor
		request(int fd);
		~request(void);

	// FIRST LINE

		// QUERY
		std::map<std::string, std::string>	_query_str;	
		// key, value

		// METHOD: GET, POST, DELETE
		std::string	_method;

		std::string	_uri;
		std::string	_http_version;

	// HEADERS
		std::map<std::string, std::string>	_headers;	
		// header, content

	// BODY
		std::string	_body;
		int		_body_length;
		bool		_has_body;
        bool        _chunked_flag;

	// STATES FOR RESPONSE
		ParsingState	_state;
		int				_error_code;


        void check_save_request_line(std::string line);
        void fix_spaces_in_line(std::string &line);
        void is_valid_method(std::string line);
        void is_valid_uri(std::string &line);
        void is_valid_httpv(std::string line);
        void check_save_headers(std::stringstream &reqfile, std::string line);
		void is_valid_header(std::string &line);
		void is_empty(std::string &line);
		void handle_headers();
};

#endif