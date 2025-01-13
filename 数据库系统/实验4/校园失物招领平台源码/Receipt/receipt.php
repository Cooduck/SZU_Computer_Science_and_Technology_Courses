<?php
session_start();
include "../connect.php";

// 验证登录状态
if (!isset($_SESSION['user_account']) || !isset($_SESSION['user_id']) || !isset($_SESSION['is_admin']) || $_SESSION['is_admin'] != true) {
    header("Location: ../Login/index.php");
    mysqli_close($con);
    exit;
}

// 从会话中获取当前登录用户的账号
$user_account = $_SESSION['user_account'];
$user_id = $_SESSION['user_id'];

// 分页参数
$posts_per_page = 4; // 每页显示的发布数量
$page = isset($_GET['page']) ? max(1, intval($_GET['page'])) : 1; // 当前页码，至少为1
$offset = ($page - 1) * $posts_per_page; // 查询起始位置

// 查询
$query = "
SELECT 
    r.receiveReceipt_id AS receiveReceipt_id,
    r.receiveReceipt_status AS receiveReceipt_status,
    r.receiveReceipt_time AS receiveReceipt_time,
    r.user_id AS user_id,
    i.item_name AS item_name,
    i.item_category_name AS item_category_name,
    i.item_description AS item_description,
    p.pickup_location AS pickup_location,
    p.pickup_date_time AS pickup_date_time,
    ip.photo_url AS photo_url
FROM 
    ReceiveReceipt r
JOIN 
    item i ON r.item_id = i.item_id
JOIN 
    post_pickup_item p ON r.item_id = p.item_id
JOIN 
    itemPhoto ip ON i.item_id = ip.item_id
ORDER BY receiveReceipt_time DESC
LIMIT ?, ?;
";

$stmt = $con->prepare($query);
$stmt->bind_param('ii', $offset, $posts_per_page);
$stmt->execute();
$posts_result = $stmt->get_result();
$posts = $posts_result->fetch_all(MYSQLI_ASSOC);

// 计算总页数
$count_query = "
SELECT 
    (SELECT COUNT(*) FROM ReceiveReceipt ) AS total_rows;
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
    <title>全部领回单</title>
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
        <h1>全部领回单</h1>
        <div class="post-list">
            <?php if (count($posts) > 0): ?>
                <?php foreach ($posts as $post): ?>
                <div class="post-item">
                    <img src="<?php echo htmlspecialchars($post['photo_url']); ?>" 
                         alt="<?php echo htmlspecialchars($post['item_name']); ?>" 
                         class="post-image">
                    <div class="post-details">
                        <p><strong>领回单ID：</strong><?php echo htmlspecialchars($post['receiveReceipt_id']); ?></p>
                        <p><strong>领回单状态：</strong><?php echo htmlspecialchars($post['receiveReceipt_status']); ?></p>
                        <p><strong>领回单发布时间：</strong><?php echo htmlspecialchars($post['receiveReceipt_time']); ?></p>
                        <p><strong>领回物品名：</strong><?php echo htmlspecialchars($post['item_name']); ?></p>
                        <p><strong>领回物品类型：</strong><?php echo htmlspecialchars($post['item_category_name']); ?></p>
                        <p><strong>领回物品详细描述：</strong><?php echo htmlspecialchars($post['item_description']); ?></p>
                        <p><strong>领回物品拾取地点：</strong><?php echo htmlspecialchars($post['pickup_location']); ?></p>
                        <p><strong>领回物品拾取发布时间：</strong><?php echo htmlspecialchars($post['pickup_date_time']); ?></p>
                        <a href="../UserDetail/user_details.php?user_id=<?php echo htmlspecialchars($post['user_id']); ?>">
                            <strong>领回人ID：</strong><?php echo htmlspecialchars($post['user_id']); ?>
                        </a>

                    <!-- 审核通过按钮，仅当状态为 "未处理" 时显示 -->
                    <?php if ($post['receiveReceipt_status'] === '未处理'): ?>
                        <button id="approve-button" class="approve-button" data-id="<?php echo htmlspecialchars($post['receiveReceipt_id']); ?>">审核通过</button>
                    <?php endif; ?>

                    <?php if ($post['receiveReceipt_status'] === '未处理'): ?>
                        <button id="disapprove-button" class="disapprove-button" data-id="<?php echo htmlspecialchars($post['receiveReceipt_id']); ?>">审核不通过，认为该用户冒领</button>
                    <?php endif; ?>

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

        <script>
            const approveButtons = document.querySelectorAll('.approve-button');
            // 设置点击事件
            approveButtons.forEach(button => {
                button.addEventListener('click', function () {
                    // 获取按钮的 data-id 属性
                    const receiveReceiptId = button.getAttribute('data-id');

                    // 构造请求数据
                    const requestData = {
                        receiveReceipt_id: receiveReceiptId
                    };

                    // 发送 AJAX 请求
                    fetch('approve_receipt.php', {
                        method: 'POST',
                        headers: {
                            'Content-Type': 'application/json'
                        },
                        body: JSON.stringify(requestData)
                    })
                    .then(response => response.json())
                    .then(data => {
                        if (data.status === 'success') {
                            alert(data.message);
                            location.reload();
                        } else {
                            alert(data.message);
                        }
                    })
                    .catch(error => {
                        console.error('Error:', error);
                    });
                });
            });

            const disapproveButtons = document.querySelectorAll('.disapprove-button');
            // 设置点击事件
            disapproveButtons.forEach(button => {
                button.addEventListener('click', function () {
                    // 获取按钮的 data-id 属性
                    const receiveReceiptId = button.getAttribute('data-id');

                    // 构造请求数据
                    const requestData = {
                        receiveReceipt_id: receiveReceiptId
                    };

                    // 发送 AJAX 请求
                    fetch('disapprove_receipt.php', {
                        method: 'POST',
                        headers: {
                            'Content-Type': 'application/json'
                        },
                        body: JSON.stringify(requestData)
                    })
                    .then(response => {
                        console.log('Response:', response);
                        return response.text();
                    })
                    .then(text => {
                        console.log('Raw Response Text:', text);
                        return JSON.parse(text);
                    })
                    .then(data => {
                        if (data.status === 'success') {
                            alert(data.message);
                            location.reload();
                        } else {
                            alert(data.message);
                        }
                    })
                    .catch(error => {
                        console.error('Error:', error);
                    });
                });
            });

        </script>
    </main>
</body>
</html>
