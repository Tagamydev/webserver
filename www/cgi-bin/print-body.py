#!/usr/bin/env python3
import sys
import os

# This version request header without spaces, forcing error is not valid.

# def main():
#     # Print headers first
#     print("Content-Type: text/plain\n")
    
#     # Print environment variables for debugging
#     print("Environment Variables:\n")
#     for key, value in os.environ.items():
#         print(f"{key}: {value}")
    
#     print("\n---\n")

#     # Read the request method
#     method = os.environ.get("REQUEST_METHOD", "")

#     # Read and print the request body
#     if method in ["POST", "PUT", "PATCH"]:
#         content_length = os.environ.get("CONTENT_LENGTH", "0")
        
#         if content_length.isdigit():
#             content_length = int(content_length)
#             body = sys.stdin.read(content_length) if content_length > 0 else "(No body received)"
#         else:
#             body = "(Invalid CONTENT_LENGTH)"
#     else:
#         body = "(No body received)"

#     print("Received Body:\n")
#     print(body)

# if __name__ == "__main__":
#     main()

def main():
    # Print response headers
    print("Content-Type: text/plain\n")

    # Read request method
    method = os.environ.get("REQUEST_METHOD", "").strip()

    # Read request body if it's a method that can have a body
    if method in ["POST", "PUT", "PATCH"]:
        content_length = os.environ.get("CONTENT_LENGTH", "0").strip()

        if content_length.isdigit():
            content_length = int(content_length)
            body = sys.stdin.read(content_length) if content_length > 0 else "(No body received)"
        else:
            body = "(CONTENT_LENGTH is not a valid number)"
    else:
        body = "(No body received)"

    # Print received body
    print("Received Body:\n")
    print(body)

if __name__ == "__main__":
    main()
