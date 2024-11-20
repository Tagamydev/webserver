#include "request.hpp"

request::request(int fd)
{
	this->http_version = "1.1";
	this->uri = "/minecraft.jpg";
	this->method = "GET";
}

request::~request(){}


// Headers
bool    request::check_save_headers(std::string line)
{
    return(true);
}



// Request line

bool request::is_valid_httpv(std::string line)
{
    //should we check separately HTTP from number?
 if (line != "HTTP/1.0" && line != "HTTP/1.1")
    return (false);
 return(true);
}

bool request::is_valid_uri(std::string &line)
{
    std::string::iterator it;
    std::string normalized;
    size_t len = line.length();

    if (line.empty())
        return (false);
    // max size?
    if (len > 2000)
        return (false);
    // Check for invalid characters
    for (it = line.begin(); it < line.end(); it++)
    {
        if (!std::isalnum(*it) && *it != '/' && *it != '.' && *it != '-' && *it != '_' && *it != '%' && *it != ':')
            return (false);
    }
    // Check for ../ and //
    for (size_t i = 0; i < len; ++i) 
    {
        if (i + 2 < len && line[i] == '.' && line[i+1] == '.' && line[i+2] == '/')
            i += 2;
        else if (line[i] == '/' && i + 1 < len && line[i + 1] == '/') 
            return (false);
        else 
            normalized += line[i];
    }
    if (normalized.empty())
        return (false);
    line = normalized;
    return (true);
}
bool request::is_valid_method(std::string line)
{
    std::string methods[3] = {"GET", "POST", "DELETE"};

    if (line.empty())
        return (false);
    for (size_t i = 0; i < methods->length(); i++)
    {
        if (!line.compare(methods[i]) && line.length() == methods[i].length())
            return (true);
    }
    std::cerr << "Invalid method name." << std::endl;
    return(false);
}

void request::fix_spaces_in_line(std::string &line)
{
    std::string parsedLine;
    int i = 0;

    if (std::isspace(line[0]))
        while (std::isspace(line[i]))
            i++;
    while (i != line.length() || line[i] != '\0')
    {
        if (std::isspace(line[i]))
        {
            while (std::isspace(line[i]))
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
    if (!this->is_valid_method(key))
        return (1); //error printed in function. Check cout error?
    this->method = key;
    line = line.substr((line.find(delimiter) + 1), line.length());
    std::cout << "Method : " << this->method << std::endl;

    //check URI
    if (line.find(delimiter) != std::string::npos)
        key = line.substr(0, line.find(delimiter));
    else
        return (std::cerr << "Invalid number of parameters on request line." << std::endl, 1);
    if (!this->is_valid_uri(key))
    {
        std::cerr << "Invalid URI path." << std::endl;
        return (1);
    }
    this->uri = key;
    std::cout << "URI : " << this->uri << std::endl;
    line = line.substr((line.find(delimiter) + 1), line.length());

    //check HTTP Version
    if (line.find(delimiter) == std::string::npos)
        key = line.substr(0, line.length());
    else
        return (std::cerr << "Invalid number of parameters on request line." << std::endl, 1);
    if (!this->is_valid_httpv(key))
    {
        std::cerr << "Invalid HTTP version." << std::endl;
        return (1);
    }
    this->http_version = key;
    std::cout << "HTTP V : " << this->http_version << std::endl;
    return (0);
}