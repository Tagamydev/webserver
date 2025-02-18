#!/bin/bash
$POST_DATA="Hola que tal"
echo "Content-Type: text/plain"
echo ""
echo "this should be body"
echo "WTF is wrong with you!!!"

    
# Read the request body from stdin
if [ "$REQUEST_METHOD" = "POST" ]; then
    read -n "$CONTENT_LENGTH" POST_DATA
    echo "Received form data: $POST_DATA"
fi
