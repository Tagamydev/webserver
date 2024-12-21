#include "response.hpp"

response::response(request &req)
{
	this->set_status_codes_list();
	this->set_mime_types_list();

	this->http_version = "1.1";
	this->request_form = &req;
	this->headers["Server"] = "42 Samusanc/Daviles simple webserver";
	this->headers["Date"] = get_actual_date();

	this->_error = false;
	if (req._error_code != -1)
	{
		this->do_error_page(req._error_code);
		return ;
	}

	if (req._method == "GET")
		this->do_get();
	else if (req._method == "POST")
		this->do_post();
	else if (req._method == "DELETE")
		this->do_delete();
	else
	{
		this->do_error_page(404);
		return ;
	}
}

response::~response(){}

void	response::do_error_page(int error)
{
	this->_error = true;
	std::cout << "Error page { " << error << " }: "; 
	std::cout << this->status_codes_list[error] << "!!" << std::endl;

}

std::string	response::get_mimeType(std::string &path)
{
	size_t	dotPos = path.find_last_of('.');

    if (dotPos != std::string::npos) {
        std::string extension = path.substr(dotPos);
        if (this->mime_types_list.count(extension)) {
            return this->mime_types_list[extension];
        }
    }
	return ("application/octet-stream");
}

void	response::get_file(std::string &path)
{
	if (this->_error)
		return ;

	std::ifstream	file(path.c_str(), std::ios::binary);
	std::stringstream	buff;
	std::stringstream	length;

	if (file.is_open())
	{
		buff << file.rdbuf();
		this->body = buff.str();
		file.close();
		this->status_code = 200;
		this->headers["Content-type"] = this->get_mimeType(path);
		length << this->body.length();
		this->headers["Content-Length"] = length.str();
	}
	else
	{
		this->status_code = 404;

	}

}

std::list<std::string> listDirectory(const std::string& path) {
    std::list<std::string> directoryEntries;
    DIR* dir = opendir(path.c_str());
    struct dirent* entry;

    if (dir == NULL)
	{
        perror("Error al abrir el directorio");
        return directoryEntries;
    }

    while ((entry = readdir(dir)) != NULL)
	{
        if (std::string(entry->d_name) != "." && std::string(entry->d_name) != "..")
            directoryEntries.push_back(entry->d_name);
    }

    closedir(dir);
    return directoryEntries;
}

void	response::do_get()
{
	if (this->_error)
		return ;

	std::string	path;
	struct stat pathStat;

	path = "." + std::string("/minecraft.jpg");//this->request_form->uri;
	if (!request_form)
		return ;
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
			//'/?' 301
    		std::list<std::string> entries = listDirectory(path);
			std::list<std::string>::iterator	i = entries.begin();
			std::list<std::string>::iterator	ie = entries.end();

			for (; i != ie; i++) {
				std::cout << *i << std::endl;
			}
		}
		else
		{
			// maybe 404?

		}
	}
	else
	{
		// not found

	}
}

void	response::do_post()
{
	if (this->_error)
		return ;

	if (!request_form)
		return ;
}

void	response::do_delete()
{
	if (this->_error)
		return ;

	if (!request_form)
		return ;
}

void	response::set_mime_types_list()
{
    this->mime_types_list[".html"] = "text/html";
    this->mime_types_list[".htm"] = "text/html";
    this->mime_types_list[".css"] = "text/css";
    this->mime_types_list[".js"] = "application/javascript";
    this->mime_types_list[".json"] = "application/json";
    this->mime_types_list[".xml"] = "application/xml";
    this->mime_types_list[".txt"] = "text/plain";
    this->mime_types_list[".csv"] = "text/csv";
    this->mime_types_list[".jpg"] = "image/jpeg";
    this->mime_types_list[".jpeg"] = "image/jpeg";
    this->mime_types_list[".png"] = "image/png";
    this->mime_types_list[".gif"] = "image/gif";
    this->mime_types_list[".bmp"] = "image/bmp";
    this->mime_types_list[".webp"] = "image/webp";
    this->mime_types_list[".svg"] = "image/svg+xml";
    this->mime_types_list[".ico"] = "image/vnd.microsoft.icon";
    this->mime_types_list[".woff"] = "font/woff";
    this->mime_types_list[".woff2"] = "font/woff2";
    this->mime_types_list[".ttf"] = "font/ttf";
    this->mime_types_list[".otf"] = "font/otf";
    this->mime_types_list[".eot"] = "application/vnd.ms-fontobject";
    this->mime_types_list[".vtt"] = "text/vtt";
    this->mime_types_list[".mp4"] = "video/mp4";
    this->mime_types_list[".mpeg"] = "video/mpeg";
    this->mime_types_list[".webm"] = "video/webm";
    this->mime_types_list[".avi"] = "video/x-msvideo";
    this->mime_types_list[".mp3"] = "audio/mpeg";
    this->mime_types_list[".wav"] = "audio/wav";
    this->mime_types_list[".ogg"] = "audio/ogg";
    this->mime_types_list[".pdf"] = "application/pdf";
    this->mime_types_list[".zip"] = "application/zip";
    this->mime_types_list[".tar"] = "application/x-tar";
    this->mime_types_list[".gz"] = "application/gzip";
    this->mime_types_list[".7z"] = "application/x-7z-compressed";
    this->mime_types_list[".rar"] = "application/vnd.rar";
    this->mime_types_list[".doc"] = "application/msword";
    this->mime_types_list[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    this->mime_types_list[".xls"] = "application/vnd.ms-excel";
    this->mime_types_list[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
    this->mime_types_list[".ppt"] = "application/vnd.ms-powerpoint";
    this->mime_types_list[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
}

void	response::set_status_codes_list()
{
	this->status_codes_list[100] = "Continue";
	this->status_codes_list[101] = "Switching Protocols";
	this->status_codes_list[102] = "Processing";
	this->status_codes_list[200] = "OK";
	this->status_codes_list[201] = "Created";
	this->status_codes_list[202] = "Accepted";
	this->status_codes_list[203] = "Non-authoritative Information";
	this->status_codes_list[204] = "No Content";
	this->status_codes_list[205] = "Reset Content";
	this->status_codes_list[206] = "Partial Content";
	this->status_codes_list[207] = "Multi-Status";
	this->status_codes_list[208] = "Already Reported";
	this->status_codes_list[226] = "IM Used";
	this->status_codes_list[300] = "Multiple Choices";
	this->status_codes_list[301] = "Moved Permanently";
	this->status_codes_list[302] = "Found";
	this->status_codes_list[303] = "See Other";
	this->status_codes_list[304] = "Not Modified";
	this->status_codes_list[305] = "Use Proxy";
	this->status_codes_list[307] = "Temporary Redirect";
	this->status_codes_list[308] = "Permanent Redirect";
	this->status_codes_list[400] = "Bad Request";
	this->status_codes_list[401] = "Unauthorized";
	this->status_codes_list[402] = "Payment Required";
	this->status_codes_list[403] = "Forbidden";
	this->status_codes_list[404] = "Not Found";
	this->status_codes_list[405] = "Method Not Allowed";
	this->status_codes_list[406] = "Not Acceptable";
	this->status_codes_list[407] = "Proxy Authentication Required";
	this->status_codes_list[408] = "Request Timeout";
	this->status_codes_list[409] = "Conflict";
	this->status_codes_list[410] = "Gone";
	this->status_codes_list[411] = "Length Required";
	this->status_codes_list[412] = "Precondition Failed";
	this->status_codes_list[413] = "Payload Too Large";
	this->status_codes_list[414] = "Request-URI Too Long";
	this->status_codes_list[415] = "Unsupported Media Type";
	this->status_codes_list[416] = "Requested Range Not Satisfiable";
	this->status_codes_list[417] = "Expectation Failed";
	this->status_codes_list[418] = "I’m a teapot";
	this->status_codes_list[421] = "Misdirected Request";
	this->status_codes_list[422] = "Unprocessable Entity";
	this->status_codes_list[423] = "Locked";
	this->status_codes_list[424] = "Failed Dependency";
	this->status_codes_list[426] = "Upgrade Required";
	this->status_codes_list[428] = "Precondition Required";
	this->status_codes_list[429] = "Too Many Requests";
	this->status_codes_list[431] = "Request Header Fields Too Large";
	this->status_codes_list[444] = "Connection Closed Without Response";
	this->status_codes_list[451] = "Unavailable For Legal Reasons";
	this->status_codes_list[499] = "Client Closed Request";
	this->status_codes_list[500] = "Internal Server Error";
	this->status_codes_list[501] = "Not Implemented";
	this->status_codes_list[502] = "Bad Gateway";
	this->status_codes_list[503] = "Service Unavailable";
	this->status_codes_list[504] = "Gateway Timeout";
	this->status_codes_list[505] = "HTTP Version Not Supported";
	this->status_codes_list[506] = "Variant Also Negotiates";
	this->status_codes_list[507] = "Insufficient Storage";
	this->status_codes_list[508] = "Loop Detected";
	this->status_codes_list[510] = "Not Extended";
	this->status_codes_list[511] = "Network Authentication Required";
	this->status_codes_list[599] = "Network Connect Timeout Error";
}

std::string	response::print_status_line()
{
	std::stringstream	result;

	result 
	<< "HTTP/" << this->http_version << " " 
	<< this->status_code << " " 
	<< this->status_codes_list[this->status_code] << "\r\n";

	return (result.str());
}

std::string	response::print_headers()
{
	std::stringstream	result;
	std::map<std::string, std::string>::iterator	i = this->headers.begin();
	std::map<std::string, std::string>::iterator	ie = this->headers.end();

	for (; i != ie; i++) {
		result << i->first << ": " << i->second;
		result << "\r\n";
	}

	return (result.str());
}

std::string	response::str()
{
	std::stringstream	result;

	result << this->print_status_line();
	result << this->print_headers();
	result << "\r\n";
	result << this->body;

	return (result.str());
}

