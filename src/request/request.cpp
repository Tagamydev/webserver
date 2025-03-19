/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samusanc <samusanc@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 07:52:36 by samusanc          #+#    #+#             */
/*   Updated: 2025/02/10 06:18:37 by samusanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "request.hpp"
#include "utils.hpp"
#include "webserver.hpp"
#include <cstddef>
#include <string>

/* while on reqFile to skip new lines at the begining of the request.
*/
request::request(client *_client, webserver *_webserver, 
std::vector<struct pollfd> &list)
{
	utils::print_debug("new request");
	this->clear();
	this->_fd = _client->get_fd();
	this->parsing();
	this->debug();
	this->check_config_file(_client, _webserver);
	this->set_cgi_extension();
	this->parse_body(this->_server);

	if (this->check_if_cgi())
	{
		this->_cgi_status = WAITING;
		this->_cgi = new cgi(*this, _client, list, _webserver);
	}
}

std::string split_hostname(const std::string& hostname)
{
	std::string	host;
	std::size_t colonPos = hostname.find(':');
	
	if (colonPos != std::string::npos)
		host = hostname.substr(0, colonPos);
	else
		host = hostname;
	return (host);
}

server	*find_wildcard_name(std::list<server*> list, std::string hostname)
{
	std::list<server*>::iterator i = list.begin();
	std::list<server*>::iterator ie = list.end();
	std::vector<std::string>	names_list;

	for (; i != ie ; i++)
	{
		names_list = (*i)->_names;
		std::vector<std::string>::iterator	j = names_list.begin();
		std::vector<std::string>::iterator	je = names_list.end();
		std::vector<std::string>::iterator	result;

		for (; j != je ; j++)
		{
			if (*j == "_" || *j == "*")
				return (*i);
		}
	}
	i = list.begin();
	ie = list.end();

	for (; i != ie ; i++)
	{
		names_list = (*i)->_names;
		if (names_list.empty())
			return (*i);
	}
	return (NULL);
}

server	*find_server_by_name(std::list<server*> list, std::string hostname)
{
	std::list<server*>::iterator i = list.begin();
	std::list<server*>::iterator ie = list.end();
	std::vector<std::string>	names_list;

	for (; i != ie ; i++)
	{
		names_list = (*i)->_names;
		std::vector<std::string>::iterator	j = names_list.begin();
		std::vector<std::string>::iterator	je = names_list.end();
		std::vector<std::string>::iterator	result;

		result = std::find(j, je, hostname);
		if (result != je)
			return (*i);
	}
	return (NULL);
}

server	*find_server_by_hostname(std::list<server*> list, std::string hostname)
{
	server		*result;
	std::string	hostname_without_port;

	hostname_without_port = split_hostname(hostname);
	result = find_server_by_name(list, hostname_without_port);
	if (!result)
		result = find_server_by_name(list, hostname);
	if (!result)
		result = find_wildcard_name(list, hostname);
	return (result);
}

std::string location_from_uri(const std::string& uri)
{
	std::string	result;
	std::string	copy(uri.c_str() + 1);
	std::size_t colonPos = copy.find('/');
	
	if (colonPos != std::string::npos)
		result = uri.substr(0, colonPos + 1);
	else
		result = uri;
	return (result);
}

location	*get_location_from_uri(server *_server, std::string url)
{
	std::map<std::string, location>::iterator	it;
	std::map<std::string, location>::iterator	ie;
	std::map<std::string, location>::iterator	best_match;

	it = _server->_locations.begin();
	ie = _server->_locations.end();
	best_match = ie;
	size_t best_match_length = 0;
	for (; it != ie ; ++it)
	{
		const std::string &loc_path = it->first;
		if (url.find(loc_path) == 0)
		{
			if (loc_path.length() > best_match_length)
			{
				best_match = it;
				best_match_length = loc_path.length();
			}
		}
	}
	if (best_match != ie)
		return &(best_match->second);
	return (NULL);
}

void	request::get_server(client *_client, webserver *_webserver)
{
	if (this->_error_code != -1)
		return ;
	std::list<server*>	_servers_list = _webserver->_port_servers_list[_client->port];
	std::string			hostname = this->_headers["host"];
	server				*result;

	result = find_server_by_hostname(_servers_list, hostname);
	if (!result)
	{
		set_error_code(404, "host not found");
		return ;
	}
	this->_server = result;
}

void	request::get_location()
{
	location			*result;

	if (this->_error_code != -1)
		return ;
	result = get_location_from_uri(this->_server, this->_uri_file);
	if (!result)
	{
		set_error_code(404, "location not found");
		return ;
	}
	result->print_location_content();

	if (result->_return)
	{
		set_error_code(std::atoi(result->_return_code.c_str()), result->_return_path);
		return ;
	}

	if (!result->is_allowed_method(this->_method))
	{
		set_error_code(405, "Method Not Allowed");
		return ;
	}
	this->_location = result;
}

void	request::check_config_file(client *_client, webserver *_webserver)
{
	if (this->_error_code != -1)
		return ;
	this->get_server(_client,_webserver);
	this->get_location();


	//check if location is cgi enabled
	this->_location = get_location_from_uri(_server, _uri_file);
	if (this->_location)
	{
		if (this->_location->_cgi_enabled)
			this->_is_cgi = true;
	}



	if (this->_location && !this->_location->_alias.empty())
	{
		this->_uri_file = this->_location->_alias;
		if (this->_uri_file[0] != '/')
			this->_uri_file = "./" + this->_uri_file;
		std::cout << "[Path]: " << this->_uri_file << std::endl;
	}
	else if (this->_location && !this->_location->_root.empty())
	{
		this->_uri_file = this->_location->_root + this->_uri_file;
		if (this->_uri_file[0] != '/')
			this->_uri_file = "./" + this->_uri_file;
		std::cout << "[Path]: " << this->_uri_file << std::endl;
	}
	else
		this->_uri_file = "." + this->_uri_file;

}

request::~request()
{
	// utils::print_debug("delete request"); //should we disconect client? Check closeRequestExample.cpp

	if (this->_cgi)
		delete this->_cgi;
}

void	request::debug()
{
	print_request();
	print_header();
	print_body();
	print_others();
}

void	request::parsing()
{
	std::string			file;
	std::stringstream	reqFile;
	std::string			line;

	file = utils::read_file(this->_fd);
	// std::cout << file << std::endl;
	reqFile << file;
	reqFile.seekg(0);
	getline(reqFile, line);
	while (!reqFile.eof() && line.empty())
		getline(reqFile, line);
	this->check_save_request_line(line);
	if (this->_error_code == -1)
		this->process_headers(reqFile, line);
	if (this->_error_code == -1)
		this->process_body(reqFile, line);
	if (this->_error_code == -1)
		this->parse_headers();
	if (this->_error_code == -1)
		parse_body(this->_server);
}

bool	request::check_if_cgi()
{
	if (this->_error_code != -1)
		return (false);
	
	// size_t	dotPos = this->_uri_file.find_last_of('.');
	// std::string extension = "";


	// //Check extension. // CHECK IF NECESSARY
    // if (dotPos != std::string::npos)
    //     extension = this->_uri_file.substr(dotPos);
	// if (this->get_cgi_extension(extension))
	// {
	// 	this->_is_cgi = true;
	// 	// return (true);
	// }
	if (utils::is_directory(this->_uri_file))
	{
		this->_is_cgi = false;
		return (false);
	}
	//check if location is cgi enabled
	// this->_location = get_location_from_uri(_server, _uri_file);
	// if (this->_location)
	// {
	// 	// this->_location->print_location_content();
	// 	// utils::print_debug();
	// 	if (this->_location->_cgi_enabled)
	// 	{
	// 		this->_is_cgi = true;
	// 		return (true);
	// 	}
	// }
	if(this->_is_cgi == true)
		return (true);
	this->_is_cgi = false;
	return (false);
}

void	request::close_cgi()
{
	if (this->_cgi)
		delete this->_cgi;
	this->_cgi = NULL;
	this->_cgi_status = DONE;
}

void	request::clear()
{
	this->_fd = -1;
	this->_method.clear();
	this->_uri.clear();
	this->_uri_file.clear();
	this->_http_version.clear();
	this->_headers.clear();
	this->_body.clear();
	this->_content_length = -1;
	this->_has_body = -1;
	this->_chunked_flag = false;
	this->_is_cgi = false;
	this->_multiform_flag = 0;
	this->_boundary.clear();
	this->_error_code = -1;
	this->_debug_msg.clear();
	this->_cgi = NULL;
	this->_cgi_status = NONE;
	this->_location = NULL;
	this->_server = NULL;
}
// Handle body

void request::parse_body(server *this_server)
{
	size_t pos;

	if (this->_error_code != -1 && this->_error_code <= 200 && this->_error_code >= 300)
		return ;
	if(!this_server)
		return ;

	if (!this->_has_body)
		return ;
	if (atoi(this->_headers["content-length"].c_str()) >= 1)
	{
		// line = utils::read_file_max_size("examples/request/chunked.txt", atoi(this->_headers["content-length"].c_str()));
		if (_body.length() >= atoi(this->_headers["content-length"].c_str()))
			_body = _body.substr(0, atoi(this->_headers["content-length"].c_str()));
		else
			this->_headers["content-length"] = utils::to_string(_body.length());
		_content_length = atoi(this->_headers["content-length"].c_str());
	}
	else if (this->_headers.count("transfer-encoding") && this->_headers["transfer-encoding"] == "chunked")
	{
		_chunked_flag = 1;
		this->process_chunked();
	}	
	
	// need to test
	if (this->_headers.count("content-type") && this->_headers["content-type"].find("multipart/form-data") != std::string::npos)
	{
		pos = this->_headers["content-type"].find("boundary=", 0);
        if (pos != std::string::npos)
            this->_boundary = this->_headers["content-type"].substr(pos + 9, this->_headers["content-type"].size());
        this->_multiform_flag = true;
	}
	if (this->_body.length() > this_server->_max_body_size)
	{
		this->_error_code = 413;
		return ; 
	}
	if (this->_body.length() > this->_content_length)
		this->_body = this->_body.substr(0, this->_content_length);
	else
		this->_headers["content-length"] = utils::to_string(_body.length());
	_content_length = atoi(this->_headers["content-length"].c_str());
}

void request::process_chunked()
{
	size_t	chunkSize = 0;
	std::string tmpBody;
	std::string newBody;
	std::stringstream ss;
	std::string line;
	// bool	flag = 1;

	//later replace to _body
	// std::cout << "CHUNKED " << this->_body << std::endl;
	tmpBody = utils::read_file_max_size("examples/request/chunked.txt", 200);
	std::cout << "\n\nCHUNKED " << tmpBody << std::endl;
    ss.str(tmpBody);
	while (getline(ss, line))
	{
		utils::trim_space_newline(line);
		if (line.empty())
			continue;
		else if (utils::hexToDecimal(line) >= 1)
		{
			chunkSize = utils::hexToDecimal(line);
			// flag = !flag;
			getline(ss, line);
			utils::trim_space_newline(line);
			if (line.empty())
				getline(ss, line);
			// newBody += line;
			newBody += line.substr(0, chunkSize);
		}
		else if (utils::hexToDecimal(line) <= 0)
			break;
	}
	if (utils::hexToDecimal(line) != 0)
		newBody += line;
	_body = newBody;
	std::cout << "\n\nNew Body " << _body << std::endl;
}

void request::process_body(std::stringstream &reqFile, std::string line)
{
	std::string tmp;

	line.clear();
	getline(reqFile, tmp);
	while (!reqFile.eof())
	{
		line += tmp;
		line += '\n';
		getline(reqFile, tmp);
	}
	line += tmp;
	utils::trim_space_newline(line);
	if (utils::is_empty(line))
	{
		this->_has_body = 0;
		return ;
	}
	this->_has_body = 1;
	this->_body = line;

	//clear line?
}

// Handle Headers

void request::parse_headers()
{
	if (!this->_headers.count("host"))
			return (set_error_code(400, "Host header not found."));
	if (this->_headers.count("content-length"))
    {
		// if (this->_headers.count("transfer-encoding"))
			// return (set_error_code(400, "Incopatible headers: content-length & transfer-encoding."));
		this->_content_length = std::atoi(_headers["content-length"].c_str());
		if (this->_content_length <= 0)
			return (set_error_code(400, "Invalid Content-length header."));
    }
	if (this->_http_version.compare("HTTP/1.0") == 0 && this->_headers.count("transfer-encoding"))
		return (set_error_code(400, "Invalid header transfer-encoding on HTTP/1.0."));
	if (this->_headers.count("transfer-encoding") && this->_headers["transfer-encoding"].find_first_of("chunked") != std::string::npos)
        this->_chunked_flag = 1;
}

bool request::space_in_header_name(std::string line)
{
	int	i = 0;

	while (line[i] != '\0')
	{
		if (line[i] == ' ' || line[i] == '\t')
			return (1);
		i++;
	}
	return(0);
}

/* @brief save formated headers in map
*/
void request::save_headers(std::string &line)
{
	int	i = 0;
	int	flag = 0;
	std::string	tmp;

	while (i < line.length())
	{
		while (line[i] == ' ')
			i++;
		if (i >= line.size() || line.find('\n') == std::string::npos) 
		{
			/*
				HOLA DAVID MIRA ESTO ES UN CHECKEO QUE LO HAGO PORQUE EL PARSING DEL BODY ESTÁ MAL, 
				ASI QUE REVISA QUE ESTO PRIMERO ESTÉ BIEN Y SEGUNDO QUE NO ROMPA NADA MÁS.
			*/
			this->_body = line;
			return ;
		}
		flag = std::count(line.begin() + i, line.begin() + line.find('\n'), ':');
		if (flag > 0)
		{
			tmp = line.substr(i, (line.find(':') - i));
			if(space_in_header_name(tmp))
				return (set_error_code(400, "Found space on header name."));
			utils::ft_to_lower(tmp);
			i = line.find(':');
			while (line[i] == ' ' || line[i] == ':')
				i++;
			this->_headers[tmp] = line.substr(i, (line.find('\n') - i));
		}
		else
		{
			tmp = line.substr(i, (line.find('\n') - i));
			if(space_in_header_name(tmp))
				// return (set_error_code(400, "Found space on header name."));
			utils::ft_to_lower(tmp);
			this->_headers[tmp] = "";
		}
		line.erase(0, line.find('\n') + 1);
		i = 0;
	}
}


void    request::process_headers(std::stringstream &reqFile, std::string line)
{
	std::string tmp;

	line.clear();
	getline(reqFile, tmp);
	while (!tmp.empty() && tmp != "\r\n\r\n")
	{
		line += tmp;
		line += '\n';
		getline(reqFile, tmp);
		if (reqFile.eof())
		{
			line += tmp;
			break;
		}
	}
	utils::fix_spaces_in_line(line);
	save_headers(line);
}

// Request (first) line

void request::is_valid_httpv(std::string line)
{
 if (line != "HTTP/1.0" && line != "HTTP/1.1")
	return (set_error_code(505, "HTTP Version Not Supported"));
}

void	request::process_uri(std::string line)
{
	if (line.find('?') == std::string::npos)
	{
		this->_uri_file = line;
		return ;
	}
	this->_uri_file = line.substr(0, line.find('?'));
	this->_uri_params = line.substr(line.find('?') + 1, line.length());
}

/**

* Checks whether the character passed is allowed in a field name
* Characters allowed as specifed in RFC:
"!" / "#" / "$" / "%" / "&" / "'"
/ "*" / "+" / "-" / "." / "^" / "_"
/ "`" / "|" / "~" / 0-9 / A-Z / a-z
**/
void request::is_valid_uri(std::string &line)
{
	std::string::iterator it;
	std::string normalized;
	size_t len = line.length();

	if (line.empty())
		return (set_error_code(400, "Empty or invalid URI."));
	if (len > MAX_URI_LENGTH)
		return (set_error_code(414, "Max. URI length reached."));
	// Check for invalid characters
	for (it = line.begin(); it < line.end(); it++)
	{
		if (!std::isalnum(*it) && *it != '/' && *it != '.' && *it != '-' && *it != '_' && *it != '%' \
		&& *it != ':' && *it != '&' && *it != '?' && *it != '!' && *it != '=' && *it != '+' \
		&& *it != '#' && *it != '*' && *it != '$' && *it != '\'' && *it != '^' && *it != '`' \
		&& *it != '|' && *it != '~')
			return (set_error_code(400, "Invalid character found on URI."));
	}
	// Check for ../ and //
	for (size_t i = 0; i < len; ++i) 
	{
		if (i + 2 < len && line[i] == '.' && line[i+1] == '.' && line[i+2] == '/')
			i += 2;
		else if (i + 1 < len && line[i] == '/' && line[i + 1] == '/') 
			continue ;
		else 
			normalized += line[i];
	}
	if (normalized.empty())
		return (set_error_code(400, "Empty or invalid URI."));
	line = normalized;
	process_uri(line);
}

void request::is_valid_method(std::string line)
{
	std::string methods[3] = {"GET", "POST", "DELETE"};

	if (line.empty())
			return (set_error_code(400, "Found space at first line."));
	for (size_t i = 0; i < methods->length(); i++)
	{
		if (!line.compare(methods[i]) && line.length() == methods[i].length())
			return ;
	}
	return (set_error_code(405, "Method Not Allowed."));
}

void request::check_save_request_line(std::string line)
{
	std::string key;
	std::map<std::string, std::string> mp;
	
	if (line[0] == ' ')
		return (set_error_code(400, "Found spaces before method."));
	utils::fix_spaces_in_line(line);
	this->_query = line;
	//check method
	if (line.find(" ") != std::string::npos)
		key = line.substr(0, line.find(" "));
	else
		// return (set_error_code(400, "No spaces on first line."));
	this->is_valid_method(key);
	this->_method = key;
	line = line.substr((line.find(" ") + 1), line.length());

	//check URI
	if (line.find(" ") == std::string::npos)
		return (set_error_code(400, "No spaces after method."));
	key = line.substr(0, line.find(" "));
	this->is_valid_uri(key);
	this->_uri = key;
	line = line.substr((line.find(" ") + 1), line.length());

	//check HTTP Version
	if (line.find(" ") != std::string::npos)
		return (set_error_code(400, "No spaces after URI."));
	key = line.substr(0, line.length());
	this->is_valid_httpv(key);
	this->_http_version = key;

}

//Getters

std::map<std::string, std::string>	request::get_headers()
{
	return(this->_headers);
}

void	request::set_cgi_extension()
{
    this->_cgi_extensions[".php"] = "/usr/bin/php-cgi";
    this->_cgi_extensions[".py"] = "/usr/bin/python3";
    this->_cgi_extensions[".cgi"] = "/usr/bin/bash";
    this->_cgi_extensions[".sh"] = "/usr/bin/bash";
}


// Handle errors
void request::set_error_code(int code, std::string msg)
{
	this->_error_code = code;
	this->_debug_msg = msg;	
}


// Utils
void	request::print_request()
{
	utils::print_debug("\n<<<<    Request    >>>>");
	utils::print_debug("Method : " + this->_method);
	utils::print_debug("URI : " + this->_uri);
	utils::print_debug("HTTP V : " + this->_http_version);
	utils::print_debug("Uri_file : " + this->_uri_file);
	utils::print_debug("Uri_params : " + this->_uri_params);
}

void	request::print_header()
{
	utils::print_debug("\n<<<<    HEADER    >>>>");
	for (std::map<std::string,std::string>::iterator it = this->_headers.begin(); it != this->_headers.end(); it++)
	{
		utils::print_debug("Key: " + it->first);
		utils::print_debug(" Value: " + it->second);
	}
}

void	request::print_body()
{
	utils::print_debug("\n<<<<    BODY    >>>>");
	utils::print_debug(this->_body);
}
void	request::print_others()
{
	std::stringstream	body_length;
	std::stringstream	has_body;
	std::stringstream	chunked_flag;
	std::stringstream	error_code;

	body_length << this->_content_length;
	has_body << this->_has_body;
	chunked_flag << this->_chunked_flag;
	error_code << this->_error_code;
	utils::print_debug("\n<<<<    Control vars    >>>>");
	utils::print_debug("body lenght: " + body_length.str());
	utils::print_debug("Has body: " + has_body.str());
	utils::print_debug("Is chunked: " + chunked_flag.str());
	utils::print_debug("Error code: " + error_code.str());
	utils::print_debug("Debug Message: " + this->_debug_msg);
}
