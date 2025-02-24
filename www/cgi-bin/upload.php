#!/usr/bin/php-cgi
<?php
header("Content-Type: text/html");

$uploadDir = "/www/cgi-bin/tmp/uploads/"; // Change if needed
if (!is_dir($uploadDir)) {
    mkdir($uploadDir, 0777, true);
}

if ($_SERVER['REQUEST_METHOD'] === 'POST' && isset($_FILES['file'])) {
    $file = $_FILES['file'];
    $targetPath = $uploadDir . basename($file['name']);

    if (move_uploaded_file($file['tmp_name'], $targetPath)) {
        echo "<h1>File Upload Successful</h1>";
        echo "<p>File: " . htmlspecialchars($file['name']) . "</p>";
        echo "<p>Size: " . $file['size'] . " bytes</p>";
    } else {
        echo "<h1>Upload Failed</h1>";
        echo "<p>Error moving the file.</p>";
    }
} else {
    echo "<h1>No file uploaded</h1>";
}
?>
