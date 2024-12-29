#include "server.hpp"

/// @brief Trim semicolon from the end on a string. Used to clean values on parameters.
/// @param line - Takes the pointer
void server::trim_semicolon(std::string &line)
{
	int i = 0;
	int j = line.length() - 1;

	while (line[i] != '\0' && line[i] == ';')
		i++;
	while (line[j] != '\0' && line[j] == ';')
		j--;
	line = line.substr(i,j - i +1);
}

void	server::process_parameters(std::string line)
{
	std::string key;
	std::string value;

	trim_space_newline(line);
	key = line.substr(0, line.find(' '));
	value = line.substr(line.find(' '), line.length());
	trim_space_newline(value);
	std::cout << "Key " << key << std::endl;
	std::cout << "value " << value << std::endl;
	trim_semicolon(value);
	//check has semicolon
	if (key == "listen")
	{
		// check max port available and is positive
		if (is_empty(value))
			throw std::runtime_error("Error reading config file. Wrong value in listen directive.");
		this->_ports.push_back(atoi(value.c_str()));

	}
	else if (key == "server_name")
	{
		if (is_empty(value))
			throw std::runtime_error("Error reading config file. Wrong value in host directive.");
		while (!value.empty())
		{
			if (value.find(' ') != std::string::npos)
			{
				this->_names.push_back(value.substr(0, value.find(' ')));
				value.erase(0, value.find(' ') + 1);
			}
			else
			{
				this->_names.push_back(value.substr(0, value.length()));
				value.erase(0, value.length());
			}
		}
	}

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
		if (is_empty(line))
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
			// std::cout << "location" << location << std::endl;
			// std::cout << "process_location"<< std::endl;
			// process_location(line);		
			location.clear();	
		}
		else
		{
			process_parameters(line);
			// std::cout << "Parameter" << line << std::endl;
		}
		// std::cout << "LINE" << line << std::endl;
		// getline(contentStream, line);

	}
	print_list_content(_ports, "Ports");
	print_list_content(_names, "Server_name");

}

server::server(std::string &content)
{
	// std::string line;
	// std::string location;
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