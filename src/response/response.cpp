#include "response.hpp"
#include "cgi.hpp"
#include <sstream>

response::response(request *_request, webserver *_webserver)
{
	this->_webserver = _webserver;
	this->_request = _request;

	this->_http_version = "1.1";
	this->_headers["Server"] = "42 Samusanc/Daviles simple webserver";
	this->_headers["Date"] = get_actual_date();
	this->_body = "";

	this->_error = false;
	this->_keep_alive = false;
	if (this->_request->_error_code != -1)
	{
		if (this->_request->_error_code >= 300 && this->_request->_error_code <= 308)
			this->do_redirection(this->_request->_error_code, this->_request->_debug_msg);
		else
			this->do_error_page(this->_request->_error_code);
	}

	if (this->_request->_cgi_status == NONE)
	{
		if (this->_request->_method == "GET")
			this->do_get();
		else if (this->_request->_method == "POST")
			this->do_post();
		else if (this->_request->_method == "DELETE")
			this->do_delete();
		else if (!_error)
			this->do_error_page(405);
	}
	else
		;
		// this->do_cgi_response();

	this->set_length();
	if (!this->_keep_alive)
		this->_headers["Connection"] = "close";
	else
		this->_headers["Connection"] = "keep-alive";
}

response::~response(){}

//CGI Parser
void	response::do_cgi_response()
{
	if (this->_request->_cgi_response.length() <= 0)
		return ;
	std::cout << "\n\n[David!!]: " << std::endl;
	// std::cout << "\n\n[David!!]: " << this->_request->_cgi_response << std::endl;
	//process_headers
	std::string headers;
	std::string tmp;
	std::stringstream ss;

	ss << this->_request->_cgi_response;
	ss.seekg(0);
	getline(ss, tmp);
	while (!tmp.empty() && tmp != "\r\n\r\n")
	{
		headers += tmp;
		headers += '\n';
		getline(ss, tmp);
		if (ss.eof())
		{
			headers += tmp;
			break;
		}
	}
	utils::fix_spaces_in_line(headers);
	// check if headers is empty?
	// save_headers
	int	i = 0;
	int	flag = 0;
	tmp.clear();

	while (i < headers.length())
	{
		while (headers[i] == ' ')
			i++;
		flag = std::count(headers.begin() + i, headers.begin() + headers.find('\n'), ':');
		if (flag > 0)
		{
			tmp = headers.substr(i, (headers.find(':') - i));
			utils::ft_toLower(tmp);
			i = headers.find(':');
			while (headers[i] == ' ' || headers[i] == ':')
				i++;
			this->_headers[tmp] = headers.substr(i, (headers.find('\n') - i));
		}
		else
		{
			tmp = headers.substr(i, (headers.find('\n') - i));
			utils::ft_toLower(tmp);
			this->_headers[tmp] = "";
		}
		headers.erase(0, headers.find('\n') + 1);
		i = 0;
	}

	//save body
	tmp.clear();
	getline(ss, tmp);
	// std::cout << "\n\nBODY\n" << tmp << std::endl;
	while (!tmp.empty())
	{
		this->_body += tmp;
		this->_body += '\n';
		getline(ss, tmp);
		if (ss.eof())
		{
			this->_body += tmp;
			break;
		}
	}
}

void	response::set_length()
{
	std::stringstream	length;

	length << this->_body.length();
	this->_headers["Content-Length"] = length.str();
}

std::string	html_head(std::string title)
{
	std::stringstream	strm;

	strm << "<!DOCTYPE HTML>\r\n";
	strm << "<html lang=\"en\">\r\n";
	strm << "    <head>\r\n";
	strm << "        <meta charset=\"utf-8\">\r\n";
	strm << "        <title>" << title << "</title>\r\n";
	strm << "    </head>\r\n";
	strm << "    <body>\r\n";
	return (strm.str());
}

std::string html_tail()
{
	std::stringstream	strm;

	strm << "    </body>\r\n";
	strm << "</html>\r\n";
	return (strm.str());
}

std::string	make_error_page_html(int error, std::string message, std::string debug)
{
	std::stringstream	strm;

	strm << html_head("Error response");
	strm << "        <h1>Error response</h1>\r\n";
	strm << "        <p>Error code: " << error << "</p>\r\n";
	strm << "        <p>Message: " << message << "</p>\r\n";
	strm << "        <p>Error code explanation: "<< error << " - " << debug << ".</p>";
	strm << html_tail();
	return (strm.str());
}

std::string	get_file(std::string &path)
{
	std::ifstream	file(path.c_str(), std::ios::binary);
	std::stringstream	buff;
	std::string			result = "";

	if (file.is_open())
	{
		buff << file.rdbuf();
		result = buff.str();
		file.close();
	}
	return (result);
}

void	response::do_error_page(int error)
{
	std::stringstream	debug;

	this->_error = true;
	this->_status_code = error;
	debug << "Error page: " << error << ": " << this->status_message(error);
	utils::print_debug(debug.str());
	this->_headers["Content-Type"] = "text/html;charset=utf-8";
	if (this->_request->_server && !this->_request->_server->_error_pages[error].empty())
	{
		this->_body = this->_request->_server->_error_pages[error];
		if (this->_body.empty())
			this->_body = make_error_page_html(error, this->status_message(error), this->_request->_debug_msg);
	}
	else
		this->_body = make_error_page_html(error, this->status_message(error), this->_request->_debug_msg);
}

std::string	response::get_mimeType(std::string &path)
{
	size_t	dotPos = path.find_last_of('.');

    if (dotPos != std::string::npos) {
        std::string extension = path.substr(dotPos);
        if (this->_webserver->_mime_types.count(extension))
            return this->_webserver->_mime_types[extension];
    }
	return ("application/octet-stream");
}

void	response::get_file(std::string &path)
{
	if (this->_error)
		return ;

	std::ifstream	file(path.c_str(), std::ios::binary);
	std::stringstream	buff;

	if (file.is_open())
	{
		buff << file.rdbuf();
		this->_body = buff.str();
		file.close();
		this->_status_code = 200;
		this->_headers["Content-Type"] = this->get_mimeType(path);
	}
	else
	{
		this->do_error_page(404);
		return ;
	}
}

std::list<std::string> listDirectory(const std::string& path) {
    std::list<std::string> directoryEntries;
    DIR* dir = opendir(path.c_str());
    struct dirent* entry;

    if (dir == NULL)
        return directoryEntries;

    while ((entry = readdir(dir)) != NULL)
	{
        if (std::string(entry->d_name) != "." && std::string(entry->d_name) != "..")
            directoryEntries.push_back(entry->d_name);
    }

    closedir(dir);
    return directoryEntries;
}

bool	is_dir(std::string path)
{
	struct stat pathStat;

	if (stat(path.c_str(), &pathStat) == 0)
	{
		if (S_ISREG(pathStat.st_mode))
			return (false);
		else if (S_ISDIR(pathStat.st_mode))
			return (true);
	}
	return (false);
}

std::string	make_autoindex(std::list<std::string> &files, std::string &path, std::string &uri)
{
	std::list<std::string>::iterator	i = files.begin();
	std::list<std::string>::iterator	ie = files.end();
	std::stringstream					result;
	std::string							tmp;

	result << html_head("Directory listing");
	result << "<h1>Directory listing for " << uri << "</h1>";
	result << "<hr>";
	result << "<ul>";

	for (; i != ie; i++)
	{
		if (is_dir(path + *i))
			tmp = "<li><a href=\"" + *i + "/"+ "\">" + *i + "</a></li>\r\n";
		else
			tmp = "<li><a href=\"" + *i + "\">" + *i + "</a></li>\r\n";
		result << tmp << std::endl;
	}

	result << "</ul>";
	result << "<hr>";
	result << html_tail();

	return (result.str());
}

void	response::get_dir(std::string &path)
{
    std::list<std::string> entries = listDirectory(path);

	std::cout << "[debug this is num]: " << this->_request->_location->_index_file.empty() 
		<< "this is the value: " << this->_request->_location->_index_file << std::endl;
	if (this->_request->_location->_index_file.empty())
	{
		if (this->_request->_location->_auto_index)
		{
			this->_status_code = 200;
			this->_body = make_autoindex(entries, path, this->_request->_uri);
		}
		else
		{
			this->do_error_page(403);
			return ;
		}
	}
	else
	{
		std::string	index_file;

		// this is a tmp index geter, i dont know how to handle the root and alias.
		index_file = "./" + this->_request->_location->_root + "/" + this->_request->_location->_index_file;
		this->get_file(index_file);
	}
}

std::string	cut_spaces(std::string &string)
{
	std::string	copy = string;
	std::string	result = "";
	char		tmp[2];

	tmp[0] = '\0';
	tmp[1] = '\0';
	for (size_t i = 0; i < copy.length(); i++)
	{
		tmp[0] = copy[i];
		if (tmp[0] > 32 && tmp[0] < 126)
			result = result + tmp;
	}
	return (result);
}

void	response::do_redirection(int code, std::string location)
{
	this->_error = true;
	this->_status_code = code;
	this->_headers["Location"] = location;
}

void	response::do_get()
{
	if (!this->_request || this->_error)
		return ;
	// check if this location have get!! permissions
	// to do ^

	std::string	path;
	struct stat pathStat;

	path = "." + this->_request->_uri;
	if (stat(path.c_str(), &pathStat) == 0)
	{
		if (S_ISREG(pathStat.st_mode))
		{
			// regular file
			this->get_file(path);
		}
		else if (S_ISDIR(pathStat.st_mode))
		{
			// directory
			//'/?' 301 redirect to match directory
			this->get_dir(path);
			char	c;

			c = path[path.length() - 1];
			if (c == '/')
				this->get_dir(path);
			else
				this->do_redirection(301, std::string(cut_spaces(this->_request->_uri) + "/"));
		}
		else
		{
			// maybe 404?
			this->do_error_page(404);
			return ;
		}
	}
	else
	{
		this->do_error_page(404);
		return ;
	}
}

void	response::do_post()
{
	std::string	path;
	if (!this->_request || this->_error)
		return ;

	path = "." + this->_request->_uri;

	// this method is cgi's deppendant, so the response came from the cgi
	// not from this, this webserver is not a cgi is a webserver cgi dependant
	// so the only method with post allowed is when the content-type has:
	// multipart/form-data

	// check post permissions

	// check if the content-type from the response has multipart/form-data
	// if not 400

	// try to upload the file!
	// in success return this
	if (true)
	{
		this->_status_code = 201;
		this->_headers["Location"] = path;
	}
	else
	{
		this->do_error_page(403);
		return ;
	}
}

void	response::delete_dir(std::string &path)
{
	if (access(path.c_str(), W_OK) == 0)
	{
		if (rmdir(path.c_str()) == 0)
			this->_status_code = 204;
		else
		{
			this->do_error_page(409);
			return ;
		}
	}
	else
	{
		this->do_error_page(403);
		return ;
	}
}

void	response::delete_file(std::string &path)
{
	if (std::remove(path.c_str()) == 0)
		this->_status_code = 204;
	else
	{
		this->do_error_page(403);
		return ;
	}
}

void	response::do_delete()
{
	if (!this->_request || this->_error)
		return ;

	// check if this location have delete permissions

	std::string	path;
	struct stat pathStat;

	path = "." + this->_request->_uri;
	if (stat(path.c_str(), &pathStat) == 0)
	{
		if (S_ISREG(pathStat.st_mode))
			this->delete_file(path);
		else if (S_ISDIR(pathStat.st_mode))
		{
			// directory
			//'/?' 301
			this->delete_dir(path);
		}
		else
		{
			this->do_error_page(404);
			return ;
		}
	}
	else
	{
		// not found
		this->do_error_page(404);
		return ;
	}

}

std::string	response::print_status_line()
{
	std::stringstream	result;

	result 
	<< "HTTP/" << this->_http_version << " " 
	<< this->_status_code << " " 
	<< this->status_message(this->_status_code) << "\r\n";

	return (result.str());
}

std::string	response::print_headers()
{
	std::stringstream	result;
	std::map<std::string, std::string>::iterator	i = this->_headers.begin();
	std::map<std::string, std::string>::iterator	ie = this->_headers.end();
	ie--;

	for (; ie != i; ie--)
	{
		result << ie->first << ": " << ie->second;
		result << "\r\n";
	}
	result << ie->first << ": " << ie->second;
	result << "\r\n";

	return (result.str());
}

std::string	response::str()
{
	std::stringstream	result;

	result << this->print_status_line();
	result << this->print_headers();

	result << "\r\n";
	result << this->_body;

	return (result.str());
}

std::string response::status_message(int code)
{
	return this->_webserver->status_message(code);
}
