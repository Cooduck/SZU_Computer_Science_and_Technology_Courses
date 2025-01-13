<?php
session_start();
?>

<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>提示界面</title>
    <link rel="stylesheet" href="https://www.layuicdn.com/layui/css/layui.css" />
    <script src="https://www.layuicdn.com/layui/layui.js" charset="utf-8"></script>
    <style>
        /* 页面内容居中 */
        body, html {
            height: 100%;
            margin: 0;
            display: flex;
            justify-content: center;
            align-items: center;
            font-family: Arial, sans-serif;
        }

        /* 容器样式 */
        .container {
            width: 500px;
            padding: 20px;
            background-color: #F2F2F2;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
            border-radius: 8px;
            position: relative; /* 为了定位返回按钮 */
        }

        .layui-card {
            margin-top: 20px;
        }

        .layui-card-body {
            padding: 20px;
            text-align: center;
        }

        /* 错误信息的样式 */
        .error-message {
            color: red;
            font-size: 18px;
            margin-top: 20px;
        }

        .correct-message {
            color: black;
            font-size: 18px;
            margin-top: 20px;
        }

        /* 返回按钮样式 */
        .back-btn {
            background-color: #1E9FFF; /* layui的蓝色按钮样式 */
            color: white;
            border: none;
            padding: 10px 20px;
            border-radius: 4px;
            cursor: pointer;
            margin-top: 20px;
        }

        .back-btn:hover {
            background-color: #009688; /* 鼠标悬停时的颜色 */
        }
    </style>
</head>
<body>

    <div class="container">
        <div class="layui-card">
            <div class="layui-card-body">
                <!-- 显示错误信息 -->
                <p class="error-message">
                    <?php 
                    if (isset($_SESSION['error_message'])) {
                        echo $_SESSION['error_message']; // 显示错误信息
                        unset($_SESSION['error_message']); // 清除错误信息，防止刷新时再次显示
                    }
                    ?>
                </p>
                
                <!-- 显示正确信息 -->
                <p class="correct-message">
                    <?php 
                    if (isset($_SESSION['correct_message'])) {
                        echo $_SESSION['correct_message']; // 显示正确信息
                        unset($_SESSION['correct_message']); // 清除正确信息，防止刷新时再次显示
                    }
                    ?>
                </p>
                
                <!-- 返回按钮 -->
                <form action="index.php">
                    <button type="submit" class="back-btn">返回首页</button>
                </form>
            </div>
        </div>
    </div>

</body>
</html>
