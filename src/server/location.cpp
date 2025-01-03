#include "location.hpp"


void	location::print_location_content()
{
	std::cout << "<<<  Location  >>> " << std::endl;
	std::cout << "PATH: " << _path << std::endl;
	std::cout << "root: " << _root << std::endl;
	std::cout << "alias: " << _alias << std::endl;
	if(_auto_index){std::cout << "autoindex: on" << std::endl;}
	else{std::cout << "autoindex: off" << std::endl;}
	std::cout << "index file: " << _index_file << std::endl;
	std::cout << "save file: " << _save_file << std::endl;
	if(_cgi_enabled){std::cout << "cgi: on" << std::endl;}
	else{std::cout << "cgi: off" << std::endl;}
	std::cout << "cgi path: " << _cgi_path << std::endl;
	std::cout << "return code: " << _return_code << std::endl;
	std::cout << "return path: " << _return_path << std::endl;
	if (!_allowed_methods.empty())
		utils::print_vector_content(_allowed_methods, "allowed methods");

}
/// @brief Returns true if the method passed as parameter is found on _allowed_methods
bool		location::is_allowed_method(std::string method)
{
	if (method.empty())
		return (false);
	for (std::vector<std::string>::iterator it = this->_allowed_methods.begin(); it != this->_allowed_methods.end(); it++)
	{
		if (*it == method)
			return (true);
	}
	return (false);
	
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
	if (utils::is_empty(key) || utils::is_empty(value))
	{
		return ;
	}
	if (key == "root")
		this->_root = value;
	else if (key == "alias")
		this->_alias = value;
	else if (key == "allowed_methods" || key == "allow")
		this->_allowed_methods = utils::split_to_vector(value, ' ');
	else if (key == "index")
		this->_index_file = value;
	else if (key == "save_file")
		this->_save_file = value;
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
		throw std::runtime_error("Error reading config file. Unknown directive error. (" + key + ")");
}

//getters
const std::string &location::get_path() const { return this->_path; }



location::location() : _path(""), _index_file(DEFAULT_INDEX), _save_file(""), _cgi_path(DEFAULT_CGI),
       _alias(""), _auto_index(false), _cgi_enabled(false), _return_path(""), _return_code("") {}

location::~location()
{}

location::location(std::string content) : _path(""), _index_file(DEFAULT_INDEX), _save_file(""), _cgi_path(DEFAULT_CGI),
       _alias(""), _auto_index(false), _cgi_enabled(false), _return_path(""), _return_code("") 
{
	std::stringstream contentStream;
	std::string	line;

	
	contentStream << content;
	contentStream.seekg(0);
	getline(contentStream, line);
	utils::trim_space_newline(line);
	utils::trim_curly_brackets(line);
	if (line.find("location") != std::string::npos)
			line.erase(0, line.find(' ') + 1);
	this->_path = line.substr(0, line.find(' '));
	while (getline(contentStream, line))
	{
		utils::trim_space_newline(line);
		utils::trim_semicolon(line);
		if (line.find('{') != std::string::npos)
			utils::trim_curly_brackets(line);
		if (line.find('}') != std::string::npos)
		{
			if (line.length() == 1)
				break;
			utils::trim_curly_brackets(line);
		}
		set_priv_attribute(line);	
	}
	// print_location_content();
	if (this->is_allowed_method("GET"))
		std::cout << "GET" << std::endl;
}
