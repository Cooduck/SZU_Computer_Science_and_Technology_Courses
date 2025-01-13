<?php
session_start();
include "../connect.php";

// 验证登录状态
if (!isset($_SESSION['user_account']) || !isset($_SESSION['user_id'])) {
    header("Location: ../Login/index.php");
    exit;
}

// 接收表单数据
$user_id = $_POST['user_id'];
$user_password = $_POST['user_password'];
$user_id_card = $_POST['user_id_card'];
$user_name = $_POST['user_name'];
$user_contact_info = $_POST['user_contact_info'];

// 更新用户信息
$sql = "UPDATE user SET 
        user_password = '$user_password',
        user_id_card = '$user_id_card',
        user_name = '$user_name',
        user_contact_info = '$user_contact_info'
        WHERE user_id = '$user_id'";

if (mysqli_query($con, $sql)) {
    // 将修改结果通过 URL 参数返回
    header("Location: profile.php?result=success&message=个人信息修改成功！");
} else {
    header("Location: profile.php?result=failure&message=修改失败，请重试！");
}
// 关闭数据库连接
mysqli_close($con);
?>
