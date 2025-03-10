#!/usr/bin/env python3
import sys
import os

def main():
    # Print headers first
    print("Content-Type: text/plain\n")
    
    # Print environment variables for debugging
    print("Environment Variables:\n")
    for key, value in os.environ.items():
        print(f"{key}: {value}")
    
    print("\n---\n")

    # Read the request method
    method = os.environ.get("REQUEST_METHOD", "")

    # Read and print the request body
    if method in ["POST", "PUT", "PATCH"]:
        content_length = os.environ.get("CONTENT_LENGTH", "0")
        
        if content_length.isdigit():
            content_length = int(content_length)
            body = sys.stdin.read(content_length) if content_length > 0 else "(No body received)"
        else:
            body = "(Invalid CONTENT_LENGTH)"
    else:
        body = "(No body received)"

    print("Received Body:\n")
    print(body)

if __name__ == "__main__":
    main()
