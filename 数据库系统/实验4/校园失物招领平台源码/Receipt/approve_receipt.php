<?php
session_start();
include "../connect.php";
header('Content-Type: application/json; charset=utf-8');

$data = json_decode(file_get_contents("php://input"), true);
$receiveReceipt_id = $data['receiveReceipt_id'];

if (!$receiveReceipt_id) {
    echo json_encode(["status" => "error", "message" => "传输的信息缺失"]);
    mysqli_close($con);
    exit;
}

if ($receiveReceipt_id) {
    // 更新领回单状态
    $query = "UPDATE ReceiveReceipt SET receiveReceipt_status = '审核通过' WHERE receiveReceipt_id = ?";
    $stmt = $con->prepare($query);
    $stmt->bind_param('i', $receiveReceipt_id);

    if (!$stmt->execute()) {
        // 更新失败
        echo json_encode(["status" => "error", "message" => "更新领回单状态"]);
        mysqli_close($con);
        exit;
    }

    // 查询对应的 item_id
    $query = "SELECT item_id FROM ReceiveReceipt WHERE receiveReceipt_id = ?";
    $stmt = $con->prepare($query);
    $stmt->bind_param('i', $receiveReceipt_id);
    $stmt->execute();
    $result = $stmt->get_result();

    if ($result->num_rows > 0) {
        $row = $result->fetch_assoc();
        $item_id = $row['item_id'];

        // 更新物品状态为已处理
        $query = "UPDATE item SET item_status = '已处理' WHERE item_id = ?";
        $stmt = $con->prepare($query);
        $stmt->bind_param('i', $item_id);

        if (!$stmt->execute()) {
            echo json_encode(["status" => "error", "message" => "更新物品状态失败"]);
            mysqli_close($con);
            exit;
        }

        // 提交事务
        $con->commit();

        echo json_encode(["status" => "success", "message" => "审核通过处理完成"]);
        mysqli_close($con);
        exit;
    } else {
        echo json_encode(["status" => "error", "message" => "未找到对应的物品"]);
        mysqli_close($con);
        exit;
    }
} else {
    // 未提供领回单ID
    echo json_encode(["status" => "error", "message" => "无效的领回单ID"]);
    mysqli_close($con);
    exit;
}
?>
