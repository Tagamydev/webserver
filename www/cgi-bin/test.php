#!/usr/bin/php-cgi
<?php
header("Content-Type: text/html");

// Retrieve query parameters
$name = isset($_GET['name']) ? htmlspecialchars($_GET['name']) : "Guest";

echo "<h1>PHP CGI Test</h1>";
echo "<p>Hello, $name! PHP CGI is working correctly.</p>";
?>
