<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>登录注册</title>
    <link rel="stylesheet" type="text/css" href="https://www.layuicdn.com/layui/css/layui.css" />
    <script src="https://www.layuicdn.com/layui/layui.js" charset="utf-8"></script>
    <link rel="stylesheet" href="style.css">
</head>
<body>

<div class="container">
    <!-- 页面标题 -->
    <fieldset id="pageTitle" class="layui-elem-field layui-field-title" style="margin-top: 20px;">
        <legend>登录注册</legend>
    </fieldset>   

    <!-- 按钮 -->
    <div id="buttons" class="layui-row layui-col-space15">
        <button id="showLogin" class="layui-btn layui-btn-normal" onclick="toggleForm('login')">登录</button>
        <button id="showRegister" class="layui-btn layui-btn-normal" onclick="toggleForm('register')">注册</button>
    </div>

    <!-- 返回按钮 -->
    <div id="backButton" class="hidden layui-row layui-col-space15">
        <button id="back" class="layui-btn layui-btn-primary" onclick="backToButtons()">返回</button>
    </div>

    <!-- 登录表单 -->
    <div id="loginForm" style="display:none; padding-top: 20px;">
        <div class="layui-card">
            <div class="layui-card-header">登录</div>
            <div class="layui-card-body">
                <form class="layui-form" action="login.php" method="post" onsubmit="return checkForm(this)" lay-filter="example">
                    <div class="layui-form-item">
                        <label class="layui-form-label">账号</label>
                        <div class="layui-input-block">
                            <input type="text" name="account" placeholder="请输入账号" class="layui-input">
                        </div>
                    </div>
                    <div class="layui-form-item">
                        <label class="layui-form-label">密码</label>
                        <div class="layui-input-block">
                            <input type="password" name="password" placeholder="请输入密码" class="layui-input">
                        </div>
                    </div>			 
                    <div class="layui-form-item">
                        <div class="layui-input-block">
                            <input type="submit" class="layui-btn layui-btn-normal" value="立即登录"/>
                        </div>
                    </div>
                </form>
            </div>
        </div>
    </div>

    <!-- 注册表单 -->
    <div id="registerForm" style="display:none; padding-top: 20px;">
        <div class="layui-card">
            <div class="layui-card-header">注册</div>
            <div class="layui-card-body">
                <form class="layui-form" action="register.php" method="post" onsubmit="return checkForm(this)" lay-filter="example">
                    <div class="layui-form-item">
                        <label class="layui-form-label">账号</label>
                        <div class="layui-input-block">
                            <input type="text" name="account" placeholder="请输入账号" class="layui-input">
                        </div>
                    </div>
                    <div class="layui-form-item">
                        <label class="layui-form-label">密码</label>
                        <div class="layui-input-block">
                            <input type="password" name="password" placeholder="请输入密码" class="layui-input">
                        </div>
                    </div>			 
                    <div class="layui-form-item">
                        <div class="layui-input-block">
                            <input type="submit" class="layui-btn layui-btn-normal" value="立即注册"/>
                        </div>
                    </div>
                </form>
            </div>
        </div>
    </div>
</div>

<script type="text/javascript">
// 验证输入不为空的脚本代码
function checkForm(form) { 
    if(form.username.value == "") { 
        alert("用户名不能为空!");
        form.username.focus();
        return false;
    }
    if(form.password.value == "") { 
        alert("密码不能为空!");
        form.password.focus();
        return false;
    }
    return true;
}

// 切换表单显示
function toggleForm(formType) {
    // 隐藏按钮并显示返回按钮
    document.getElementById('buttons').classList.add('hidden');
    document.getElementById('backButton').classList.remove('hidden');
    
    // 隐藏标题
    document.getElementById('pageTitle').classList.add('hidden');

    // 显示对应的表单并隐藏另一个表单
    if (formType === 'login') {
        document.getElementById('loginForm').style.display = 'block';
        document.getElementById('registerForm').style.display = 'none';
    } else if (formType === 'register') {
        document.getElementById('registerForm').style.display = 'block';
        document.getElementById('loginForm').style.display = 'none';
    }
}

// 返回按钮的功能
function backToButtons() {
    // 显示按钮并隐藏返回按钮
    document.getElementById('buttons').classList.remove('hidden');
    document.getElementById('backButton').classList.add('hidden');
    
    // 隐藏表单
    document.getElementById('loginForm').style.display = 'none';
    document.getElementById('registerForm').style.display = 'none';

    // 显示标题
    document.getElementById('pageTitle').classList.remove('hidden');
}
</script>

</body>
</html>
