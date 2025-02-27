/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserver.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: samusanc <samusanc@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/15 08:03:12 by samusanc          #+#    #+#             */
/*   Updated: 2025/01/28 17:14:19 by samusanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserver.hpp"

webserver::webserver(std::string &path)
{
	// set init
	this->set_status_codes();
	this->set_mime_types();

	// parser
	this->get_file_info(path);
	this->_config_file = save_config_file(path);
	remove_comments(_config_file);
	utils::fix_spaces_in_line(_config_file);
	check_brackets(_config_file);
	// print_config_info (_config_file);
	check_save_server_vector(_config_file);
	// print_list_content(_server_block_list, title));

	//bucle creando servers
	for (std::list<std::string>::iterator it = _server_block_list.begin(); it != _server_block_list.end(); it++)
	{
		server newServer(*it);
		//add to list of servers
		_servers.push_back(newServer);
		// std::cout << "Create Server "  << newServer.get_first_name() << std::endl;
	}
	check_servers();
	sort_servers();
	this->_loop = new loopHandler(*this);
}

webserver::~webserver()
{
	delete this->_loop;
}

void	webserver::sort_servers()
{
	std::vector<server>::iterator	i = this->_servers.begin();
	std::vector<server>::iterator	ie = this->_servers.end();

	for (; i != ie ; i++)
	{
		std::vector<int>::iterator	j = i->_ports.begin();
		std::vector<int>::iterator	je = i->_ports.end();

		for (; j != je ; j++)
		{
			std::list<server*>	&_list = this->_port_servers_list[*j];

			_list.push_back(&(*i));
		}
	}
}

/// Parsing Utils
void	print_config_info(std::string configFile)
{
	std::cout << "\n<<<   Config File   >>>" << std::endl;
	std::cout << configFile << std::endl;
}

/// Parsing 

/// @brief checking if at least one of the three mandatory parameters of servers is different.
void webserver::check_servers()
{
    if (_servers.size() <= 1)
        return;

    std::set<std::pair<int, std::string> > seenConfigurations;

    for (std::vector<server>::iterator itServer = _servers.begin(); itServer != _servers.end(); ++itServer)
    {
        const std::vector<int>& ports = itServer->get_ports();
        const std::vector<std::string>& names = itServer->get_names();

        for (std::vector<int>::const_iterator itPort = ports.begin(); itPort != ports.end(); ++itPort)
        {
            for (std::vector<std::string>::const_iterator itName = names.begin(); itName != names.end(); ++itName)
            {
                std::pair<int, std::string> config = std::make_pair(*itPort, *itName);

                if (seenConfigurations.find(config) != seenConfigurations.end())
                {
                    std::ostringstream errorMessage;
                    errorMessage << "Error in config file: Duplicate port and server name combination ("
                                 << *itPort << ", " << *itName << ").";
                    throw std::runtime_error(errorMessage.str());
                }
                seenConfigurations.insert(config);
            }
        }
    }
}

size_t	webserver::find_end_server(size_t start, std::string configFile)
{
	size_t brackets = 0;
	size_t end = start;

	//while (configFile[++end])
	for (; end < configFile.length(); end++)
	{
		if (configFile[end] == '{')
			brackets++;
		if (configFile[end] == '}')
		{
			if (!brackets)
				return (end - 1); // to avoid save bracket
			brackets--;
		}
	}
	if (brackets > 0)
		throw std::runtime_error("Error reading config file. Scope error, check for open brackets. (1)");
	if (end >= configFile.length())
		return (start); // for the last search?
	return (start);
}

size_t	webserver::find_start_server(size_t start, std::string configFile)
{
	size_t pos = start;
	
	if (configFile.find("server", start) == std::string::npos)
		return (configFile.length());
	while (pos < configFile.length())
	{
		if (configFile[pos] == 's')
			break;
		else if (!isspace(configFile[pos]))
			throw std::runtime_error("Error reading config file. Wrong character out of server scope. (1)");
		pos++;
	}
	if (configFile.compare(pos, 6, "server") != 0)
		throw std::runtime_error("Error reading config file. Wrong character out of server scope. (2)");
	pos += 6;
	while (configFile[pos] && isspace(configFile[pos] ))
		pos++;
	if (configFile[pos] == '{')
		return (pos + 1); // to avoid bracket
	else
		throw std::runtime_error("Error reading config file. Wrong character out of server scope.(3)");

}

std::list<std::string>	webserver::check_save_server_vector(std::string &configFile)
{
	size_t start = 0;
	size_t end = 1;

	if (configFile.find("server", 0) == std::string::npos)
		throw std::runtime_error("Error reading config file. Server not found.");
	while (start != end && start < configFile.length())
	{
		start = find_start_server(start, configFile);
		end = find_end_server(start, configFile);
		if (start == end)
			break ;
		this->_server_block_list.push_back(configFile.substr(start, end - start));
		
		start = end + 2; //because I save the end before bracket
	}
	if (_server_block_list.empty())
		throw std::runtime_error("Error reading config file. Scope error, check for open brackets. (2)");
	return (this->_server_block_list);
}

void	webserver::check_brackets(std::string configFile)
{
	size_t brackets = 0;
	size_t i = 0;

	while (configFile[i])
	{
		if (configFile[i] == '{')
			brackets++;
		else if (configFile[i] == '}')
			brackets--;
		i++;
	}
	if (brackets != 0)
		throw std::runtime_error("Error reading config file. Scope error, check for open brackets. (0)");
}

void	webserver::remove_comments(std::string &configFile)
{
	size_t	pos;

	pos = configFile.find('#');
	while (pos != std::string::npos)
	{	
		configFile.erase(pos, configFile.find('\n', pos) - pos);
		pos = configFile.find('#');
	}
}

/*
returns of stat()
S_ISREG(m)  is it a regular file?
S_ISDIR(m)  directory?
S_ISCHR(m)  character device?
S_ISBLK(m)  block device?
S_ISFIFO(m) FIFO (named pipe)?
S_ISLNK(m)  symbolic link?  (Not in POSIX.1-1996.)
S_ISSOCK(m) socket?  (Not in POSIX.1-1996.)
*/
void	webserver::get_file_info(std::string path)
{
	struct stat st;
	
    if (stat(path.c_str(), &st) == -1)
		throw std::runtime_error("Error reading config file.");
    else if (S_ISDIR(st.st_mode))
		throw std::runtime_error("Error reading config file. Is a directory.");
    else if (S_ISREG(st.st_mode))
    	return ;
    else
		throw std::runtime_error("Error reading config file. Not a regular file.");
}

std::string	webserver::save_config_file(std::string &path)
{
	std::string			result;
	std::stringstream	buff;
	std::ifstream		file(path.c_str());

	if (!file)
		throw std::runtime_error("Error reading config file.");
	buff << file.rdbuf();
	file.close();
	result = buff.str();
	if (result.empty())
		throw std::runtime_error("Config file is empty.");
	return (result);
}

void	webserver::set_mime_types()
{
    this->_mime_types[".html"] = "text/html";
    this->_mime_types[".htm"] = "text/html";
    this->_mime_types[".css"] = "text/css";
    this->_mime_types[".js"] = "application/javascript";
    this->_mime_types[".json"] = "application/json";
    this->_mime_types[".xml"] = "application/xml";
    this->_mime_types[".txt"] = "text/plain";
    this->_mime_types[".csv"] = "text/csv";
    this->_mime_types[".jpg"] = "image/jpeg";
    this->_mime_types[".jpeg"] = "image/jpeg";
    this->_mime_types[".png"] = "image/png";
    this->_mime_types[".gif"] = "image/gif";
    this->_mime_types[".bmp"] = "image/bmp";
    this->_mime_types[".webp"] = "image/webp";
    this->_mime_types[".svg"] = "image/svg+xml";
    this->_mime_types[".ico"] = "image/vnd.microsoft.icon";
    this->_mime_types[".woff"] = "font/woff";
    this->_mime_types[".woff2"] = "font/woff2";
    this->_mime_types[".ttf"] = "font/ttf";
    this->_mime_types[".otf"] = "font/otf";
    this->_mime_types[".eot"] = "application/vnd.ms-fontobject";
    this->_mime_types[".vtt"] = "text/vtt";
    this->_mime_types[".mp4"] = "video/mp4";
    this->_mime_types[".mpeg"] = "video/mpeg";
    this->_mime_types[".webm"] = "video/webm";
    this->_mime_types[".avi"] = "video/x-msvideo";
    this->_mime_types[".mp3"] = "audio/mpeg";
    this->_mime_types[".wav"] = "audio/wav";
    this->_mime_types[".ogg"] = "audio/ogg";
    this->_mime_types[".pdf"] = "application/pdf";
    this->_mime_types[".zip"] = "application/zip";
    this->_mime_types[".tar"] = "application/x-tar";
    this->_mime_types[".gz"] = "application/gzip";
    this->_mime_types[".7z"] = "application/x-7z-compressed";
    this->_mime_types[".rar"] = "application/vnd.rar";
    this->_mime_types[".doc"] = "application/msword";
    this->_mime_types[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    this->_mime_types[".xls"] = "application/vnd.ms-excel";
    this->_mime_types[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
    this->_mime_types[".ppt"] = "application/vnd.ms-powerpoint";
    this->_mime_types[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
}

void	webserver::set_status_codes()
{
	this->_status_codes[100] = "Continue";
	this->_status_codes[101] = "Switching Protocols";
	this->_status_codes[102] = "Processing";
	this->_status_codes[200] = "OK";
	this->_status_codes[201] = "Created";
	this->_status_codes[202] = "Accepted";
	this->_status_codes[203] = "Non-authoritative Information";
	this->_status_codes[204] = "No Content";
	this->_status_codes[205] = "Reset Content";
	this->_status_codes[206] = "Partial Content";
	this->_status_codes[207] = "Multi-Status";
	this->_status_codes[208] = "Already Reported";
	this->_status_codes[226] = "IM Used";
	this->_status_codes[300] = "Multiple Choices";
	this->_status_codes[301] = "Moved Permanently";
	this->_status_codes[302] = "Found";
	this->_status_codes[303] = "See Other";
	this->_status_codes[304] = "Not Modified";
	this->_status_codes[305] = "Use Proxy";
	this->_status_codes[307] = "Temporary Redirect";
	this->_status_codes[308] = "Permanent Redirect";
	this->_status_codes[400] = "Bad Request";
	this->_status_codes[401] = "Unauthorized";
	this->_status_codes[402] = "Payment Required";
	this->_status_codes[403] = "Forbidden";
	this->_status_codes[404] = "Not Found";
	this->_status_codes[405] = "Method Not Allowed";
	this->_status_codes[406] = "Not Acceptable";
	this->_status_codes[407] = "Proxy Authentication Required";
	this->_status_codes[408] = "Request Timeout";
	this->_status_codes[409] = "Conflict";
	this->_status_codes[410] = "Gone";
	this->_status_codes[411] = "Length Required";
	this->_status_codes[412] = "Precondition Failed";
	this->_status_codes[413] = "Payload Too Large";
	this->_status_codes[414] = "Request-URI Too Long";
	this->_status_codes[415] = "Unsupported Media Type";
	this->_status_codes[416] = "Requested Range Not Satisfiable";
	this->_status_codes[417] = "Expectation Failed";
	this->_status_codes[418] = "I’m a teapot";
	this->_status_codes[421] = "Misdirected Request";
	this->_status_codes[422] = "Unprocessable Entity";
	this->_status_codes[423] = "Locked";
	this->_status_codes[424] = "Failed Dependency";
	this->_status_codes[426] = "Upgrade Required";
	this->_status_codes[428] = "Precondition Required";
	this->_status_codes[429] = "Too Many Requests";
	this->_status_codes[431] = "Request Header Fields Too Large";
	this->_status_codes[444] = "Connection Closed Without Response";
	this->_status_codes[451] = "Unavailable For Legal Reasons";
	this->_status_codes[499] = "Client Closed Request";
	this->_status_codes[500] = "Internal Server Error";
	this->_status_codes[501] = "Not Implemented";
	this->_status_codes[502] = "Bad Gateway";
	this->_status_codes[503] = "Service Unavailable";
	this->_status_codes[504] = "Gateway Timeout";
	this->_status_codes[505] = "HTTP Version Not Supported";
	this->_status_codes[506] = "Variant Also Negotiates";
	this->_status_codes[507] = "Insufficient Storage";
	this->_status_codes[508] = "Loop Detected";
	this->_status_codes[510] = "Not Extended";
	this->_status_codes[511] = "Network Authentication Required";
	this->_status_codes[599] = "Network Connect Timeout Error";
}

std::string webserver::status_message(int code)
{
	std::string	result;

	result = this->_status_codes[code];
	return (result);
}

std::string webserver::mime_types(std::string &extention)
{
	std::string	result;

	result = this->_mime_types[extention];
	return (result);
}

