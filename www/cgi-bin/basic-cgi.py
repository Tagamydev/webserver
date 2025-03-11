#!		/usr/bin/env python3
import os
import urllib.parse

print("Content-type: text/html\n")
print("<html>")
print("<head><title>CGI Script</title></head>")
print("<body>")

query_string = os.environ.get("QUERY_STRING", "")
parameters = urllib.parse.parse_qs(query_string)

print("<h1>Query String Parameters:</h1>")
print("<ul>")
for key, values in parameters.items():
	for value in values:
		print(f"<li>{key}: {value}</li>")
print("</ul>")

print("</body>")

print("</html>")

