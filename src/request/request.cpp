#include "request.hpp"

request::request(std::fstream    &reqfile)
{
    std::string line;

    getline(reqfile, line);
    this->check_save_request_line(line);
    this->check_save_headers(reqfile, line);



	// this->http_version = "1.1";
	// this->uri = "/minecraft.jpg";
	// this->method = "GET";
}

request::~request(){}


// Headers
void request::is_empty(std::string &line)
{
	int	i = 0;

	while (line[i] != '\0' && line[i] == ' ' && line[i] == '\n')
		i++;
	if (line.length() == i)
		throw std::runtime_error("400 Bad Request");
}

void request::is_valid_header(std::string &line)
{
	int	i = 0;
	int	flag = 0;
	std::string	tmp;

	while (i < line.length())
	{
		while (line[i] == ' ')
			i++;
		// check : before \n
		if (i < line.size() && line.find('\n') != std::string::npos) 
    		flag = std::count(line.begin() + i, line.begin() + line.find('\n'), ':');
		if (line.find(':') == std::string::npos || line.find(':') > line.find('\n') || flag > 1)
			throw std::runtime_error("400 Bad Request");
		tmp = line.substr(i, (line.find(':') - i));
		i = line.find(':') + 1;
		while (line[i] == ' ')
			i++;
		this->headers[tmp] = line.substr(i,(line.find('\n') - i));
		is_empty(this->headers[tmp]);

		line.erase(0, line.find('\n') + 1);
		i = 0;
	}
	std::cout << "\n<<<<    HEADER    >>>>" << std::endl;
	for (std::map<std::string,std::string>::iterator it = this->headers.begin(); it != this->headers.end(); it++)
	{
		std::cout << "Key: " << it->first << "| Value: " << it->second << "|" << std::endl;
	}
}
void    request::check_save_headers(std::fstream &reqfile, std::string line)
{
    std::string tmp;

	line.clear();
	getline(reqfile, tmp);
	while (!tmp.empty() && tmp != "\r\n\r\n")
	{
		line += tmp;
		line += '\n';
		getline(reqfile, tmp);
		if (reqfile.eof())
		{
			line += tmp;
			break;
		}
	}
	fix_spaces_in_line(line);
	is_valid_header(line);
	// std::cout << "\n<<<<    HEADER    >>>>" << std::endl;
	// std::cout << line << std::endl;

}



// Request line

void request::is_valid_httpv(std::string line)
{
    //should we check separately HTTP from number?
 if (line != "HTTP/1.0" && line != "HTTP/1.1")
    throw std::runtime_error("505 HTTP Version Not Supported");
}

void request::is_valid_uri(std::string &line)
{
    std::string::iterator it;
    std::string normalized;
    size_t len = line.length();

    if (line.empty())
		throw std::runtime_error("400 Bad Request");
    // max size?
    if (len > 2000)
		throw std::runtime_error("414 URI Too Long");
    // Check for invalid characters
    for (it = line.begin(); it < line.end(); it++)
    {
        if (!std::isalnum(*it) && *it != '/' && *it != '.' && *it != '-' && *it != '_' && *it != '%' && *it != ':')
			throw std::runtime_error("400 Bad Request");

    }
    // Check for ../ and //
    for (size_t i = 0; i < len; ++i) 
    {
        if (i + 2 < len && line[i] == '.' && line[i+1] == '.' && line[i+2] == '/')
            i += 2;
        else if (line[i] == '/' && i + 1 < len && line[i + 1] == '/') 
			throw std::runtime_error("400 Bad Request");

        else 
            normalized += line[i];
    }
    if (normalized.empty())
		throw std::runtime_error("400 Bad Request");
    line = normalized;
}
void request::is_valid_method(std::string line)
{
    std::string methods[3] = {"GET", "POST", "DELETE"};

    if (line.empty())
        throw std::runtime_error("405 Method Not Allowed");
    for (size_t i = 0; i < methods->length(); i++)
    {
        if (!line.compare(methods[i]) && line.length() == methods[i].length())
            return ;
    }
	throw std::runtime_error("405 Method Not Allowed");
}

void request::fix_spaces_in_line(std::string &line)
{
    std::string parsedLine;
    int i = 0;

    if (line[0] == ' ' || line[0] == '\t' || line[0] == '\r' || line[0] == '\f')
        while (line[i] == ' ' || line[i] == '\t' || line[i] == '\r' || line[i] == '\f')
            i++;
    while (i != line.length() || line[i] != '\0')
    {
        if (line[i] == ' ' || line[i] == '\t' || line[i] == '\r' || line[i] == '\f')
        {
            while (line[i] == ' ' || line[i] == '\t' || line[i] == '\r' || line[i] == '\f')
               i++;
            if (line[i] == '\0')
                break;
            parsedLine += ' ';
        }
        parsedLine += line[i];
        i++;
    }
    line = parsedLine;
}

bool request::check_save_request_line(std::string line)
{
    // std::string parsedLine;
    // int i = 0;
    std::string key;
    std::string delimiter;
    std::map<std::string, std::string> mp;
    
    delimiter = " ";
    // std::cout << "Line : " << line << "|\n";
    this->fix_spaces_in_line(line);
    // std::cout << "Parsed Line : " << line << "|\n";
    // std::cout << std::endl;

    std::cout << "\n<<<<    Request    >>>>" << std::endl;
    //check method
    if (line.find(delimiter) != std::string::npos)
        key = line.substr(0, line.find(delimiter));
    this->is_valid_method(key);
    this->method = key;
    line = line.substr((line.find(delimiter) + 1), line.length());
    std::cout << "Method : " << this->method << std::endl;

    //check URI
    if (line.find(delimiter) != std::string::npos)
        key = line.substr(0, line.find(delimiter));
    else
        return (std::cerr << "Invalid number of parameters on request line." << std::endl, 1);
    this->is_valid_uri(key);
    this->uri = key;
    std::cout << "URI : " << this->uri << std::endl;
    line = line.substr((line.find(delimiter) + 1), line.length());

    //check HTTP Version
    if (line.find(delimiter) == std::string::npos)
        key = line.substr(0, line.length());
    else
        return (std::cerr << "Invalid number of parameters on request line." << std::endl, 1);
    this->is_valid_httpv(key);
    this->http_version = key;
    std::cout << "HTTP V : " << this->http_version << std::endl;
    return (0);
}