<?php
session_start();
include "../connect.php";

// 验证登录状态
if (!isset($_SESSION['user_account']) || !isset($_SESSION['user_id'])) {
    header("Location: ../Login/index.php");
    exit;
}

// 获取当前用户的账号
$user_account = $_SESSION['user_account'];

// 查询数据库获取用户信息
$sql = "SELECT * FROM user WHERE user_account = '$user_account'";
$result = mysqli_query($con, $sql);

// 如果查询结果为空，则跳转到错误页面
if (mysqli_num_rows($result) == 0) {
    die("无法找到用户信息！");
}

// 获取用户数据
$user = mysqli_fetch_assoc($result);

// 关闭数据库连接
mysqli_close($con);
?>

<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>个人资料</title>
    <link rel="stylesheet" href="style.css">
</head>
<body>
    <div id="popup" class="popup"></div>

    <header>
        <div class="logo">校园失物招领平台</div>
    </header>

    <main>
        <h1>个人资料</h1>
        <form action="update_profile.php" method="post">
            <label for="user_id">用户ID：</label>
            <p id="user_id"><?php echo htmlspecialchars($user['user_id']); ?></p>
            <input type="hidden" name="user_id" value="<?php echo htmlspecialchars($user['user_id']); ?>">

            <label for="user_account">账号：</label>
            <p id="user_account"><?php echo htmlspecialchars($user['user_account']); ?></p>

            <label for="user_password">密码：</label>
            <input type="text" id="user_password" name="user_password" value="<?php echo htmlspecialchars($user['user_password']); ?>"><br>

            <label for="user_name">姓名：</label>
            <input type="text" id="user_name" name="user_name" value="<?php echo htmlspecialchars($user['user_name']); ?>"><br>

            <label for="user_contact_info">联系方式：</label>
            <input type="text" id="user_contact_info" name="user_contact_info" value="<?php echo htmlspecialchars($user['user_contact_info']); ?>"><br>

            <label for="user_id_card">证件号：</label>
            <input type="text" id="user_id_card" name="user_id_card" value="<?php echo htmlspecialchars($user['user_id_card']); ?>"><br>

            <div class="btn-container">
                <button type="submit">保存修改</button>
                <!-- 返回主页面按钮 -->
                <a href="../FrontPage/index.php" class="button">返回主页面</a>
            </div>
        </form>

        <script>
            const urlParams = new URLSearchParams(window.location.search);
            const result = urlParams.get('result');
            const message = urlParams.get('message');  // 获取 message 参数
            const popup = document.getElementById('popup');

            if (result === 'success') {
                popup.textContent = message || '个人信息修改成功！';
                popup.style.display = 'block';
            } else if (result === 'failure') {
                popup.textContent = message || '修改失败，请重试！';
                popup.classList.add('failure');
                popup.style.display = 'block';
            }

            // 一秒后隐藏弹窗
            if (popup.style.display === 'block') {
                setTimeout(() => {
                    popup.style.display = 'none';
                }, 2000);
            }
        </script>
    </main>
</body>
</html>
