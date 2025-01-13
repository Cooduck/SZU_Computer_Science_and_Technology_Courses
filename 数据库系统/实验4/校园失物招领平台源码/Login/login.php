<?php 
include "../connect.php"; 
session_start();
header("content-type:text/html;charset=utf-8");

// 接收 $_POST 中的用户名和密码
$account = $_POST['account'];
$password = $_POST['password'];

// 查看表 user 账号与密码是否匹配
$sql = "SELECT user_id FROM user WHERE user_account = '$account' AND user_password = '$password'";  // 防止 SQL 注入，使用引号

// 执行查询
$result = mysqli_query($con, $sql);

// 判断是否存在匹配的用户
if (mysqli_num_rows($result) > 0) {
    // 获取用户数据
    $row = mysqli_fetch_assoc($result);

    // 将用户账号和 ID 存储到会话中
    $_SESSION['user_account'] = $account;
    $_SESSION['user_id'] = (int)$row['user_id']; // 设置 user_id

    header("Location: ../FrontPage/index.php");
} else {
    $_SESSION['error_message'] = "登录失败，请检查账号或密码是否正确";
    header("Location: tip.php"); // 跳转到错误页面
}

// 关闭数据库连接
mysqli_close($con);
?>
