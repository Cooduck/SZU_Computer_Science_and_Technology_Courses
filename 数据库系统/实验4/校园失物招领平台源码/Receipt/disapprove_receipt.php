<?php
session_start();
include "../connect.php";
header('Content-Type: application/json; charset=utf-8');

$data = json_decode(file_get_contents("php://input"), true);
$receiveReceipt_id = $data['receiveReceipt_id'];
$blacked_reason = "该用户冒领招领物品"; 
$admin_id = $_SESSION['user_id'];

if (!$receiveReceipt_id) {
    echo json_encode(["status" => "error", "message" => "传输的信息缺失"]);
    mysqli_close($con);
    exit;
}

try {
    // 更新领回单状态为审核不通过
    $query = "UPDATE ReceiveReceipt SET receiveReceipt_status = '审核不通过' WHERE receiveReceipt_id = ?";
    $stmt = $con->prepare($query);
    $stmt->bind_param('i', $receiveReceipt_id);

    if (!$stmt->execute()) {
        throw new Exception("更新领回单状态失败");
    }

    // 查询用户ID
    $query = "SELECT user_id FROM ReceiveReceipt WHERE receiveReceipt_id = ?";
    $stmt = $con->prepare($query);
    $stmt->bind_param('i', $receiveReceipt_id);
    if (!$stmt->execute()) {
        throw new Exception("查询用户ID失败". $stmt->error);
    }
    $result = $stmt->get_result();

    if ($result->num_rows > 0) {
        $row = $result->fetch_assoc();
        $user_id = $row['user_id'];

        // 检查用户是否已经在黑名单中
        $query = "SELECT COUNT(*) AS count FROM dishonestPerson WHERE user_id = ?";
        $stmt = $con->prepare($query);
        $stmt->bind_param('i', $user_id);
        $stmt->execute();
        $result = $stmt->get_result();
        $row = $result->fetch_assoc();

        if ($row['count'] > 0) {
            throw new Exception("该用户已在黑名单中");
        }

        // 将用户加入黑名单
        $query = "INSERT INTO dishonestPerson (user_id, added_time, blacked_reason, admin_id) VALUES (?, NOW(), ?, ?)";
        $stmt = $con->prepare($query);
        $stmt->bind_param('isi', $user_id, $blacked_reason, $admin_id);
        $stmt->execute();   

        // 更新用户状态为失信人员
        $query = "UPDATE user SET user_status = '失信人员' WHERE user_id = ?";
        $stmt = $con->prepare($query);
        $stmt->bind_param('i', $user_id);

        if (!$stmt->execute()) {
            throw new Exception("更新用户状态失败");
        }

        // 提交事务
        $con->commit();

        echo json_encode(["status" => "success", "message" => "审核不通过处理完成"]);
    } else {
        throw new Exception("未找到对应的用户ID");
    }
} catch (Exception $e) {
    // 事务回滚
    $con->rollback();

    echo json_encode(["status" => "error", "message" => $e->getMessage()]);
}

// 关闭连接
mysqli_close($con);
?>
