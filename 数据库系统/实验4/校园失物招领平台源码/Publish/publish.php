<?php
session_start();
// 验证登录状态
if (!isset($_SESSION['user_account']) || !isset($_SESSION['user_id'])) {
    header("Location: ../Login/index.php");
    exit;
}
?>

<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>发布启事 - 校园失物招领平台</title>
    <link rel="stylesheet" href="style.css">
</head>
<body>
    <div id="popup" class="popup"></div>

    <!-- 顶部导航栏 -->
    <header>
        <div class="logo">校园失物招领平台</div>
        <nav class="nav-links">
            <a href="../FrontPage/index.php">首页</a>
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

    <h2>发布启事</h2>
    <form action="upload_handler.php" method="POST" enctype="multipart/form-data">
        <!-- 基本信息 -->
        <label for="announcement_type">启事类型</label>
        <select name="announcement_type" id="announcement_type" onchange="toggleFields()" required>
            <option value="lost">寻物启事</option>
            <option value="found">失物招领</option>
        </select>

        <label for="item_name">物品名称</label>
        <input type="text" id="item_name" name="item_name" placeholder="请输入物品名称" required>

        <label for="item_category_name">物品类别</label>
        <select name="item_category_name" id="item_category_name" required>
            <option value="个人证件">个人证件：身份证、护照、驾驶证、学生证等。</option>
            <option value="电子产品">电子产品：手机、笔记本电脑、平板电脑、相机等。</option>
            <option value="钱包和卡类">钱包和卡类：钱包、银行卡、信用卡、交通卡等。</option>
            <option value="钥匙">钥匙：房门钥匙、车钥匙、自行车钥匙等。</option>
            <option value="首饰和手表">首饰和手表：项链、戒指、耳环、手镯、手表等。</option>
            <option value="衣物和配饰">衣物和配饰：外套、帽子、围巾、手套、鞋子等。</option>
            <option value="书籍和文具">书籍和文具：书籍、笔记本、笔、文件夹等。</option>
            <option value="运动器材">运动器材：健身卡、瑜伽垫、球类等。</option>
            <option value="食品和饮料">食品和饮料：未开封的食品、饮料等。</option>
            <option value="其他物品">其他物品：雨伞、背包、行李箱等。</option>
        </select>

        <label for="item_description">物品描述</label>
        <textarea id="item_description" name="item_description" rows="4" placeholder="详细描述物品的特征和信息"></textarea>

        <!-- 丢失字段 -->
        <div id="lostFields">
            <label for="lost_location">丢失地点</label>
            <input type="text" id="lost_location" name="lost_location" placeholder="请输入物品丢失地点" required>
        </div>

        <!-- 拾取字段 -->
        <div id="foundFields" class="hidden">
            <label for="pickup_location">拾取地点</label>
            <input type="text" id="pickup_location" name="pickup_location" placeholder="请输入物品拾取地点" required>
        </div>

        <label for="item_image">物品图片</label>
        <input type="file" id="item_image" name="item_image" accept="image/*" required>

        <label>是否公布联系方式</label>
        <select name="display_type" required>
            <option value="yes">是</option>
            <option value="no">否</option>
        </select>

        <div class="button-container">
            <button class="dropdown-btn" type="submit">发布</button>
            <button class="dropdown-btn" type="reset">重置</button>
        </div>
    </form>

    <script>
        function toggleFields() {
            const typeSelect = document.getElementById("announcement_type").value;
            const isLost = typeSelect === "lost";

            document.getElementById("lostFields").classList.toggle("hidden", !isLost);
            document.getElementById("foundFields").classList.toggle("hidden", isLost);

            document.getElementById("lost_location").required = isLost;
            document.getElementById("pickup_location").required = !isLost;
        }

        window.onload = function () {
            toggleFields(); // 页面加载时初始化
        };

        const urlParams = new URLSearchParams(window.location.search);
        const result = urlParams.get('result');
        const message = urlParams.get('message');  // 获取 message 参数
        const popup = document.getElementById('popup');

        if (result === 'success') {
            popup.style.backgroundColor = "#4CAF50";
            popup.textContent = message || '成功！';
            popup.style.display = 'block';
        } else if (result === 'failure') {
            popup.style.backgroundColor = "#f44336";
            popup.textContent = message || '失败！';
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
</body>
</html>
