#include "webserver.hpp"

std::string	save_config_file(std::string &path)
{
	std::string			result;
	std::stringstream	buff;
	std::ifstream		file(path.c_str());

	if (!file)
		throw std::runtime_error("Error reading config file.");
	else
	{
		buff << file.rdbuf();
		file.close();
	}
	result = buff.str();
	return (result);
}

webserver::webserver(std::string &path)
{
	_config_file = save_config_file(path);
}

webserver::~webserver() {}