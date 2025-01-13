<?php
session_start();
include "../connect.php";

// 验证登录状态
if (!isset($_SESSION['user_account']) || !isset($_SESSION['user_id'])) {
    header("Location: ../Login/index.php");
    mysqli_close($con);
    exit;
}
?>

<?php
// 物品列表

// SQL 查询
$sql = "
SELECT 
    i.item_id,
    i.item_name,
    i.item_category_name,
    i.item_description,
    ppi.user_id,
    ppi.pickup_location,
    ppi.pickup_date_time,
    ip.photo_url,
    u.user_contact_info
FROM 
    post_pickup_item ppi
JOIN 
    item i ON ppi.item_id = i.item_id
LEFT JOIN 
    itemPhoto ip ON i.item_id = ip.item_id
JOIN 
    user u ON ppi.user_id = u.user_id
";

// 执行查询
$res = mysqli_query($con, $sql);

// 检查查询是否成功
if (!$res) {
    die("查询失败: " . mysqli_error($con));
}

// 将结果存储为二维数组
$items = [];
while ($row = mysqli_fetch_array($res, MYSQLI_ASSOC)) {
    $items[] = [
        "item_id" => $row["item_id"],
        "item_name" => $row["item_name"],
        "item_category_name" => $row["item_category_name"],
        "item_description" => $row["item_description"],
        "user_id" => $row["user_id"],
        "pickup_location" => $row["pickup_location"],
        "pickup_date_time" => $row["pickup_date_time"],
        "photo_url" => $row["photo_url"],
        "user_contact_info" => $row["user_contact_info"]
    ];
}

// 关闭连接
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
            <a href="../FrontPage/index.php">首页</a>
            <a href="xunwu.php">寻物大厅</a>
            <a href="zhaolin.php">招领大厅</a>
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

    <!-- 搜索栏 -->
    <section class="search-bar">
        <input type="text" placeholder="搜索">
        <button class="search-btn">搜索</button>
    </section>

    <!-- 物品栏 -->
    <main>
        <div class="items">
            <?php foreach ($items as $item): ?>
            <div class="item">
                <img src="<?php echo $item['photo_url']; ?>" alt="<?php echo $item['item_name']; ?>">
                <div class="item-title">[招领] <?php echo $item['item_name']; ?></div>
                
                <!-- 隐藏的物品描述和联系方式 -->
                <div class="item-description hidden">物品类型：<?php echo $item['item_category_name']; ?></div>
                <div class="item-description hidden">物品描述：<?php echo $item['item_description']; ?></div>
                <div class="item-description hidden">物品拾取地点：<?php echo $item['pickup_location']; ?></div>
                <div class="item-description hidden">拾取发布时间：<?php echo $item['pickup_date_time']; ?></div>
                <div class="item-contact hidden">发布者联系方式：<?php echo $item['user_contact_info']; ?></div>
                
                <button class="view-details-btn" data-item="<?php echo htmlspecialchars(json_encode($item)); ?>">查看详情</button>
            </div>
            <?php endforeach; ?>
        </div>
    </main>

    <!-- 招领物品详细信息的弹窗 -->
    <div id="popup" class="popup">
        <div class="popup-content">
            <span id="close-popup" class="close-popup">&times;</span>
            <!-- 图片容器 -->
            <div class="popup-image-container">
                <img id="popup-image" src="" alt="" class="popup-image">
            </div>
            
            <div id="popup-item-title" class="item-title"></div>
            <div id="popup-item-category" class="item-description"></div>
            <div id="popup-item-description" class="item-description"></div>
            <div id="popup-pickup_location" class="item-description"></div>
            <div id="popup-pickup_date_time" class="item-description"></div>
            <div id="popup-user-contact" class="item-description"></div>

            <!-- 发布领回单按钮 -->
            <button id="publish-receipt-btn" class="publish-receipt-btn">发布领回单</button>
        </div>
    </div>

    <script>
        // 获取弹窗和相关元素
        const popup = document.getElementById('popup');
        const closePopup = document.getElementById('close-popup');
        const viewDetailsButtons = document.querySelectorAll('.view-details-btn');
        
        // 设置查看详情按钮点击事件
        viewDetailsButtons.forEach(button => {
            button.addEventListener('click', function() {
                const item = JSON.parse(this.getAttribute('data-item')); // 获取按钮绑定的物品数据

                // 存储物品数据到 localStorage
                localStorage.setItem('currentItem', JSON.stringify(item));

                // 填充弹窗内容
                document.getElementById('popup-image').src = item.photo_url;
                document.getElementById('popup-item-title').textContent = '[寻物] ' + item.item_name;
                document.getElementById('popup-item-category').textContent = '物品类型：' + item.item_category_name;
                document.getElementById('popup-item-description').textContent = '物品描述：' + item.item_description;
                document.getElementById('popup-pickup_location').textContent = '物品拾取地点：' + item.pickup_location;
                document.getElementById('popup-pickup_date_time').textContent = '拾取发布时间：' + item.pickup_date_time;
                document.getElementById('popup-user-contact').textContent = '发布者联系方式：' + item.user_contact_info;

                // 显示弹窗
                popup.style.display = 'flex';
            });
        });

        // 关闭弹窗
        closePopup.addEventListener('click', function() {
            popup.style.display = 'none';
        });

        // 点击弹窗外部区域时关闭弹窗
        window.addEventListener('click', function(event) {
            if (event.target === popup) {
                popup.style.display = 'none';
            }
        });

        // 获取发布领回单按钮
        const publishReceiptBtn = document.getElementById('publish-receipt-btn');

        // 设置点击事件
        publishReceiptBtn.addEventListener('click', function () {
            // 从 localStorage 获取当前物品信息
            const currentItem = JSON.parse(localStorage.getItem('currentItem'));
            if (!currentItem || !currentItem.item_id || !currentItem.user_id) {
                alert('无法发布领回单，物品信息缺失');
                return;
            }

            // 构造请求数据
            const requestData = {
                item_id: currentItem.item_id,
                user_id: currentItem.user_id
            };

            // 发送 AJAX 请求
            fetch('submit_receipt.php', {
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
                console.log('Parsed Data:', data);
                if (data.status === 'success') {
                    alert(data.message);
                } else {
                    alert('发布失败：' + data.message);
                }
            })
            .catch(error => {
                console.error('请求失败', error);
            });


        });
    </script>

</div>
</body>
</html>
