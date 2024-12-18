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
	std::stringstream	reqFile;
	std::string			line;

	this->clear();
	file = read_file(fd);
	reqFile << file;
	reqFile.seekg(0);
	getline(reqFile, line);
	while (!reqFile.eof() && line.empty())
		getline(reqFile, line);
	this->check_save_request_line(line);
	this->process_headers(reqFile, line);
	this->process_body(reqFile, line);
	this->parse_headers();

	print_request();
	print_header();
	print_body();
	
	print_others();
}

request::~request(){}

void	request::clear()
{
	this->_fd = -1;
	this->_method.clear();
	this->_uri.clear();
	this->_http_version.clear();
	this->_headers.clear();
	this->_body.clear();
	this->_body_length = -1;
	this->_has_body = -1;
	this->_chunked_flag = false;
	this->_error_code = -1;
}

void request::process_body(std::stringstream &reqFile, std::string line)
{
	std::string tmp;

	line.clear();
	getline(reqFile, tmp);
	while (!reqFile.eof())
	{
		line += tmp;
		line += '\n';
		getline(reqFile, tmp);
	}
	line += tmp;
	trim_space_newline(line);
	if (is_empty(line))
	{
		this->_has_body = 0;
		return ;
	}
	this->_has_body = 1;
	this->_body = line;
	//clear line?
}

// Headers

void request::parse_headers()
{
	if (!this->_headers.count("host"))
			throw std::runtime_error("400 Bad Request");
	if (this->_headers.count("content-length"))
    {
		if (this->_headers.count("transfer-encoding"))
			throw std::runtime_error("400 Bad Request");
		//Check how it works nginx
		this->_body_length = std::atoi(_headers["content-length"].c_str());
		//check if return of atoi is number?
		if (this->_body_length <= 0)
			throw std::runtime_error("400 Bad Request");
    }
	if (this->_http_version.compare("HTTP/1.0") == 0 && this->_headers.count("transfer-encoding"))
				throw std::runtime_error("400 Bad Request");
	if (this->_headers.count("transfer-encoding") && this->_headers["transfer-encoding"].find_first_of("chunked") != std::string::npos)
        this->_chunked_flag = 1;
}

bool request::space_in_header_name(std::string line)
{
	int	i = 0;

	while (line[i] != '\0')
	{
		if (line[i] == ' ' || line[i] == '\t')
			return (1);
		i++;
	}
	return(0);
}

/* @brief save formated headers in map
*/
void request::save_headers(std::string &line)
{
	int	i = 0;
	int	flag = 0;
	std::string	tmp;

	while (i < line.length())
	{
		while (line[i] == ' ')
			i++;
		if (i < line.size() && line.find('\n') != std::string::npos) 
			flag = std::count(line.begin() + i, line.begin() + line.find('\n'), ':');
		if (line.find(':') == std::string::npos || line.find(':') > line.find('\n') || flag > 1)
			throw std::runtime_error("400 Bad Request - Found header without :");
		tmp = line.substr(i, (line.find(':') - i));
		trim_space_newline(tmp);
		if(space_in_header_name(tmp))
			throw std::runtime_error("400 Bad Request - Found space on header name.");
		ft_toLower(tmp);
		i = line.find(':') + 1;
		while (line[i] == ' ')
			i++;
		this->_headers[tmp] = line.substr(i,(line.find('\n') - i));
		line.erase(0, line.find('\n') + 1);
		i = 0;
	}
	
}


void    request::process_headers(std::stringstream &reqFile, std::string line)
{
	std::string tmp;

	line.clear();
	getline(reqFile, tmp);
	while (!tmp.empty() && tmp != "\r\n\r\n")
	{
		line += tmp;
		line += '\n';
		getline(reqFile, tmp);
		if (reqFile.eof())
		{
			line += tmp;
			break;
		}
	}
	fix_spaces_in_line(line);
	save_headers(line);
}

// Request line

void request::is_valid_httpv(std::string line)
{
 if (line != "HTTP/1.0" && line != "HTTP/1.1")
	throw std::runtime_error("505 HTTP Version Not Supported");
}

void	request::process_uri(std::string line)
{
	if (line.find('?') == std::string::npos)
	{
		this->_uri_file = line;
		return ;
	}
	this->_uri_file = line.substr(0, line.find('?'));
	this->_uri_params = line.substr(line.find('?') + 1, line.length());
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
	if (len > MAX_URI_LENGTH)
		throw std::runtime_error("414 URI Too Long");
	// Check for invalid characters
	for (it = line.begin(); it < line.end(); it++)
	{
		if (!std::isalnum(*it) && *it != '/' && *it != '.' && *it != '-' && *it != '_' && *it != '%' \
		&& *it != ':' && *it != '&' && *it != '?' && *it != '!' && *it != '=' && *it != '+' \
		&& *it != '#' && *it != '*' && *it != '$' && *it != '\'' && *it != '^' && *it != '`' \
		&& *it != '|' && *it != '~')
			throw std::runtime_error("400 Bad Request - Uri invalid character.");

	}
	// Check for ../ and //
	for (size_t i = 0; i < len; ++i) 
	{
		if (i + 2 < len && line[i] == '.' && line[i+1] == '.' && line[i+2] == '/')
			i += 2;
		else if (i + 1 < len && line[i] == '/' && line[i + 1] == '/') 
			continue ;
		else 
			normalized += line[i];
	}
	if (normalized.empty())
		throw std::runtime_error("400 Bad Request");
	line = normalized;
	process_uri(line);
}

void request::is_valid_method(std::string line)
{
	std::string methods[3] = {"GET", "POST", "DELETE"};

	if (line.empty())
		throw std::runtime_error("400 Bad Request - Found space at first line.");
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

	if (line[0] == ' ' || line[0] == '\t' || line[0] == '\r' || line[0] == '\f' || line[0] == '\n')
		while (line[i] == ' ' || line[i] == '\t' || line[i] == '\r' || line[i] == '\f' || line[i] == '\n')
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

	//check method
	if (line.find(delimiter) != std::string::npos)
		key = line.substr(0, line.find(delimiter));
	this->is_valid_method(key);
	this->_method = key;
	line = line.substr((line.find(delimiter) + 1), line.length());

	//check URI
	if (line.find(delimiter) == std::string::npos)
		throw std::runtime_error("400 Bad Request");
	key = line.substr(0, line.find(delimiter));
	this->is_valid_uri(key);
	this->_uri = key;
	line = line.substr((line.find(delimiter) + 1), line.length());

	//check HTTP Version
	if (line.find(delimiter) != std::string::npos)
		throw std::runtime_error("400 Bad Request");
	key = line.substr(0, line.length());
	this->is_valid_httpv(key);
	this->_http_version = key;

}


//assets
void	request::print_request()
{
	std::cout << "\n<<<<    Request    >>>>" << std::endl;
	std::cout << "Method : " << this->_method << std::endl;
	std::cout << "URI : " << this->_uri << std::endl;
	std::cout << "HTTP V : " << this->_http_version << std::endl;
	std::cout << "Uri_file : " << this->_uri_file << std::endl;
	std::cout << "Uri_params : " << this->_uri_params << std::endl;
}

void	request::print_header()
{
	std::cout << "\n<<<<    HEADER    >>>>" << std::endl;
	for (std::map<std::string,std::string>::iterator it = this->_headers.begin(); it != this->_headers.end(); it++)
	{
		std::cout << "Key: " << it->first <<  std::endl;
		std::cout << " Value: " << it->second << "" << std::endl;
	}
}

void	request::print_body()
{
	std::cout << "\n<<<<    BODY    >>>>" << std::endl;
	std::cout << this->_body <<  std::endl;
}
void	request::print_others()
{
std::cout << "\n<<<<    Control vars    >>>>" << std::endl;
	std::cout << "body lenght: " << this->_body_length << std::endl;
	std::cout << "Has body : " << this->_has_body << std::endl;
	std::cout << "Is chunked : " << this->_chunked_flag << std::endl;
}