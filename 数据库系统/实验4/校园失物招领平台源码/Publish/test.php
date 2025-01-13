<?php
session_start();
include "../connect.php";

// 验证登录状态
if (!isset($_SESSION['user_account']) || !isset($_SESSION['user_id'])) {
    header("Location: ../Login/index.php");
    exit;
}

if ($_SERVER["REQUEST_METHOD"] === "POST") {
    echo "<h1>测试信息</h1>";

    // 输出基本信息
    echo "<h2>基本信息</h2>";
    echo "启事类型: " . htmlspecialchars($_POST['announcement_type']) . "<br>";
    echo "物品名称: " . htmlspecialchars($_POST['item_name']) . "<br>";
    echo "物品类别: " . htmlspecialchars($_POST['item_category_name']) . "<br>";
    echo "物品描述: " . nl2br(htmlspecialchars($_POST['item_description'])) . "<br>";

    // 根据启事类型输出相关字段
    if ($_POST['announcement_type'] === 'lost') {
        echo "<h2>丢失信息</h2>";
        echo "丢失地点: " . htmlspecialchars($_POST['lost_location']) . "<br>";
        echo "丢失发布时间: " . htmlspecialchars($_POST['lost_date_time']) . "<br>";
    } elseif ($_POST['announcement_type'] === 'found') {
        echo "<h2>拾取信息</h2>";
        echo "拾取地点: " . htmlspecialchars($_POST['pickup_location']) . "<br>";
        echo "拾取发布时间: " . htmlspecialchars($_POST['pickup_date_time']) . "<br>";
    }

    // 输出其他信息
    echo "<h2>其他信息</h2>";
    echo "是否公布联系方式: " . htmlspecialchars($_POST['display_type']) . "<br>";

    // 处理上传的图片
    if (isset($_FILES['item_image']) && $_FILES['item_image']['error'] === UPLOAD_ERR_OK) {
        echo "<h2>图片信息</h2>";
        echo "图片名称: " . htmlspecialchars($_FILES['item_image']['name']) . "<br>";
        echo "图片类型: " . htmlspecialchars($_FILES['item_image']['type']) . "<br>";
        echo "临时文件位置: " . htmlspecialchars($_FILES['item_image']['tmp_name']) . "<br>";
        echo "文件大小: " . htmlspecialchars($_FILES['item_image']['size']) . " bytes<br>";
    } else {
        echo "<h2>图片上传错误</h2>";
        echo "错误代码: " . htmlspecialchars($_FILES['item_image']['error']) . "<br>";
    }
} else {
    echo "<h1>无效的请求方法</h1>";
}
?>
