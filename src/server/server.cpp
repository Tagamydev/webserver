#include "server.hpp"


void	server::print_config_file(server server)
{
	//general
	utils::print_list_content(this->_ports, "Ports");
	utils::print_list_content(this->_names, "Server_name");
	std::cout << "_max_body_size: " << this->_max_body_size << std::endl;
	utils::print_map_content(this->_error_pages, "Error pages");
	//location
	
}

location server::process_location(std::string line)
{
	location	loc(line);


	return (loc);
}

void	server::set_error_pages(std::string value)
{

}

bool	server::is_valid_port(int port)
{
	//check for ips? localhost?
	if (port < 0 || port > 65535)
		return (false);
	return (true);
}



void	server::process_parameters(std::stringstream &contentStream, std::string line)
{
	std::string key;
	std::string value;
	std::string tmp_str;
	std::vector<std::string> tmp_vector;

	utils::trim_space_newline(line);
	key = line.substr(0, line.find(' '));
	value = line.substr(line.find(' '), line.length());
	utils::trim_space_newline(value);
	// std::cout << "Key " << key << std::endl;
	// std::cout << "value " << value << std::endl;
	utils::trim_semicolon(value);
	//check has semicolon?
	if (key == "listen")
	{
		if (utils::is_empty(value))
			throw std::runtime_error("Error reading config file. Wrong value in listen directive.");
		if (!is_valid_port(atoi(value.c_str())))
			throw std::runtime_error("Error reading config file. Invalid port, out of range.");
		this->_ports.push_back(atoi(value.c_str()));
	}
	else if (key == "server_name")
	{
		if (utils::is_empty(value))
			throw std::runtime_error("Error reading config file. Wrong value in host directive.");
		this->_names = utils::split_to_list(value, ' ');
	}
	else if (key == "client_max_body_size")
	{
		if (utils::is_empty(value))
			throw std::runtime_error("Error reading config file. Wrong value in client_max_body_size directive.");
		if (!atoi(value.c_str()) || atoi(value.c_str()) < 0)
			throw std::runtime_error("Error reading config file. Client_max_body_size, invalid or out of range.");
		this->_max_body_size = atoi(value.c_str());
	}
	else if (key == "error_page")
	{
		if (utils::is_empty(value))
			throw std::runtime_error("Error reading config file. Wrong value in host directive.");
		tmp_vector = utils::split_to_vector(value, ' ');
		tmp_str = tmp_vector[tmp_vector.size() - 1];
		for (size_t i = 0; i < tmp_vector.size() - 1; i++)
		{
			if (atoi(tmp_vector[i].c_str()) >= 100 && atoi(tmp_vector[i].c_str()) <= 600 )
				this->_error_pages[atoi(tmp_vector[i].c_str())] = tmp_str;
			else
				throw std::runtime_error("Error reading config file. Wrong or invalid error code directive.");
		}
	}
	else if (key == "location")
	{
		std::string location;

		check_location(line);
		location += line+ '\n';
		while (getline(contentStream, line))
		{
			if (utils::is_empty(line))
				continue;
			else if (line.find('{') != std::string::npos)
			{
				location += line + '\n';
				while (getline(contentStream, line))
				{
					location += line + '\n';
					if (line.find('}') != std::string::npos)
					{
						line.clear();
						break ;		
					}
				}
			}
			else
				throw std::runtime_error("Error reading config file. Wrong parameter in location declaration. (1)");
			if (line.empty())
				break ;		
		}
		// std::cout << "location in parameter" << location << std::endl;
		// std::cout << "process_location"<< std::endl;
		process_location(location);		
		location.clear();	
	}
	else
		throw std::runtime_error("Error reading config file. Unknown directive.");

}

void	server::check_location(std::string line)
{
	size_t	i = 0;

	while (i < line.length())
	{
		if (line[i] == 'l')
			break;
		else if (!isspace(line[i]))
			throw std::runtime_error("Error reading config file. Wrong parameter in server scope. (1)");
		i++;
	}
	if (line.compare(i, 8, "location") != 0)
		throw std::runtime_error("Error reading config file. Wrong parameter in server scope. (2)");
	i += 8;
	while (line[i] && isspace(line[i] ))
		i++;
	if (line[i] == '{')
		throw std::runtime_error("Error reading config file. Wrong parameter in server scope. (3)");
	else
		return ;

}

void	server::check_save_parameters(std::stringstream &contentStream)
{
	std::string line;
	std::string location;

	while (getline(contentStream, line))
	{
		if (utils::is_empty(line))
			continue;
		else if (line.find('{') != std::string::npos)
		{
			check_location(line);
			location += line+ '\n';
			while (getline(contentStream, line))
			{
				location += line + '\n';
				if (line.find('}') != std::string::npos)
				break ;
			}
			process_location(location);		
			location.clear();	
		}
		else
		{
			process_parameters(contentStream, line);
			// std::cout << "Parameter" << line << std::endl;
		}
		// std::cout << "LINE" << line << std::endl;
		// getline(contentStream, line);

	}


}
// Init default
void server::initErrorPages(void)
{
	_error_pages[301] = "";
	_error_pages[302] = "";
	_error_pages[400] = "";
	_error_pages[401] = "";
	_error_pages[402] = "";
	_error_pages[403] = "";
	_error_pages[404] = "";
	_error_pages[405] = "";
	_error_pages[406] = "";
	_error_pages[500] = "";
	_error_pages[501] = "";
	_error_pages[502] = "";
	_error_pages[503] = "";
	_error_pages[505] = "";
	_error_pages[505] = "";
}


server::server(std::string &content)
{
	// std::string line;
	// std::string location;
	initErrorPages();
	std::stringstream contentStream;

	contentStream << content;
	contentStream.seekg(0);
	check_save_parameters(contentStream);
	
	//get line
	//loop until location or brackets
	//split first parameters
}
server::server()
{

}

server::~server()
{

}