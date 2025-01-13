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

// 获取日志类型
$log_type = isset($_GET['log_type']) && $_GET['log_type'] === 'admin' ? 'admin' : 'user';

if ($log_type === 'user') {
    // 查询用户日志
    $query = "SELECT log_id, user_id, operation_type, operation_content, operation_time 
              FROM userLog 
              ORDER BY operation_time DESC 
              LIMIT ?, ?";
    $count_query = "SELECT COUNT(*) AS total_rows 
                    FROM userLog";
} else {
    // 查询管理员日志
    $query = "SELECT log_id, admin_id, operation_type, operation_content, operation_time 
              FROM administratorLog 
              ORDER BY operation_time DESC 
              LIMIT ?, ?";
    $count_query = "SELECT COUNT(*) AS total_rows 
                    FROM administratorLog";
}

$stmt = $con->prepare($query);
$stmt->bind_param('ii', $offset, $posts_per_page);
$stmt->execute();
$posts_result = $stmt->get_result();
$posts = $posts_result->fetch_all(MYSQLI_ASSOC);

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
    <title>日志</title>
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

    <div class="back-button">
        <a href="?log_type=user&page=1" class="user-log-link <?php echo $log_type === 'user' ? 'active' : ''; ?>">用户日志</a>
        <a href="?log_type=admin&page=1" class="admin-log-link <?php echo $log_type === 'admin' ? 'active' : ''; ?>">管理员日志</a>
    </div>


    <main>
        <?php if ($log_type === 'user'): ?>
            <h1>用户日志</h1>
        <?php else: ?>
            <h1>管理员日志</h1>
        <?php endif; ?>
        <div class="post-list">
        <?php if (count($posts) > 0): ?>
            <?php foreach ($posts as $post): ?>
                <div class="post-item">
                    <div class="post-details">
                        <?php if ($log_type === 'user'): ?>
                            <p><strong>日志ID：</strong><?php echo htmlspecialchars($post['log_id']); ?></p>
                            <p><strong>用户ID：</strong><?php echo htmlspecialchars($post['user_id']); ?></p>
                        <?php else: ?>
                            <p><strong>日志ID：</strong><?php echo htmlspecialchars($post['log_id']); ?></p>
                            <p><strong>管理员ID：</strong><?php echo htmlspecialchars($post['admin_id']); ?></p>
                        <?php endif; ?>
                        <p><strong>操作类型：</strong><?php echo htmlspecialchars($post['operation_type']); ?></p>
                        <p><strong>操作内容：</strong><?php echo htmlspecialchars($post['operation_content']); ?></p>
                        <p><strong>操作时间：</strong><?php echo htmlspecialchars($post['operation_time']); ?></p>
                    </div>
                </div>
            <?php endforeach; ?>
        <?php else: ?>
            <p>暂无日志记录。</p>
        <?php endif; ?>
        </div>

        <!-- 分页导航 -->
        <div class="pagination">
            <?php if ($page > 1): ?>
                <a href="?log_type=<?php echo $log_type; ?>&page=<?php echo $page - 1; ?>" class="prev-page">上一页</a>
            <?php endif; ?>

            <?php for ($i = 1; $i <= $total_pages; $i++): ?>
                <a href="?log_type=<?php echo $log_type; ?>&page=<?php echo $i; ?>" class="<?php echo $i === $page ? 'current-page' : ''; ?>">
                    <?php echo $i; ?>
                </a>
            <?php endfor; ?>

            <?php if ($page < $total_pages): ?>
                <a href="?log_type=<?php echo $log_type; ?>&page=<?php echo $page + 1; ?>" class="next-page">下一页</a>
            <?php endif; ?>
        </div>

    </main>
</body>
</html>
