<?php
session_start();
include "../connect.php";

// 验证登录状态
if (!isset($_SESSION['user_account']) || !isset($_SESSION['user_id'])) {
    header("Location: ../Login/index.php");
    exit;
}

// 从会话中获取当前登录用户的账号
$user_account = $_SESSION['user_account'];
$user_id = $_SESSION['user_id'];

// 分页参数
$posts_per_page = 4; // 每页显示的发布数量
$page = isset($_GET['page']) ? max(1, intval($_GET['page'])) : 1; // 当前页码，至少为1
$offset = ($page - 1) * $posts_per_page; // 查询起始位置

// 查询用户的发布数据
$query = "
SELECT 
    '寻物' AS post_type,
    lost_item.item_id AS post_id,
    item.item_name,
    lost_item.lost_date_time AS post_time,
    photo.photo_url
FROM post_lost_item lost_item
LEFT JOIN item ON lost_item.item_id = item.item_id
LEFT JOIN itemPhoto photo ON lost_item.item_id = photo.item_id
WHERE lost_item.user_id = ?

UNION ALL

SELECT 
    '招领' AS post_type,
    pickup_item.item_id AS post_id,
    item.item_name,
    pickup_item.pickup_date_time AS post_time,
    photo.photo_url
FROM post_pickup_item pickup_item
LEFT JOIN item ON pickup_item.item_id = item.item_id
LEFT JOIN itemPhoto photo ON pickup_item.item_id = photo.item_id
WHERE pickup_item.user_id = ?

UNION ALL

SELECT 
    '领回单' AS post_type,
    receipt.receiveReceipt_id AS post_id,
    item.item_name,
    receipt.receiveReceipt_time AS post_time,
    photo.photo_url
FROM ReceiveReceipt receipt
LEFT JOIN item ON receipt.item_id = item.item_id
LEFT JOIN itemPhoto photo ON receipt.item_id = photo.item_id
WHERE receipt.user_id = ?

ORDER BY post_time DESC
LIMIT ?, ?;
";

$stmt = $con->prepare($query);
$stmt->bind_param('iiiii', $user_id, $user_id, $user_id, $offset, $posts_per_page);
$stmt->execute();
$posts_result = $stmt->get_result();
$posts = $posts_result->fetch_all(MYSQLI_ASSOC);

// 计算总页数
$count_query = "
SELECT 
    (SELECT COUNT(*) FROM post_lost_item WHERE user_id = $user_id) + 
    (SELECT COUNT(*) FROM post_pickup_item WHERE user_id = $user_id) + 
    (SELECT COUNT(*) FROM ReceiveReceipt WHERE user_id = $user_id) AS total_rows;
";

$stmt_count = $con->query($count_query);
$total_posts = (int)$stmt_count->fetch_assoc()['total_rows'];
$total_pages = ceil($total_posts / $posts_per_page);
mysqli_close($con);
?>


<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>我的全部发布</title>
    <link rel="stylesheet" href="style.css">
</head>
<body>
    <header>
        <div class="logo">校园失物招领平台</div>
    </header>

    <!-- 返回按钮 -->
    <div class="back-button">
        <a href="../FrontPage/index.php" class="back-link">返回</a>
    </div>

    <main>
        <h1>我的全部发布</h1>
        <div class="post-list">
            <?php if (count($posts) > 0): ?>
                <?php foreach ($posts as $post): ?>
                <div class="post-item">
                    <img src="<?php echo htmlspecialchars($post['photo_url']); ?>" 
                         alt="<?php echo htmlspecialchars($post['item_name']); ?>" 
                         class="post-image">
                    <div class="post-details">
                        <p><strong>类型：</strong><?php echo htmlspecialchars($post['post_type']); ?></p>
                        <p><strong>ID：</strong><?php echo htmlspecialchars($post['post_id']); ?></p>
                        <p><strong>物品名称：</strong><?php echo htmlspecialchars($post['item_name']); ?></p>
                        <p><strong>发布时间：</strong><?php echo htmlspecialchars($post['post_time']); ?></p>
                    </div>
                </div>
                <?php endforeach; ?>
            <?php else: ?>
                <p>暂无发布信息。</p>
            <?php endif; ?>
        </div>

        <!-- 分页导航 -->
        <div class="pagination">
            <?php if ($page > 1): ?>
            <a href="?page=<?php echo $page - 1; ?>" class="prev-page">上一页</a>
            <?php endif; ?>

            <?php for ($i = 1; $i <= $total_pages; $i++): ?>
            <a href="?page=<?php echo $i; ?>" class="<?php echo $i === $page ? 'current-page' : ''; ?>">
                <?php echo $i; ?>
            </a>
            <?php endfor; ?>

            <?php if ($page < $total_pages): ?>
            <a href="?page=<?php echo $page + 1; ?>" class="next-page">下一页</a>
            <?php endif; ?>
        </div>
    </main>
</body>
</html>
