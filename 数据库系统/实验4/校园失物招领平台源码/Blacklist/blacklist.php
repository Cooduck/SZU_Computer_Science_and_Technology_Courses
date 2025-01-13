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
    dp.blacklist_id,
    u.user_id,
    u.user_name,
    u.user_account,
    u.user_contact_info,
    dp.added_time,
    dp.blacked_reason,
    dp.admin_id
FROM 
    dishonestPerson dp
JOIN 
    user u
ON 
    dp.user_id = u.user_id
ORDER BY added_time DESC
LIMIT ?, ?;
";

$stmt = $con->prepare($query);
$stmt->bind_param('ii', $offset, $posts_per_page);
$stmt->execute();
$posts_result = $stmt->get_result();
$posts = $posts_result->fetch_all(MYSQLI_ASSOC);

// 计算总页数
$count_query = "
SELECT (SELECT COUNT(*) FROM dishonestPerson) AS total_rows;
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
    <title>失信用户名单</title>
    <link rel="stylesheet" href="style.css">
</head>
<body>
    <header>
        <div class="logo">校园失物招领平台</div>
    </header>

    <!-- 返回按钮 -->
    <div class="back-button">
        <a href="../FrontPage/index.php" class="back-link">返回</a>
        <button id="add-dishonest-user-btn" class="add-dishonest-user-btn">添加失信用户</button>
    </div>

    <main>
        <h1>失信用户名单</h1>
        <div class="post-list">
            <?php if (count($posts) > 0): ?>
                <?php foreach ($posts as $post): ?>
                <div class="post-item">
                    <div class="post-details">
                        <p><strong>失信名单ID：</strong><?php echo htmlspecialchars($post['blacklist_id']); ?></p>
                        <p><strong>失信用户ID：</strong><?php echo htmlspecialchars($post['user_id']); ?></p>
                        <p><strong>失信用户名称：</strong><?php echo htmlspecialchars($post['user_name']); ?></p>
                        <p><strong>失信用户账号：</strong><?php echo htmlspecialchars($post['user_account']); ?></p>
                        <p><strong>失信用户联系方式：</strong><?php echo htmlspecialchars($post['user_contact_info']); ?></p>
                        <p><strong>被加入名单的原由：</strong><?php echo htmlspecialchars($post['blacked_reason']); ?></p>
                        <p><strong>被加入名单的时间：</strong><?php echo htmlspecialchars($post['added_time']); ?></p>
                        <p><strong>负责添加的管理员ID：</strong><?php echo htmlspecialchars($post['admin_id']); ?></p>
                    </div>
                </div>
                <?php endforeach; ?>
            <?php else: ?>
                <p class="no-data-message">暂无发布信息。</p>
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

    <div id="add-dishonest-user-modal" class="hidden">
        <div class="modal-content">
            <h2>添加失信用户</h2>
            <form id="add-dishonest-user-form">
                <label for="user-select">选择用户：</label>
                <select id="user-select" name="user_id" required>
                </select>
                <label for="blacked-reason">失信原因：</label>
                <textarea id="blacked-reason" name="blacked_reason" required></textarea>
                <div class="modal-buttons">
                    <button type="submit">确定</button>
                    <button type="button" id="cancel-modal">取消</button>
                </div>
            </form>
        </div>
    </div>

    <script>
    document.addEventListener("DOMContentLoaded", () => {
        const modal = document.getElementById("add-dishonest-user-modal");
        const addButton = document.getElementById("add-dishonest-user-btn");
        const cancelButton = document.getElementById("cancel-modal");
        const userSelect = document.getElementById("user-select");
        const form = document.getElementById("add-dishonest-user-form");

        // 打开弹窗
        addButton.addEventListener("click", () => {
            modal.classList.remove("hidden");
            modal.classList.add("modal");
            fetch("fetch_users.php") // 获取未在黑名单中的用户
                .then((response) => response.json())
                .then((data) => {
                    userSelect.innerHTML = ""; // 清空已有内容
                    data.forEach((user) => {
                        const option = document.createElement("option");
                        option.value = user.user_id;
                        option.textContent = `ID:${user.user_id}  姓名:${user.user_name}  账号:${user.user_account}`;
                        userSelect.appendChild(option);
                    });
                });
        });

        // 关闭弹窗
        cancelButton.addEventListener("click", () => {
            modal.classList.add("hidden");
            modal.classList.remove("modal");
        });

        // 提交表单
        form.addEventListener("submit", (event) => {
            event.preventDefault();
            const formData = new FormData(form);

            fetch("add_dishonest_user.php", {
                method: "POST",
                body: formData,
            })
                .then((response) => response.json())
                .then((result) => {
                    if (result.status === "success") {
                        alert("用户已成功添加至失信名单！");
                        modal.classList.add("hidden");
                        location.reload(); // 刷新页面以更新列表
                    } else {
                        alert(result.message || "添加失败，请稍后重试！");
                    }
                })
                .catch((error) => console.error("Error:", error));
        });
    });
    </script>
</body>
</html>
