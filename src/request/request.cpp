#include "request.hpp"

request::request(int fd)
{
	this->http_version = "1.1";
	this->uri = "/minecraft.jpg";
	this->method = "GET";
}

request::~request(){}

void request::fix_spaces_in_line(std::string &line)
{
    std::string parsedLine;
    int i = 0;

    if (line[0] == ' ')
        while (line[i] == ' ')
            i++;
    while (i != line.length() || line[i] != '\0')
    {
        if (line[i] == ' ')
        {
            while (line[i] == ' ')
               i++;
            if (line[i] == '\0')
                break;
            parsedLine += ' ';
        }
        parsedLine += line[i];
        i++;
    }
    //check leaks
    line = parsedLine;
}

int request::check_startline(std::string line)
{
    // std::string parsedLine;
    // int i = 0;
    std::string key;
    std::string delimiter;
    std::map<std::string, std::string> mp;
    
    delimiter = " ";
    std::cout << "Line :" << line << "|\n";
    //parse line? fix spaces
    // if (line[0] == ' ')
    //     while (line[i] == ' ')
    //         i++;
    // while (i != line.length() || line[i] != '\0')
    // {
    //     if (line[i] == ' ')
    //     {
    //         while (line[i] == ' ')
    //            i++;
    //         if (line[i] == '\0')
    //             break;
    //         parsedLine += ' ';
    //     }
    //     parsedLine += line[i];
    //     i++;
    // }
    this->fix_spaces_in_line(line);
    std::cout << "Parsed Line: " << line << "|\n";
    std::cout << std::endl;


    while (line.find(delimiter) != std::string::npos)
    {
    // std::cout << "Line :" << line << "\n";
    key = line.substr(0, line.find(delimiter));
    std::cout << "Key :" << key << "\n";
    // redefine line?
    line = line.substr((line.find(delimiter) + 1), line.length());
	// nextKey = line.substr((line.find(delimiter) + 1), line.length());
    }
    key = line;
    std::cout << "Key :" << key << "\n";
    std::cout << std::endl;
    return (0);
}