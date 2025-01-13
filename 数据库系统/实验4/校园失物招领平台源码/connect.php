<?php 
$hostname = "localhost"; //local hist
$database = "Campus_Lost_and_Found_Platform"; //database name 
$username = "root"; //user
$password = ""; //password
$con = mysqli_connect($hostname, $username, $password, $database); 
mysqli_set_charset($con, "utf8mb4");
if (mysqli_connect_errno()) 
{
  echo "Failed to connect to MySQL: " . mysqli_connect_error();
  exit();
}
?>