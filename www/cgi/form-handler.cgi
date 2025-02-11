#!/bin/bash

echo "Content-Type: text/plain"
echo ""

# Read the request body from stdin
if [ "$REQUEST_METHOD" = "POST" ]; then
    read -n "$CONTENT_LENGTH" POST_DATA
    echo "Received form data: $POST_DATA"
fi
