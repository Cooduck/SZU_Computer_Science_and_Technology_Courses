<?php
include "../connect.php"; 
session_start();
header("content-type:text/html;charset=utf-8");

// 接收用户名和密码
$account = $_POST['account'];
$password = $_POST['password'];

// 检查账号或密码是否为空
if ($account == "" || $password == "") {
    $_SESSION['error_message'] = "账号或密码不能为空";
    header("Location: tip.php"); // 跳转到错误页面
    exit;
} else {
    // 查询数据库中已存在的账号
    $sql = "SELECT * FROM user WHERE user_account = '$account'";
    $result = mysqli_query($con, $sql);
    $num = mysqli_num_rows($result); // 已存在账号的数量

    if ($num) {
        $_SESSION['error_message'] = "该账号已存在，请更换一个账号";
        header("Location: tip.php"); // 跳转到错误页面
        exit;
    } else {
        // 插入新用户
        $sql_insert = "INSERT INTO user(user_account, user_password, user_status) VALUES('$account', '$password', '正常')";
        $result_insert = mysqli_query($con, $sql_insert);

        // 判断是否注册成功
        if (!$result_insert) {
            $_SESSION['error_message'] = "注册不成功，请重新注册";
            header("Location: tip.php"); // 跳转到错误页面
            exit;
        } else {
            $_SESSION['correct_message'] = "注册成功，请返回登录界面进行登录";
            header("Location: tip.php"); // 跳转到错误页面
            exit;
        }
    }
}

mysqli_close($con);  // 关闭数据库连接
?>
