#include "server.hpp"

server::server(std::string &content)
{
	std::string line;
	std::string location;
	std::stringstream contentStream;

	contentStream << content;
	contentStream.seekg(0);
	while (getline(contentStream, line))
	{
		if (is_empty(line))
			continue;
		else if (line.find('{') != std::string::npos)
		{
			location += line+ '\n';
			while (getline(contentStream, line))
			{
				location += line + '\n';
				if (line.find('}') != std::string::npos)
				break ;
			}
			std::cout << "location" << location << std::endl;
			// std::cout << "process_location"<< std::endl;
			// process_location(line);		
			location.clear();	
		}
		else
		{
			std::cout << "Parameter" << line << std::endl;
		}
		// std::cout << "LINE" << line << std::endl;
		// getline(contentStream, line);

	}
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


void	server::save_parameters(std::string line, std::string sep)
{
	if (is_empty(line))
		return ;

}