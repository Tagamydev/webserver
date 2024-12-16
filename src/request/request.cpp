#include "request.hpp"
#include <string>
std::string	read_file(int fd)
{
    const std::size_t bufferSize = 1024;
    char buffer[bufferSize];
    std::string result;

    ssize_t bytesRead;
    while ((bytesRead = read(fd, buffer, bufferSize)) > 0) {
        result.append(buffer, bytesRead);
    }

    if (bytesRead == -1) {
        std::cerr << "Error reading file descriptor." << std::strerror(errno) << std::endl;
    }
	return (result);
}

request::request(int fd)
{
	std::string			file;
	std::stringstream	reqfile;
	std::string			line;

	file = read_file(fd);
	// std::cout << "LINE \n" << file << std::endl;
	reqfile << file;
	reqfile.seekg(0);
	getline(reqfile, line);
	this->check_save_request_line(line);
	this->process_headers(reqfile, line);
	this->process_body(reqfile, line);

	this->parse_headers();


	// this->_http_version = "1.1";
	// this->_uri = "/minecraft.jpg";
	// this->_method = "GET";
}

request::~request(){}



void request::process_body(std::stringstream &reqfile, std::string line)
{
	std::string tmp;

	line.clear();
	getline(reqfile, tmp);
	// if (reqfile.eof())
	// {
	// 	line += tmp;
	// }
	while ( !reqfile.eof())
	{
		line += tmp;
		line += '\n';
		getline(reqfile, tmp);
	}
	std::cout << "\n <<<<<<<<<< Body A >>>>>>>>>\n " << line << std::endl;
	trim_space_newline(line);
	std::cout << "\n <<<<<<<<<< Body B>>>>>>>>>\n " << line << "||" << std::endl;
}

// Headers
/* Function to forbid empty values on headers*/
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
		//check what to do in this case. NGINX accept header without : and value.
		// if(flag)
		// 	std::cout << "Found header without :" << std::endl;
		if (line.find(':') == std::string::npos || line.find(':') > line.find('\n') || flag > 1)
			throw std::runtime_error("400 Bad Request - Found header without :");
			// throw std::runtime_error("400 Bad Request");
		
		
		tmp = line.substr(i, (line.find(':') - i));
		ft_toLower(tmp);
		i = line.find(':') + 1;
		while (line[i] == ' ')
			i++;
		this->_headers[tmp] = line.substr(i,(line.find('\n') - i));
		// Call function is_empty to rejecct empty header
		// is_empty(this->_headers[tmp]);

		line.erase(0, line.find('\n') + 1);
		i = 0;
	}
	print_header();
}


void request::parse_headers()
{
	//check if has body first
	if (!this->_headers.count("host"))
			throw std::runtime_error("400 Bad Request");
	if (this->_headers.count("content-length"))
    {
		if (this->_headers.count("transfer-encoding"))
			throw std::runtime_error("400 Bad Request");
        this->_has_body = true;
        this->_body_length = std::atoi(_headers["content-length"].c_str());
    }
	if (this->_http_version.compare("HTTP/1.0") == 0 && this->_headers.count("transfer-encoding"))
				throw std::runtime_error("400 Bad Request");
	if (this->_headers.count("transfer-encoding") && this->_headers["transfer-encoding"].find_first_of("chunked") != std::string::npos)
	{
		this->_has_body = true;
        this->_chunked_flag = true;
	}


	std::cout << "\n<<<<    Control vars    >>>>" << std::endl;
	std::cout << "body lenght: " << this->_body_length << std::endl;
	std::cout << "Has body : " << this->_has_body << std::endl;
	std::cout << "Is chunked : " << this->_chunked_flag << std::endl;
}


void    request::process_headers(std::stringstream &reqfile, std::string line)
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
}

// Request line

void request::is_valid_httpv(std::string line)
{
 if (line != "HTTP/1.0" && line != "HTTP/1.1")
	throw std::runtime_error("505 HTTP Version Not Supported");
}

/**

* Checks whether the character passed is allowed in a field name
* Characters allowed as specifed in RFC:

"!" / "#" / "$" / "%" / "&" / "'"
/ "*" / "+" / "-" / "." / "^" / "_"
/ "`" / "|" / "~" / 0-9 / A-Z / a-z

**/
void request::is_valid_uri(std::string &line)
{
	std::string::iterator it;
	std::string normalized;
	size_t len = line.length();

	if (line.empty())
		throw std::runtime_error("400 Bad Request");
	// max size?
	if (len > 4000)
		throw std::runtime_error("414 URI Too Long");
	// Check for invalid characters
	// Found this on web (ch >= '#' && ch <= ';') || (ch >= '?' && ch <= '[') || (ch >= 'a' && ch <= 'z') ||
	//	ch == '!' || ch == '=' || ch == ']' || ch == '_' || ch == '~')
	for (it = line.begin(); it < line.end(); it++)
	{
		if (!std::isalnum(*it) && *it != '/' && *it != '.' && *it != '-' && *it != '_' && *it != '%' && *it != ':' && *it != '&' && *it != '?' && *it != '=')
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

void request::check_save_request_line(std::string line)
{
	std::string key;
	std::string delimiter;
	std::map<std::string, std::string> mp;
	
	delimiter = " ";
	this->fix_spaces_in_line(line);

	std::cout << "\n<<<<    Request    >>>>" << std::endl;
	//check method
	if (line.find(delimiter) != std::string::npos)
		key = line.substr(0, line.find(delimiter));
	this->is_valid_method(key);
	this->_method = key;
	line = line.substr((line.find(delimiter) + 1), line.length());
	std::cout << "Method : " << this->_method << std::endl;

	//check URI
	if (line.find(delimiter) == std::string::npos)
		throw std::runtime_error("400 Bad Request");
	key = line.substr(0, line.find(delimiter));
	this->is_valid_uri(key);
	this->_uri = key;
	std::cout << "URI : " << this->_uri << std::endl;
	line = line.substr((line.find(delimiter) + 1), line.length());

	//check HTTP Version
	if (line.find(delimiter) != std::string::npos)
		throw std::runtime_error("400 Bad Request");
	key = line.substr(0, line.length());
	this->is_valid_httpv(key);
	this->_http_version = key;
	std::cout << "HTTP V : " << this->_http_version << std::endl;
}


//assets
void	request::print_header()
{
	std::cout << "\n<<<<    HEADER    >>>>" << std::endl;
	for (std::map<std::string,std::string>::iterator it = this->_headers.begin(); it != this->_headers.end(); it++)
	{
		std::cout << "Key: " << it->first <<  std::endl;
		std::cout << " Value: " << it->second << "" << std::endl;
	}
}