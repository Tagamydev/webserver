#pragma once
#ifndef	REQUEST_HPP
# define REQUEST_HPP
# include "main.hpp"

# define MAX_URI_LENGTH 4096
# define MAX_CONTENT_LENGTH 30000000
# define MAX_BUFFER_LENGTH 2048

enum ParsingState
{

};

class   request{
	public:

		// con/destructor
		request(int fd);
		~request(void);

		/* Copy constructor*/
		request(const request& other);

		/* Copy assignment operator overload */
		request& operator = (const request& other);

	// FIRST LINE

		// QUERY
		// std::map<std::string, std::string>	_query_str;	
		// key, value
		int	_fd;
		// METHOD: GET, POST, DELETE
		std::string	_method;
		std::string	_uri;
		std::string	_uri_file;
		std::string	_uri_params;

		std::string	_http_version;

	// HEADERS
		std::map<std::string, std::string>	_headers;	
		// header, content

	// BODY
		std::string	_body;
		int		_body_length;
		int		_has_body;
        bool        _chunked_flag;

	// STATES FOR RESPONSE
		int				_error_code;
		std::string		_debug_msg;

        // void fix_spaces_in_line(std::string &line);
        void check_save_request_line(std::string line);
        void is_valid_method(std::string line);
        void is_valid_uri(std::string &line);
        void process_uri(std::string line);
        void is_valid_httpv(std::string line);
        void process_headers(std::stringstream &reqFile, std::string line);
		void save_headers(std::string &line);
		void parse_headers();
		bool space_in_header_name(std::string line);
		void process_body(std::stringstream &reqFile, std::string line);

	// Utils
		void	clear();
		void	set_error_code(int code, std::string msg);
		void	print_request();
		void	print_header();
		void	print_body();
		void	print_others();
		
};

#endif