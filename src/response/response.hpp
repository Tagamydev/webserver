#pragma once
#ifndef	RESPONSE_HPP
# define RESPONSE_HPP
# include "main.hpp"
# include "request.hpp"

class request;

class   response{
	public:

		// con/destructor
		response(request &rq);
		~response(void);

	// STATUS LINE

		std::string	http_version;
		int			status_code;

	// HEADERS
		std::map<std::string, std::string>	headers;	
		// header, content

	// BODY
		std::string	body;
	
	// PRINT RESPONSE
		std::string	str();


	// FUNTIONS


	private:

		bool		_error;
		std::map<int, std::string>	status_codes_list;
		std::map<std::string, std::string>	mime_types_list;
		void		set_status_codes_list();
		void		set_mime_types_list();
		void		do_get();
		void		do_post();
		void		do_delete();
		void		get_file(std::string &path);

		void		do_error_page();

		std::string	get_mimeType(std::string &path);
		std::string print_status_line();
		std::string print_headers();
		request		*request_form;
};

#endif