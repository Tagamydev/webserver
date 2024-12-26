#!/usr/bin/env python3

import cgi

# Output headers required by CGI
print("Content-Type: text/html")  # Specifies HTML content
print()  # Blank line separates headers from the body

# HTML Content
print("""
<!DOCTYPE html>
<html>
<head>
    <title>Simple CGI Script</title>
</head>
<body>
    <h1>Hello, World!</h1>
    <p>This is a simple CGI script written in Python.</p>
</body>
</html>
""")

