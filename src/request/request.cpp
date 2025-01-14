#include "request.hpp"
#include "utils.hpp"
#include <string>


/* while on reqFile to skip new lines at the begining of the request.
*/
request::request(client *_client, webserver *_webserver)
{
	std::string			file;
	std::stringstream	reqFile;
	std::string			line;
	int					fd;

	fd = _client->get_fd();

	this->_cgi = NULL;
	this->clear();
	this->_cgi_status = NONE;

	file = utils::read_file(fd);
	std::cout << file << std::endl;
	reqFile << file;
	reqFile.seekg(0);
	getline(reqFile, line);
	while (!reqFile.eof() && line.empty())
		getline(reqFile, line);
	this->check_save_request_line(line);
	if (this->_error_code == -1)
		this->process_headers(reqFile, line);
	if (this->_error_code == -1)
		this->process_body(reqFile, line);
	if (this->_error_code == -1)
		this->parse_headers();

	print_request();
	print_header();
	print_body();
	print_others();

	if (this->check_if_cgi())
	{
		this->_cgi = new cgi(*this, _client);
	}
}

request::~request()
{
	if (this->_cgi)
		delete this->_cgi;
}

bool	request::check_if_cgi()
{
	// tmp we assume all is a cgi!
	return (false);
}

void	request::close_cgi()
{
	delete this->_cgi;
	this->_cgi_status = NONE;
}

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
	this->_debug_msg.clear();
}
// Handle body

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
	utils::trim_space_newline(line);
	if (utils::is_empty(line))
	{
		this->_has_body = 0;
		return ;
	}
	this->_has_body = 1;
	this->_body = line;
	//clear line?
}

// Handle Headers

void request::parse_headers()
{
	if (!this->_headers.count("host"))
			return (set_error_code(400, "Host header not found."));
	if (this->_headers.count("content-length"))
    {
		if (this->_headers.count("transfer-encoding"))
			return (set_error_code(400, "Incopatible headers: content-length & transfer-encoding."));
		this->_body_length = std::atoi(_headers["content-length"].c_str());
		if (this->_body_length <= 0)
			return (set_error_code(400, "Invalid Content-length header."));
    }
	if (this->_http_version.compare("HTTP/1.0") == 0 && this->_headers.count("transfer-encoding"))
		return (set_error_code(400, "Invalid header transfer-encoding on HTTP/1.0."));
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
		if (i >= line.size() || line.find('\n') == std::string::npos) 
			return (set_error_code(-1, "No error, should skip headers."));
		flag = std::count(line.begin() + i, line.begin() + line.find('\n'), ':');
		if (flag > 0)
		{
			tmp = line.substr(i, (line.find(':') - i));
			if(space_in_header_name(tmp))
				return (set_error_code(400, "Found space on header name."));
			utils::ft_toLower(tmp);
			i = line.find(':');
			while (line[i] == ' ' || line[i] == ':')
				i++;
			this->_headers[tmp] = line.substr(i, (line.find('\n') - i));
		}
		else
		{
			tmp = line.substr(i, (line.find('\n') - i));
			if(space_in_header_name(tmp))
				return (set_error_code(400, "Found space on header name."));
			utils::ft_toLower(tmp);
			this->_headers[tmp] = "";
		}
		line.erase(0, line.find('\n') + 1);
		i = 0;
	}
	/*
	*/
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
	utils::fix_spaces_in_line(line);
	// check if line is empty?
	save_headers(line);
}

// Request (first) line

void request::is_valid_httpv(std::string line)
{
 if (line != "HTTP/1.0" && line != "HTTP/1.1")
	return (set_error_code(505, "HTTP Version Not Supported"));
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
		return (set_error_code(400, "Empty or invalid URI."));
	// max size?
	if (len > MAX_URI_LENGTH)
		return (set_error_code(414, "Max. URI length reached."));
	// Check for invalid characters
	for (it = line.begin(); it < line.end(); it++)
	{
		if (!std::isalnum(*it) && *it != '/' && *it != '.' && *it != '-' && *it != '_' && *it != '%' \
		&& *it != ':' && *it != '&' && *it != '?' && *it != '!' && *it != '=' && *it != '+' \
		&& *it != '#' && *it != '*' && *it != '$' && *it != '\'' && *it != '^' && *it != '`' \
		&& *it != '|' && *it != '~')
			return (set_error_code(400, "Invalid character found on URI."));
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
		return (set_error_code(400, "Empty or invalid URI."));
	line = normalized;
	process_uri(line);
}

void request::is_valid_method(std::string line)
{
	std::string methods[3] = {"GET", "POST", "DELETE"};

	if (line.empty())
			return (set_error_code(400, "Found space at first line."));
	for (size_t i = 0; i < methods->length(); i++)
	{
		if (!line.compare(methods[i]) && line.length() == methods[i].length())
			return ;
	}
	return (set_error_code(405, "Method Not Allowed."));
}

void request::check_save_request_line(std::string line)
{
	std::string key;
	std::map<std::string, std::string> mp;
	
	if (line[0] == ' ')
		return (set_error_code(400, "Found spaces before method."));
	utils::fix_spaces_in_line(line);
	//check method
	if (line.find(" ") != std::string::npos)
		key = line.substr(0, line.find(" "));
	else
		return (set_error_code(400, "No spaces on first line."));
	this->is_valid_method(key);
	this->_method = key;
	line = line.substr((line.find(" ") + 1), line.length());

	//check URI
	if (line.find(" ") == std::string::npos)
		return (set_error_code(400, "No spaces after method."));
	key = line.substr(0, line.find(" "));
	this->is_valid_uri(key);
	this->_uri = key;
	line = line.substr((line.find(" ") + 1), line.length());

	//check HTTP Version
	if (line.find(" ") != std::string::npos)
		return (set_error_code(400, "No spaces after URI."));
	key = line.substr(0, line.length());
	this->is_valid_httpv(key);
	this->_http_version = key;

}

// Handle errors
void request::set_error_code(int code, std::string msg)
{
	this->_error_code = code;
	this->_debug_msg = msg;	
}


// Utils

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
	std::cout << "Has body: " << this->_has_body << std::endl;
	std::cout << "Is chunked: " << this->_chunked_flag << std::endl;
	std::cout << "Error code: " << this->_error_code << std::endl;
	std::cout << "Debug Message: " << this->_debug_msg << std::endl;
}
