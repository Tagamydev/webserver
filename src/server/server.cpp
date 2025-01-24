#include "server.hpp"

/// @brief print config files parameters saved in server.
void	server::print_config_file()
{
	location tmp_loc;
	//general
	utils::print_vector_content(this->_ports, "Ports");
	utils::print_vector_content(this->_names, "Server_name");
	std::cout << "_max_body_size: " << this->_max_body_size << std::endl;
	utils::print_map_content(this->_error_pages, "Error pages");
	//location
	size_t i = 1;
		std::cout << "\n<<<   "<< "locations" << "   >>>" << std::endl;
	for (std::map<std::string, location>::iterator it = _locations.begin(); it != _locations.end(); it++)
		{
			tmp_loc = it->second;
			std::cout << "--- Item " << i++;
		//	what is this???
			//it->second;
			std::cout  << std::endl;
			tmp_loc.print_location_content();
		}
	std::cout << std::endl;
	
}

const int &server::get_first_port() const { return *this->_ports.begin(); }

const std::string &server::get_first_name() const { return *this->_names.begin(); }

const std::vector<int> &server::get_ports() const { 	return this->_ports; }

const std::vector<std::string> &server::get_names() const { 	return this->_names; }

/// @brief chec if is a number in a valid range. Also check if is an ip or localhost to print invalid message
bool	server::is_valid_port(std::string port)
{
	size_t tmp = atoi(port.c_str());
	
	if (port.find('.') != std::string::npos)
		throw std::runtime_error("Error reading config file. IP address on listen directive is not supported.");
	else if (port.find("localhost") != std::string::npos)
		throw std::runtime_error("Error reading config file. Localhost on listen directive is not supported.");
	if (tmp < 0 || tmp > 65535)
	{
		return (false);
	}
	return (true);
}

/// @brief check if mandatory parameters are empty. Throw error for now in case of empty.
void server::check_empty_set_default()
{
	/*
	if (this->_names.empty())
	{
		// _names.push_back("");
		throw std::runtime_error("Error reading config file. Server name can't be empty.");
	}
	*/
	if (this->_ports.empty())
	{
		// _names.push_back("");
		throw std::runtime_error("Error reading config file. You must insert a port to liten to.");
	}
	//set default root
	// if (this->_root.empty())
	// {
	// 	// _names.push_back("");
	// }

}

/// @brief check if there's duplicated location. It wont, its a map xD
void server::check_location_list() 
{
	if (this->_locations.size() < 2)
		return ;
	std::vector<std::string>::const_iterator it1;
	std::vector<std::string>::const_iterator it2;
	std::vector<std::string> paths;
	utils::map_to_vector_populate_keys(_locations, paths);
	for (it1 = paths.begin(); it1 != paths.end() - 1; it1++) {
		for (it2 = it1 + 1; it2 != paths.end(); it2++) {
			if (*it1 == *it2)
				throw std::runtime_error("Error reading config file. Duplicated Location path.");
		}
	}
	return ;
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
	utils::trim_semicolon(value);
	if (key == "listen")
	{
		if (utils::is_empty(value))
			throw std::runtime_error("Error reading config file. Wrong value in listen directive.");
		if (!is_valid_port(value))
			throw std::runtime_error("Error reading config file. Invalid port, out of range.");
		std::cout << value << "vs." << atoi(value.c_str()) << std::endl;
		this->_ports.push_back(atoi(value.c_str()));
	}
	else if (key == "server_name")
	{
		if (utils::is_empty(value))
			throw std::runtime_error("Error reading config file. Wrong value in server_name directive.");
		utils::ft_toLower(value);
		this->_names = utils::split_to_vector(value, ' ');
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
			throw std::runtime_error("Error reading config file. Wrong value in error_page directive.");
		tmp_vector = utils::split_to_vector(value, ' ');
		tmp_str = tmp_vector[tmp_vector.size() - 1];
		for (size_t i = 0; i < tmp_vector.size() - 1; i++)
		{
			if (atoi(tmp_vector[i].c_str()) >= 100 && atoi(tmp_vector[i].c_str()) <= 600 )
				this->_error_pages[atoi(tmp_vector[i].c_str())] = tmp_str;
			else
				throw std::runtime_error("Error reading config file. Wrong or invalid error page directive.");
		}
	}
	else if (key == "location")
	{
		std::string loc_str;

		check_location(line);
		loc_str += line+ '\n';
		while (getline(contentStream, line))
		{
			if (utils::is_empty(line))
				continue;
			else if (line.find('{') != std::string::npos)
			{
				loc_str += line + '\n';
				while (getline(contentStream, line))
				{
					loc_str += line + '\n';
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
		location	new_location(loc_str);
		_locations[new_location.get_path()] = new_location;
		loc_str.clear();	
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
	std::string loc_str;

	while (getline(contentStream, line))
	{
		if (utils::is_empty(line))
			continue;
		else if (line.find('{') != std::string::npos)
		{
			check_location(line);
			loc_str += line+ '\n';
			while (getline(contentStream, line))
			{
				loc_str += line + '\n';
				if (line.find('}') != std::string::npos)
				break ;
			}
			location	new_location(loc_str);
			_locations[new_location.get_path()] = new_location;
			loc_str.clear();	
		}
		else
			process_parameters(contentStream, line);
	}
}

/// @brief  Init server vector with default error pages
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


server::server(std::string &content):_max_body_size (MAX_BODY_SIZE), _is_relative(false)
{
	std::stringstream contentStream;

	initErrorPages();
	contentStream << content;
	contentStream.seekg(0);
	check_save_parameters(contentStream);
	check_location_list();	
	check_empty_set_default();
}
server::server()
{

}

server::~server()
{

}
