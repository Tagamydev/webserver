#!/usr/bin/env python3

import cgi
import os

# Directory where uploaded files will be saved
UPLOAD_DIR = "/tmp"

# Set content type
print("Content-Type: text/html\n")

# Parse form data
form = cgi.FieldStorage()

# Check if a file was uploaded
if "file" not in form or not form["file"].filename:
    print("<h2>Error: No file uploaded</h2>")
    exit(0)

# Get uploaded file details
uploaded_file = form["file"]
filename = os.path.basename(uploaded_file.filename)  # Get original filename
file_path = os.path.join(UPLOAD_DIR, filename)  # Save location

# Save the file
try:
    with open(file_path, "wb") as f:
        f.write(uploaded_file.file.read())
    print(f"<h2>File '{filename}' uploaded successfully!</h2>")
    print(f"<p>Saved at: {file_path}</p>")
except Exception as e:
    print(f"<h2>Upload failed</h2><p>{e}</p>")
