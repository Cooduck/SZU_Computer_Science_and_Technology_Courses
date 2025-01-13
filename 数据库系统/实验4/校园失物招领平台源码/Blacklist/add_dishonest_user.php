<?php
session_start();
include "../connect.php";

// 验证管理员权限
if (!isset($_SESSION['is_admin'])) {
    echo json_encode(["status" => "error", "message" => "无权限操作"]);
    mysqli_close($con);
    exit;
}

$admin_id = $_SESSION['user_id'];
$user_id = $_POST['user_id'];
$blacked_reason = $_POST['blacked_reason'];

if (!$user_id || !$blacked_reason) {
    echo json_encode(["status" => "error", "message" => "参数不完整"]);
    mysqli_close($con);
    exit;
}

// 检查用户是否已在黑名单中
$query_check = "SELECT 1 FROM dishonestPerson WHERE user_id = ?";
$stmt_check = $con->prepare($query_check);
$stmt_check->bind_param("i", $user_id);
$stmt_check->execute();
$stmt_check->store_result();

if ($stmt_check->num_rows > 0) {
    echo json_encode(["status" => "error", "message" => "用户已在黑名单中"]);
    mysqli_close($con);
    exit;
}

// 更新用户状态为失信人员
$query = "UPDATE user SET user_status = '失信人员' WHERE user_id = ?";
$stmt = $con->prepare($query);
$stmt->bind_param('i', $user_id);

if (!$stmt->execute()) {
    echo json_encode(["status" => "success", "message" => "更新用户状态失败"]);
    mysqli_close($con);
    exit;
}

// 添加用户至黑名单
$query_add = "
INSERT INTO dishonestPerson (user_id, blacked_reason, admin_id) 
VALUES (?, ?, ?)";
$stmt_add = $con->prepare($query_add);
$stmt_add->bind_param("isi", $user_id, $blacked_reason, $admin_id);

if ($stmt_add->execute()) {
    echo json_encode(["status" => "success"]);
} else {
    echo json_encode(["status" => "error", "message" => "添加失败"]);
}

mysqli_close($con);
?>
