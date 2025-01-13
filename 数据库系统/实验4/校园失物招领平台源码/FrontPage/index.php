<?php
session_start();
include "../connect.php";
// 验证登录状态
if (!isset($_SESSION['user_account']) || !isset($_SESSION['user_id'])) {
    header("Location: ../Login/index.php");
    mysqli_close($con);
    exit;
}

// 检查是否为管理员
$_SESSION['is_admin'] = false;
$user_id = $_SESSION['user_id'];
$admin_check_sql = "SELECT * FROM administrator WHERE user_id = ?";
$stmt = $con->prepare($admin_check_sql);
$stmt->bind_param("i", $user_id);
$stmt->execute();
$admin_result = $stmt->get_result();

if ($admin_result && $admin_result->num_rows > 0) {
    $_SESSION['is_admin'] = true;
}

$lost_sql = "
SELECT 
    i.item_name,
    i.item_category_name,
    i.item_description,
    pli.lost_location,
    pli.lost_date_time AS event_time,
    ip.photo_url
FROM 
    post_lost_item pli
JOIN 
    item i ON pli.item_id = i.item_id
LEFT JOIN 
    itemPhoto ip ON i.item_id = ip.item_id
ORDER BY 
    pli.lost_date_time DESC
LIMIT 3";

$pickup_sql = "
SELECT 
    i.item_name,
    i.item_category_name,
    i.item_description,
    ppi.pickup_location,
    ppi.pickup_date_time AS event_time,
    ip.photo_url
FROM 
    post_pickup_item ppi
JOIN 
    item i ON ppi.item_id = i.item_id
LEFT JOIN 
    itemPhoto ip ON i.item_id = ip.item_id
ORDER BY 
    ppi.pickup_date_time DESC
LIMIT 3";

// 执行查询
$lost_result = $con->query($lost_sql);
$pickup_result = $con->query($pickup_sql);

$lost_items = [];
$pickup_items = [];

if ($lost_result) {
    while ($row = $lost_result->fetch_assoc()) {
        $lost_items[] = $row;
    }
}

if ($pickup_result) {
    while ($row = $pickup_result->fetch_assoc()) {
        $pickup_items[] = $row;
    }
}

mysqli_close($con);
?>

<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>校园失物招领平台</title>
    <link rel="stylesheet" href="style.css">
</head>
<body>
    <!-- 顶部导航栏 -->
    <header>
        <div class="logo">校园失物招领平台</div>
        <nav class="nav-links">
            <a href="index.php">首页</a>
            <a href="../Hall/xunwu.php">寻物大厅</a>
            <a href="../Hall/zhaolin.php">招领大厅</a>
            <a href="../Publish/publish.php">发布</a>
        </nav>
        <div class="user-menu">
            <span class="username">
                <?php
                // 显示用户名
                echo "欢迎回来，" . htmlspecialchars($_SESSION['user_account']);
                ?>
            </span>
            <div class="dropdown">
                <button class="dropdown-btn">▼</button>
                <div class="dropdown-content">
                    <a href="#">最新消息</a>
                    <a href="../Publishing/publishing.php">正在发布</a>
                    <a href="../UserProfile/profile.php">个人资料</a>
                    <?php if ($_SESSION['is_admin']): ?>
                        <a href="../Receipt/receipt.php">领回单</a>
                        <a href="../Log/logs.php">日志</a>
                        <a href="../Blacklist/blacklist.php">黑名单</a>
                    <?php endif; ?>
                    <a href="../Login/index.php">退出</a>
                </div>
            </div>
        </div>
    </header>

    <!-- 内容区 -->
    <main style="display: flex; justify-content: center; gap: 5%; margin: 0 auto; width: 90%;">
        <!-- 最新寻物 -->
        <div class="latest-section" style="width: 30%; text-align: center;">
            <h2 style="font-family: 'Arial', sans-serif; color: #2c3e50; margin-bottom: 20px;">最新寻物</h2>
            <?php foreach ($lost_items as $item): ?>
                <div class="item" style="margin-bottom: 20px;">
                    <img src="<?php echo htmlspecialchars($item['photo_url']); ?>" 
                        alt="<?php echo htmlspecialchars($item['item_name']); ?>" 
                        style="width: 200px; height: auto; border-radius: 10px; max-height: 300px; object-fit: cover;">
                    <div class="item-details" style="margin-top: 10px; text-align: left;">
                        <p style="font-weight: bold; font-size: 1.2rem; color: #34495e;">
                            <strong>[寻物]</strong> <?php echo htmlspecialchars($item['item_name']); ?>
                        </p>
                        <p style="color: #7f8c8d; margin-bottom: 5px;">物品类别：<?php echo htmlspecialchars($item['item_category_name']); ?></p>
                        <p style="color: #7f8c8d; margin-bottom: 5px;">详细描述：<?php echo htmlspecialchars($item['item_description']); ?></p>
                        <p style="color: #7f8c8d; margin-bottom: 5px;">丢失地点：<?php echo htmlspecialchars($item['lost_location']); ?></p>
                        <p style="color: #7f8c8d; margin-bottom: 5px;">丢失发布时间：<?php echo htmlspecialchars($item['event_time']); ?></p>
                    </div>
                </div>
            <?php endforeach; ?>
        </div>

        <!-- 最新招领 -->
        <div class="latest-section" style="width: 30%; text-align: center;">
            <h2 style="font-family: 'Arial', sans-serif; color: #2c3e50; margin-bottom: 20px;">最新招领</h2>
            <?php foreach ($pickup_items as $item): ?>
                <div class="item" style="margin-bottom: 20px;">
                    <img src="<?php echo htmlspecialchars($item['photo_url']); ?>" 
                        alt="<?php echo htmlspecialchars($item['item_name']); ?>" 
                        style="width: 200px; height: auto; border-radius: 10px; max-height: 300px; object-fit: cover;">
                    <div class="item-details" style="margin-top: 10px; text-align: left;">
                        <p style="font-weight: bold; font-size: 1.2rem; color: #34495e;">
                            <strong>[招领]</strong> <?php echo htmlspecialchars($item['item_name']); ?>
                        </p>
                        <p style="color: #7f8c8d; margin-bottom: 5px;">物品类别：<?php echo htmlspecialchars($item['item_category_name']); ?></p>
                        <p style="color: #7f8c8d; margin-bottom: 5px;">详细描述：<?php echo htmlspecialchars($item['item_description']); ?></p>
                        <p style="color: #7f8c8d; margin-bottom: 5px;">拾取地点：<?php echo htmlspecialchars($item['pickup_location']); ?></p>
                        <p style="color: #7f8c8d; margin-bottom: 5px;">拾取发布时间：<?php echo htmlspecialchars($item['event_time']); ?></p>
                    </div>
                </div>
            <?php endforeach; ?>
        </div>
    </main>


</body>
</html>
