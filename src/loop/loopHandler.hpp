/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   loopHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samusanc <samusanc@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/14 09:24:16 by samusanc          #+#    #+#             */
/*   Updated: 2025/01/14 18:46:00 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef	LOOPHANDLER_HPP
# define LOOPHANDLER_HPP
# include "main.hpp"
# include "webserver.hpp"
# include "request.hpp"
# include "client.hpp"

class webserver;
class request;
class client;

struct loopHandler{
	public:

		loopHandler(webserver &server);
		~loopHandler();

		void	new_server(int port);
		std::vector<struct pollfd>	make_fd_list();

	private:
		webserver							*_webserver;

		std::map<int, struct pollfd>		_port_serverFd;
		std::map<client *, struct pollfd>	_client_clientFd;
		std::map<int, struct pollfd>		_clientFd_cgiFd;
		
		// UTILS
		void	delete_client(client *_client);
		client	*get_client_from_clientFd(int fd);
		int		get_clientFd_from_cgiFd(int fd);
		void	send_response(int &i, std::vector<struct pollfd> &list);
		void	handle_client(int &i, std::vector<struct pollfd> &list);
		bool	is_server(int fd);
		bool	is_cgi(int fd);
		void	delete_fd_from_cgi_list(int fd);
		void	delete_cgi_from_list(cgi *_cgi);
		void	read_from_cgi(int &i, std::vector<struct pollfd> &list);
		void	new_client(struct pollfd socket);
		void	new_request(int fd);
		void	check_additions(int &i, std::vector<struct pollfd> &list);
		void	send_to_cgi(int &i, std::vector<struct pollfd> &list);


};

#endif
