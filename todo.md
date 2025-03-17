## TODO
- [x] Choose the port and host of each server.
- [x] Setup the server_names or not.
- [x] The first server for a host:port will be the default for this host:port (that means it will answer to all the requests that don’t belong to another server).
- [x] Setup default error pages.
- [ ] Limit client body size.
- [ ] Setup routes with one or multiple of the following rules/configurations (routes won't be using regexp):
  - [x] Define a list of accepted HTTP methods for the route.
  - [x] Define an HTTP redirection.
  - [x] Define a directory or a file from where the file should be searched (for example, if URL `/kapouet` is rooted to `/tmp/www`, URL `/kapouet/pouic/toto/pouet` is `/tmp/www/pouic/toto/pouet`).
  - [x] Turn on or off directory listing.
  - [x] Set a default file to answer if the request is a directory.
  - [ ] Execute CGI based on certain file extensions (for example, `.php`).
  - [ ] Make it work with POST and GET methods.
  - [ ] Make the route able to accept uploaded files and configure where they should be saved.
- [x] Do you wonder what a CGI is?
- [ ] Because you won’t call the CGI directly, use the full path as `PATH_INFO`.
- [ ] Just remember that, for chunked requests, your server needs to unchunk it. The CGI will expect EOF as the end of the body.
- [x] Same things for the output of the CGI. If no `content_length` is returned from the CGI, EOF will mark the end of the returned data.
- [ ] Your program should call the CGI with the file requested as the first argument.
- [ ] The CGI should be run in the correct directory for relative path file access.
- [ ] Your server should work with one CGI (php-CGI, Python, etc.).

there is one problem with the root, also i have to check the cgi

