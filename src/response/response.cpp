#include "response.hpp"
#include "utils.hpp"
#include "cgi.hpp"
#include <sstream>

std::string replaceToken(const std::string &token, const std::string &replacement, const std::string &buffer) {
    std::string result = buffer;
    std::size_t pos = 0;

    while ((pos = result.find(token, pos)) != std::string::npos) {
        result.replace(pos, token.length(), replacement);
        pos += replacement.length(); // Move past the replaced token
    }
    return result;
}

response::response(request *_request, webserver *_webserver)
{
	this->_webserver = _webserver;
	this->_request = _request;

	this->_http_version = "1.1";
	this->_headers["Server"] = "42 Samusanc / Daviles- >> Webserver";
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
		this->set_length();
		if (!this->_keep_alive)
			this->_headers["Connection"] = "close";
		else
			this->_headers["Connection"] = "keep-alive"; // Check connection keep-alive
		return ;
	}

	if (!this->_request->_location->_alias.empty())
	{
		std::string locPath = this->_request->_location->_path;
		std::string alias   = this->_request->_location->_alias;
		std::string uri     = this->_request->_uri;

		if (uri.find(locPath) == 0)
			uri = alias + uri.substr(locPath.length());
		if (uri.empty() || uri[0] != '/')
			uri = "./" + uri;

		this->_request->_uri = uri + "/";
	}
	else if (!this->_request->_location->_root.empty())
	{
		this->_request->_uri = this->_request->_location->_root + this->_request->_uri;
		if (this->_request->_uri.empty() || this->_request->_uri[0] != '/')
			this->_request->_uri = "./" + this->_request->_uri;
	}
	else
		this->_request->_uri = "." + this->_request->_uri;


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
		this->do_cgi_response();

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
	if (this->_request->_cgi_response.empty())
		return;

	std::string headers;
	std::string tmp;
	std::stringstream ss(this->_request->_cgi_response);

	while (getline(ss, tmp))
	{
		if (tmp == "\r" || tmp == "\r\n" || tmp.empty()) // End of headers
			break;
		headers += tmp + '\n';
	}

	utils::fix_spaces_in_line(headers);

	size_t i = 0;
	while (i < headers.length())
	{
		while (i < headers.length() && headers[i] == ' ')
			i++;

		size_t colonPos = headers.find(':', i);
		size_t lineEnd = headers.find('\n', i);

		if (colonPos != std::string::npos && colonPos < lineEnd)
		{
			std::string key = headers.substr(i, colonPos - i);
			utils::ft_to_lower(key); // Normalize header name

			size_t valueStart = colonPos + 1;
			while (valueStart < lineEnd && headers[valueStart] == ' ') // Skip spaces after ':'
				valueStart++;
			std::string value = headers.substr(valueStart, lineEnd - valueStart);

			this->_headers[key] = value; // Store the header
		}
		else 
		{
			std::string key = headers.substr(i, lineEnd - i);
			utils::ft_to_lower(key);
			this->_headers[key] = "";
		}
		i = lineEnd + 1;
	}

	this->_body.clear();
	while (getline(ss, tmp))
	{
		if (!tmp.empty())
			this->_body += tmp + '\n';
	}
	// std::cout << "\n\nResponse Body " << this->_body << std::endl;
	this->_status_code = 200;
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

std::string	str_file(std::string &path)
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
		else
		{
			this->_body = "./" + this->_body;
			this->_body = str_file(this->_body);
		}
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

	if (!this->_request->_location)
	{
		this->_status_code = 200;
			this->_body = make_autoindex(entries, path, this->_request->_uri_file);
		//this->do_error_page(404);
		return;
	}
	if (this->_request->_location->_index_file.empty())
	{
		if (this->_request->_location->_auto_index)
		{
			this->_status_code = 200;
			this->_body = make_autoindex(entries, path, this->_request->_uri_file);
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


	path = this->_request->_uri_file;
	std::cout << "{{{}}}" << path << std::endl;
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

			std::cout << "\n\nHERE PATH: " << path << std::endl;
			c = path[path.length() - 1];
			if (c == '/')
				this->get_dir(path);

			/*
			else
				this->do_redirection(301, std::string(cut_spaces(this->_request->_uri_file) + "/"));
				*/
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

bool	response::post_file(std::string path)
{
	std::ofstream file(path.c_str(), std::ios::out);
	if (!file)
		return (false);
	file << this->_request->_body;
	file.close();
	return (true);
}

void	response::do_post()
{
	std::string	path;
	if (!this->_request || this->_error)
		return ;

	path = this->_request->_uri_file;

	if (this->post_file(path))
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

	path = this->_request->_uri_file;
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
