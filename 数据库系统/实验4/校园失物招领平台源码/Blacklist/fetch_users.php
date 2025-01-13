<?php
session_start();
include "../connect.php";

$query = "
SELECT user_id, user_name, user_account 
FROM user 
WHERE user_id NOT IN (SELECT user_id FROM dishonestPerson)
ORDER BY user_id
";

$result = $con->query($query);

$users = [];
while ($row = $result->fetch_assoc()) {
    $users[] = $row;
}

echo json_encode($users);
mysqli_close($con);
?>
