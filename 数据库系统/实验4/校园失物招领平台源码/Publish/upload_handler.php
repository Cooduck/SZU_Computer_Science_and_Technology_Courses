<?php
include "../connect.php"; 
session_start();

// 设置返回 JSON 格式
header("Content-Type: text/plain");

$user_account = $_SESSION['user_account'];

// 查询 user_id
$sql_user = "SELECT user_id FROM user WHERE user_account = '$user_account'";
$result = $con->query($sql_user);

if ($result->num_rows > 0) {
    $row = $result->fetch_assoc();
    $publish_user_id = $row['user_id'];
} else {
    header("Location: publish.php?result=failure&message=用户未找到，请重新登录！");
    exit;
}

// 获取表单数据
$announcement_type = $_POST['announcement_type'];
$item_name = $_POST['item_name'];
$item_category_name = $_POST['item_category_name'];
$item_description = $_POST['item_description'];
$lost_location = $_POST['lost_location'];
$pickup_location = $_POST['pickup_location'];
$item_status = "未处理";

// 图片处理
$upload_dir = '../picture/'; // 指定图片存储目录
if (!is_dir($upload_dir)) {
    mkdir($upload_dir, 0777, true); // 如果目录不存在则创建
}

if ($_FILES['item_image']['error'] !== UPLOAD_ERR_OK) {
    switch ($_FILES['item_image']['error']) {
        case UPLOAD_ERR_INI_SIZE:
        case UPLOAD_ERR_FORM_SIZE:
            $error_message = "文件大小超出限制！";
            break;
        case UPLOAD_ERR_PARTIAL:
            $error_message = "文件只上传了一部分，请重试！";
            break;
        case UPLOAD_ERR_NO_FILE:
            $error_message = "没有文件被上传！";
            break;
        default:
            $error_message = "文件上传失败，错误代码：" . $_FILES['item_image']['error'];
    }
    header("Location: publish.php?result=failure&message=$error_message");
    exit;
} else {
    $file_extension = pathinfo($_FILES['item_image']['name'], PATHINFO_EXTENSION);
    $new_filename = uniqid('img_', true) . '.' . $file_extension; // 生成唯一文件名
    $file_path = $upload_dir . $new_filename;

    if (!move_uploaded_file($_FILES['item_image']['tmp_name'], $file_path)) {
        header("Location: publish.php?result=failure&message=文件保存失败！");
        exit;
    }
}

// 开始事务
mysqli_begin_transaction($con);

try {
    // 插入数据到 Item 表
    $sql_item = "INSERT INTO Item (item_name, item_category_name, item_description, item_status) 
                 VALUES ('$item_name', '$item_category_name', '$item_description', '$item_status')";

    if (!$con->query($sql_item)) {
        header("Location: publish.php?result=failure&message=物品信息插入失败！");
        exit;
    }

    $item_id = $con->insert_id;

    // 插入数据到 ItemPhoto 表，保存图片路径
    $sql_photo = "INSERT INTO ItemPhoto (item_id, photo_url) VALUES (?, ?)";
    $stmt_photo = $con->prepare($sql_photo);
    $stmt_photo->bind_param("is", $item_id, $file_path);

    if (!$stmt_photo->execute()) {
        header("Location: publish.php?result=failure&message=插入照片路径失败！");
        exit;
    }

    if ($announcement_type === "lost") {
        $sql_lost = "INSERT INTO post_lost_item (user_id, item_id, lost_location, lost_date_time) 
                     VALUES ($publish_user_id, $item_id, '$lost_location', NOW())";

        if (!$con->query($sql_lost)) {
            header("Location: publish.php?result=failure&message=失物信息插入失败！");
            exit;
        }
    } elseif ($announcement_type === "found") {
        $sql_pickup = "INSERT INTO post_pickup_item (user_id, item_id, pickup_location, pickup_date_time) 
                       VALUES ($publish_user_id, $item_id, '$pickup_location', NOW())";

        if (!$con->query($sql_pickup)) {
            header("Location: publish.php?result=failure&message=招领信息插入失败！");
            exit;
        }
    } 

    mysqli_commit($con);
    header("Location: publish.php?result=success&message=发布成功！");
} catch (Exception $e) {
    mysqli_rollback($con);
    header("Location: publish.php?result=failure&message=" . $e->getMessage());
}

mysqli_close($con);
?>


