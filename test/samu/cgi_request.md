A **CGI (Common Gateway Interface)** is used in a web server to dynamically generate responses to client requests by running external programs or scripts (e.g., in Python, Perl, PHP, etc.). CGI is often invoked when a client request requires server-side processing beyond just serving static files. 

Here are some examples of requests that typically require a CGI in a web server:

---

### 1. **Form Submission**
When a client submits a form, the server can process the form data using a CGI script.

#### Example:
A user submits a form on a webpage:

```html
<form action="/cgi-bin/form_handler.cgi" method="POST">
  <label for="name">Name:</label>
  <input type="text" id="name" name="name">
  <button type="submit">Submit</button>
</form>
```

The web server receives the POST request and passes the form data to `form_handler.cgi`, which processes the input (e.g., saves it to a database or performs some calculations) and generates a response.

Request:
```
POST /cgi-bin/form_handler.cgi HTTP/1.1
Content-Type: application/x-www-form-urlencoded
Content-Length: 20

name=John+Doe
```

Response (from `form_handler.cgi`):
```
HTTP/1.1 200 OK
Content-Type: text/html

<html>
  <body>
    <h1>Thank you, John Doe!</h1>
  </body>
</html>
```

---

### 2. **Dynamic Content Generation**
A CGI script can dynamically generate HTML content based on user input or server-side data.

#### Example:
A URL like `/cgi-bin/weather.cgi?city=London` can trigger a CGI script (`weather.cgi`) to fetch the weather data for London and generate a response:

Request:
```
GET /cgi-bin/weather.cgi?city=London HTTP/1.1
```

The script might query a weather API and return something like:

Response:
```
HTTP/1.1 200 OK
Content-Type: text/html

<html>
  <body>
    <h1>Weather in London</h1>
    <p>15°C, Clear skies</p>
  </body>
</html>
```

---

### 3. **File Upload**
When a client uploads a file, the server can use a CGI script to handle the uploaded data.

#### Example:
```html
<form action="/cgi-bin/upload.cgi" method="POST" enctype="multipart/form-data">
  <label for="file">Upload a file:</label>
  <input type="file" id="file" name="file">
  <button type="submit">Upload</button>
</form>
```

Request (raw content includes the uploaded file):
```
POST /cgi-bin/upload.cgi HTTP/1.1
Content-Type: multipart/form-data; boundary=----WebKitFormBoundary
Content-Length: ...

------WebKitFormBoundary
Content-Disposition: form-data; name="file"; filename="example.txt"
Content-Type: text/plain

This is the content of the file.
------WebKitFormBoundary--
```

The CGI script `upload.cgi` processes the file and stores it on the server, returning a response indicating success.

---

### 4. **Database Queries**
When a client requests data from a database, a CGI script can act as a middleware to retrieve and format the data.

#### Example:
A request like `/cgi-bin/user_info.cgi?user_id=123` can invoke `user_info.cgi`, which queries a database for information about the user with ID 123.

Response:
```html
<html>
  <body>
    <h1>User Info</h1>
    <p>Name: Jane Doe</p>
    <p>Email: jane@example.com</p>
  </body>
</html>
```

---

### 5. **Search Engine**
A CGI script can process search queries and return results.

#### Example:
A search form sends a query to `/cgi-bin/search.cgi`.

Request:
```
GET /cgi-bin/search.cgi?query=webserver HTTP/1.1
```

The CGI script searches a database or index for the term `webserver` and generates an HTML page with the results.

Response:
```html
<html>
  <body>
    <h1>Search Results for "webserver"</h1>
    <ul>
      <li><a href="/doc1.html">Understanding Web Servers</a></li>
      <li><a href="/doc2.html">Building a Web Server in C++</a></li>
    </ul>
  </body>
</html>
```

---

### Summary:
Requests that require a CGI typically involve:
- **Processing user input** (e.g., forms, file uploads).
- **Fetching dynamic data** (e.g., from a database or API).
- **Performing server-side logic** (e.g., calculations, condition-based responses).
- **Handling complex interactions** (e.g., search engines, report generation).

Let me know if you'd like guidance on implementing CGI handling in your web server!
