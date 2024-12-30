#pragma once
#ifndef	LOOPHANDLER_HPP
# define LOOPHANDLER_HPP
# include "main.hpp"
# include "webserver.hpp"
# include "request.hpp"

class webserver;
class request;

struct loopHandler{
	public:
		loopHandler(webserver &server);
		~loopHandler();

		std::vector<struct pollfd>	_fdsList;

		void			do_poll();
		unsigned int	total_fds();

		int				port_from_client_fd(int fd);

		void			new_client(int number);
		void			new_request(int number);
		void			new_server(int port);

		bool			check_poll_in(int number);
		bool			check_poll_in_server(int number);
		bool			check_poll_in_cgi(int number);

		bool			check_poll_out(int number);
		bool			check_poll_out_cgi(int number);

		request			*get_request_from_client(int n_client);

		// number of cgi (relative to the fds list), and the  request assosiated to that cgi;
		std::map<int, int>			_cgi_request;

	private:
		std::map<int, request*>		_client_and_request;

		// this is saved to find the port by the server FD along with the _clientFD_serverFD
		// this allow us to find the port of a client from the client fd
		std::map<int, int>			_serverFD_port;
		std::map<int, int>			_clientFD_serverFD;


		// list of all servers fd's for loop check
		std::list<int>				_serversFD;
		std::list<int>				_cgiFD;

		webserver					*_webserver;

		//Utils
};


#endif