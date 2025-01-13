<?php
session_start();
include "../connect.php";

// 验证是否登录
if (!isset($_SESSION['user_account']) || !isset($_SESSION['user_id'])) {
    header("Location: ../Login/index.php");
    exit;
}

// 获取传递的 user_id
if (!isset($_GET['user_id']) || !is_numeric($_GET['user_id'])) {
    die("用户ID无效！");
}
$user_id = intval($_GET['user_id']);

// 查询用户详细信息
$user_query = "
    SELECT user_id, user_account, user_id_card, user_name, user_contact_info, user_status
    FROM user
    WHERE user_id = ?;
";
$stmt_user = $con->prepare($user_query);
$stmt_user->bind_param('i', $user_id);
$stmt_user->execute();
$user_result = $stmt_user->get_result();
$user_details = $user_result->fetch_assoc();

if (!$user_details) {
    die("用户不存在！");
}

// 查询用户发布的丢失信息
$lost_items_query = "
    SELECT i.item_name, i.item_category_name, i.item_description, p.lost_location, p.lost_date_time, ip.photo_url
    FROM post_lost_item p
    JOIN item i ON p.item_id = i.item_id
    JOIN itemPhoto ip ON i.item_id = ip.item_id
    WHERE p.user_id = ?;
";
$stmt_lost_items = $con->prepare($lost_items_query);
$stmt_lost_items->bind_param('i', $user_id);
$stmt_lost_items->execute();
$lost_items_result = $stmt_lost_items->get_result();
$lost_items = $lost_items_result->fetch_all(MYSQLI_ASSOC);

mysqli_close($con);
?>

<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>用户详细信息</title>
    <link rel="stylesheet" href="style.css">
</head>
<body>
    <header>
        <div class="logo">校园失物招领平台</div>
    </header>

    <!-- 返回按钮 -->
    <div class="back-button">
        <a href="javascript:history.back();" class="back-link">返回</a>
    </div>


    <main>
        <h1>用户详细信息</h1>
        <div>
            <p><strong>用户ID：</strong><?php echo htmlspecialchars($user_details['user_id']); ?></p>
            <p><strong>账号：</strong><?php echo htmlspecialchars($user_details['user_account']); ?></p>
            <p><strong>身份证：</strong><?php echo htmlspecialchars($user_details['user_id_card']); ?></p>
            <p><strong>姓名：</strong><?php echo htmlspecialchars($user_details['user_name']); ?></p>
            <p><strong>联系方式：</strong><?php echo htmlspecialchars($user_details['user_contact_info']); ?></p>
            <p><strong>状态：</strong><?php echo htmlspecialchars($user_details['user_status']); ?></p>
        </div>

        <h1>用户发布的丢失物品</h1>
        <div class="lost-list">
            <?php if (count($lost_items) > 0): ?>
                <?php foreach ($lost_items as $item): ?>
                <div class="lost-item">
                    <img src="<?php echo htmlspecialchars($item['photo_url']); ?>" 
                            alt="<?php echo htmlspecialchars($item['item_name']); ?>" 
                            class="lost-image">
                    <div class="lost-details">
                        <p><strong>物品名：</strong><?php echo htmlspecialchars($item['item_name']); ?></p>
                        <p><strong>类别：</strong><?php echo htmlspecialchars($item['item_category_name']); ?></p>
                        <p><strong>描述：</strong><?php echo htmlspecialchars($item['item_description']); ?></p>
                        <p><strong>丢失地点：</strong><?php echo htmlspecialchars($item['lost_location']); ?></p>
                        <p><strong>丢失时间：</strong><?php echo htmlspecialchars($item['lost_date_time']); ?></p>
                    </div>
                </div>
                <?php endforeach; ?>
            <?php else: ?>
                <p>暂无丢失信息。</p>
            <?php endif; ?>
        </div>
    </main>
</body>
</html>
