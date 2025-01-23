#pragma once
#ifndef	RESPONSE_HPP
# define RESPONSE_HPP
# include "main.hpp"
# include "request.hpp"
# include "webserver.hpp"

class request;
class webserver;

class   response{
	public:

		// con/destructor
		response(request *_request, webserver *_webserver);
		~response(void);

	// STATUS LINE

		std::string	_http_version;
		int			_status_code;

	// HEADERS
		std::map<std::string, std::string>	_headers;	
		// header, content

	// BODY
		std::string	_body;
	
	// PRINT RESPONSE
		std::string	str();


	// FUNTIONS

		bool		_keep_alive;

	private:

		bool		_error;

		void		do_get();
		void		do_post();
		void		do_delete();
		void		do_cgi_response();

		void		set_length();
		void		get_file(std::string &path);
		void		get_dir(std::string &path);
		void		delete_dir(std::string &path);
		void		delete_file(std::string &path);

		std::string	status_message(int error);
		void		do_error_page(int error);
		void		do_redirection(int code, std::string location);

		std::string	get_mimeType(std::string &path);
		std::string print_status_line();
		std::string print_headers();

		webserver	*_webserver;
		request		*_request;

		//cgi parser
		void 			set_error_code(int code, std::string msg);

		int				_error_code;
		std::string		_debug_msg;
		
};

#endif
