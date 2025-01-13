<?php
session_start();
include "../connect.php";
header('Content-Type: application/json; charset=utf-8');

// 验证登录状态
if (!isset($_SESSION['user_account'])) {
    echo json_encode(["status" => "error", "message" => "未登录用户"]);
    mysqli_close($con);
    mysqli_close($con);
    exit;
}

// 获取前端传递的数据
$data = json_decode(file_get_contents("php://input"), true);
$item_id = $data['item_id'];
$user_id = $_SESSION['user_id'];

if (!$item_id || !$user_id) {
    echo json_encode(["status" => "error", "message" => "传输的信息缺失"]);
    mysqli_close($con);
    exit;
}

// 检查是否已经存在该用户和物品的领回单记录
$check_query = "SELECT COUNT(*) as count FROM ReceiveReceipt WHERE item_id = ? AND user_id = ?";
$check_stmt = $con->prepare($check_query);

if (!$check_stmt) {
    echo json_encode(['status' => 'error', 'message' => 'SQL查询语句准备失败']);
    mysqli_close($con);
    exit;
}

$check_stmt->bind_param("ii", $item_id, $user_id);
$check_stmt->execute();
$check_result = $check_stmt->get_result();
$row = $check_result->fetch_assoc();

if ($row['count'] > 0) {
    echo json_encode(['status' => 'error', 'message' => '不能重复发布领回单']);
    $check_stmt->close();
    mysqli_close($con);
    exit;
}

// 插入领回单记录
$insert_query = "INSERT INTO ReceiveReceipt (item_id, user_id, receiveReceipt_status, receiveReceipt_time) VALUES (?, ?, '未处理', NOW())";

// 使用预处理语句防止SQL注入
$stmt = $con->prepare($insert_query);

if (!$stmt) {
    echo json_encode(['status' => 'error', 'message' => 'SQL语句准备失败']);
    mysqli_close($con);
    exit;
}

$stmt->bind_param("ii", $item_id, $user_id);

if ($stmt->execute()) {
    echo json_encode(['status' => 'success', 'message' => '领回单发布成功']);
} else {
    echo json_encode(['status' => 'error', 'message' => '插入数据库失败']);
}

$stmt->close();

// 关闭数据库连接
mysqli_close($con);
?>
