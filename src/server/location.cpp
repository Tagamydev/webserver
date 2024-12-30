#include "location.hpp"


void	location::print_location_content()
{
	std::cout << "<<<  Location  >>> " << std::endl;
	std::cout << "PATH " << _path << std::endl;
	std::cout << "ROOT " << _root << std::endl;
	std::cout << "ALIAS " << _alias << std::endl;
	std::cout << "index " << _index << std::endl;
	std::cout << "auto_index " << _auto_index << std::endl;
	std::cout << "index_file " << _index_file << std::endl;
	std::cout << "save_file " << _save_file << std::endl;
	std::cout << "cgi_enabled " << _cgi_enabled << std::endl;
	std::cout << "cgi " << _cgi << std::endl;
	std::cout << "return_code " << _return_code << std::endl;
	std::cout << "return_path " << _return_path << std::endl;
	print_vector_content(_allowed_methods, "allowed methods");

}
	

void	location::set_priv_attribute(std::string line)
{
	std::string	key;
	std::string	value;
	key = line.substr(0, line.find(' '));
	value = line.substr(line.find(' ') + 1, line.length());


	// std::cout << "Line " << line << "\n" << std::endl;
	// std::cout << "Key " << key << std::endl;
	// std::cout << "value " << value << std::endl;
	if (key == "root")
	{
		this->_root = value;
	}
	else if (key == "alias")
	{
		this->_alias = value;
	}
	else if (key == "allowed_methods" || key == "allow")
	{
		this->_allowed_methods = split_to_vector(value, ' ');
	}
	else if (key == "index")
	{
		this->_index_file = value;
	}
	else if (key == "save_file")
	{
		this->_save_file = value;
	}
	else if (key == "autoindex")
	{
		if (value == "on")
			this->_auto_index = true;
	}
	else if (key == "cgi")
	{
		if (value == "on")
			this->_cgi_enabled = true;
		else if (value == "on")
			this->_cgi_enabled = false;
	}
	else if (key == "return")
	{
		this->_return_code = value.substr(0, value.find(' '));
		value.erase(0, value.find(' ') + 1);

		this->_return_path = value;
	}
	else
	{
		throw std::runtime_error("Error reading config file. Unknown directive error. (" + key + ")");
	}
}



location::location() : _root(DEFAULT_ROOT), _index(DEFAULT_INDEX),
_cgi(DEFAULT_CGI) {}

location::~location()
{}

location::location(std::string content) : _path(""), _index_file(DEFAULT_INDEX), _save_file(""), _cgi(DEFAULT_CGI),
       _alias(""), _auto_index(false), _cgi_enabled(false), _return_path(""), _return_code("") 
{
	std::stringstream contentStream;
	std::string	line;

	
	contentStream << content;
	contentStream.seekg(0);
	getline(contentStream, line);
	trim_space_newline(line);
	trim_curly_brackets(line);
	if (line.find("location") != std::string::npos)
			line.erase(0, line.find(' ') + 1);
	this->_path = line.substr(0, line.find(' '));
	while (getline(contentStream, line))
	{
		trim_space_newline(line);
		trim_semicolon(line);
		if (line.find('}') != std::string::npos)
		{
			if (line.length() == 1)
				break;
			trim_curly_brackets(line);
		}
		set_priv_attribute(line);	

	}
	// print_location_content();
		

}
