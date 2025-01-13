/*==============================================================*/
/* DBMS name:      MySQL 5.0                                    */
/* Created on:     2024/12/12 23:55:12                          */
/*==============================================================*/


drop trigger if exists addReceipt;

drop trigger if exists addDishonestPerson;

drop procedure if exists GetReceiveReceiptsByUser;

drop procedure if exists GetReceiveReceiptsByDateRange;

drop VIEW if exists viewUnprocessedReceipts;

drop VIEW if exists viewDishonestUsers;

/*==============================================================*/
/* Table: ReceiveReceipt                                        */
/*==============================================================*/
DROP TABLE IF EXISTS ReceiveReceipt;
create table ReceiveReceipt
(
   receiveReceipt_id    int not null AUTO_INCREMENT,
   item_id              int not null,
   user_id              int not null,
   receiveReceipt_status varchar(30) not null,
   receiveReceipt_time  DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
   primary key (receiveReceipt_id)
) ENGINE=InnoDB;

/*==============================================================*/
/* Table: administrator                                         */
/*==============================================================*/
DROP TABLE IF EXISTS administrator;
create table administrator
(
   admin_id             int not null AUTO_INCREMENT,
   user_id              int not null,
   admin_role           longtext not null,
   assigned_time        DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
   primary key (admin_id)
) ENGINE=InnoDB;

/*==============================================================*/
/* Table: administratorLog                                      */
/*==============================================================*/
DROP TABLE IF EXISTS administratorLog;
create table administratorLog
(
   log_id               int not null AUTO_INCREMENT,
   admin_id             int not null,
   operation_type       varchar(30) not null,
   operation_content    text not null,
   operation_time       DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
   primary key (log_id)
) ENGINE=InnoDB;

/*==============================================================*/
/* Table: comment                                               */
/*==============================================================*/
DROP TABLE IF EXISTS comment;
create table comment
(
   comment_id           int not null AUTO_INCREMENT,
   item_id              int not null,
   user_id              int not null,
   comment_content      text not null,
   comment_time         DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
   primary key (comment_id)
) ENGINE=InnoDB;

/*==============================================================*/
/* Table: complaint                                             */
/*==============================================================*/
DROP TABLE IF EXISTS complaint;
create table complaint
(
   complaint_id         int not null AUTO_INCREMENT,
   receiveReceipt_id    int not null not null,
   user_id              int not null not null,
   complaint_category   varchar(30) not null,
   complaint_reason     text not null,
   primary key (complaint_id)
) ENGINE=InnoDB;

/*==============================================================*/
/* Table: dishonestPerson                                       */
/*==============================================================*/
DROP TABLE IF EXISTS dishonestPerson;
create table dishonestPerson
(
   blacklist_id         int not null AUTO_INCREMENT,
   user_id              int not null,
   added_time           DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
   blacked_reason       text not null,
   admin_id             int not null,
   primary key (blacklist_id)
) ENGINE=InnoDB;

/*==============================================================*/
/* Table: handleComplaints                                      */
/*==============================================================*/
DROP TABLE IF EXISTS handleComplaints;
create table handleComplaints
(
   admin_id             int not null,
   complaint_id         int not null,
   complaint_status     varchar(30) not null,
   complaint_response   text,
   handle_time          DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
   primary key (admin_id, complaint_id)
) ENGINE=InnoDB;

/*==============================================================*/
/* Table: item                                                  */
/*==============================================================*/
DROP TABLE IF EXISTS item;
create table item
(
   item_id              INT not null AUTO_INCREMENT,
   item_name            varchar(30) not null,
   item_category_name   varchar(30) not null,
   item_description     text not null,
   item_status          varchar(30) not null,
   primary key (item_id)
) ENGINE=InnoDB;

/*==============================================================*/
/* Table: itemPhoto                                             */
/*==============================================================*/
DROP TABLE IF EXISTS itemPhoto;
create table itemPhoto
(
   photo_id             INT not null AUTO_INCREMENT,
   item_id              int not null,
   photo_url            varchar(255) not null,
   primary key (photo_id)
) ENGINE=InnoDB;

/*==============================================================*/
/* Table: post_lost_item                                        */
/*==============================================================*/
DROP TABLE IF EXISTS post_lost_item;
create table post_lost_item
(
   user_id              int not null,
   item_id              int not null,
   lost_location        varchar(50) not null,
   lost_date_time       DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
   primary key (user_id, item_id)
) ENGINE=InnoDB;

/*==============================================================*/
/* Table: post_pickup_item                                      */
/*==============================================================*/
DROP TABLE IF EXISTS post_pickup_item;
create table post_pickup_item
(
   user_id              int not null,
   item_id              int not null,
   pickup_location      varchar(50) not null,
   pickup_date_time     DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
   primary key (user_id, item_id)
) ENGINE=InnoDB;

/*==============================================================*/
/* Table: user                                                  */
/*==============================================================*/
DROP TABLE IF EXISTS user;
create table user
(
   user_id              int not null AUTO_INCREMENT,
   user_account         varchar(30) not null,
   user_password        varchar(30) not null,
   user_id_card         varchar(30),
   user_name            varchar(30),
   user_contact_info    varchar(30),
   user_status          varchar(30) not null,
   primary key (user_id)
) ENGINE=InnoDB;

create unique index account_UK on user(user_account);

/*==============================================================*/
/* Table: userLog                                               */
/*==============================================================*/
DROP TABLE IF EXISTS userLog;
create table userLog
(
   log_id               int not null AUTO_INCREMENT,
   user_id              int not null,
   operation_type       varchar(30) not null,
   operation_content    text not null,
   operation_time       DATETIME DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
   primary key (log_id)
) ENGINE=InnoDB;

/*==============================================================*/
/* View: viewDishonestUsers                                     */
/*==============================================================*/
create VIEW  viewDishonestUsers
as
SELECT 
    u.user_id,
    u.user_id_card,
    u.user_name,
    u.user_contact_info,
    u.user_status,
    dp.added_time,
    dp.blacked_reason,
    dp.admin_id
FROM 
    DishonestPerson dp
JOIN 
    User u ON dp.user_id = u.user_id;

/*==============================================================*/
/* View: viewUnprocessedReceipts                                */
/*==============================================================*/
create VIEW  viewUnprocessedItem
as
SELECT 
    i.item_name,
    i.item_category_name,
    i.item_description,
    c.comment_content,
    c.user_id,
    c.comment_time
FROM 
    item i
LEFT JOIN 
    comment c ON i.item_id = c.item_id
WHERE 
    i.item_status = '未处理';

alter table ReceiveReceipt add constraint FK_item_receive_receipt foreign key (item_id)
      references item (item_id) on delete restrict on update restrict;

alter table ReceiveReceipt add constraint FK_user_receive_receipt foreign key (user_id)
      references user (user_id) on delete restrict on update restrict;

alter table administrator add constraint FK_administrator_user foreign key (user_id)
      references user (user_id) on delete restrict on update restrict;

alter table administratorLog add constraint FK_administrator_administratorLog foreign key (admin_id)
      references administrator (admin_id) on delete restrict on update restrict;

alter table comment add constraint FK_comment_item foreign key (item_id)
      references item (item_id) on delete restrict on update restrict;

alter table comment add constraint FK_user_comment foreign key (user_id)
      references user (user_id) on delete restrict on update restrict;

alter table complaint add constraint FK_receive_receipt_complaint foreign key (receiveReceipt_id)
      references ReceiveReceipt (receiveReceipt_id) on delete restrict on update restrict;

alter table complaint add constraint FK_user_complaint foreign key (user_id)
      references user (user_id) on delete restrict on update restrict;

alter table dishonestPerson add constraint FK_administrator_dishonestPerson foreign key (admin_id)
      references administrator (admin_id) on delete restrict on update restrict;

alter table dishonestPerson add constraint FK_用户_失信人员 foreign key (user_id)
      references user (user_id) on delete restrict on update restrict;

alter table handleComplaints add constraint FK_handleComplaints foreign key (complaint_id)
      references complaint (complaint_id) on delete restrict on update restrict;

alter table handleComplaints add constraint FK_handleComplaints2 foreign key (admin_id)
      references administrator (admin_id) on delete restrict on update restrict;

alter table itemPhoto add constraint FK_item_itemPhoto foreign key (item_id)
      references item (item_id) on delete restrict on update restrict;

alter table post_lost_item add constraint FK_post_lost_item foreign key (item_id)
      references item (item_id) on delete restrict on update restrict;

alter table post_lost_item add constraint FK_post_lost_item2 foreign key (user_id)
      references user (user_id) on delete restrict on update restrict;

alter table post_pickup_item add constraint FK_post_pickup_item foreign key (item_id)
      references item (item_id) on delete restrict on update restrict;

alter table post_pickup_item add constraint FK_post_pickup_item2 foreign key (user_id)
      references user (user_id) on delete restrict on update restrict;

alter table userLog add constraint FK_user_log foreign key (user_id)
      references user (user_id) on delete restrict on update restrict;

-- 为 receiveReceipt 表的 user_id 添加索引
CREATE INDEX idx_receiveReceipt_user_id ON receiveReceipt(user_id);

-- 为 dishonestPerson 表的 user_id 添加索引
CREATE INDEX idx_dishonestPerson_user_id ON dishonestPerson(user_id);

DELIMITER $$

CREATE PROCEDURE GetReceiveReceiptsByUser(
    IN input_user_id INT      -- 输入参数：用户ID
)
BEGIN
    -- 查询指定用户的所有领回单信息
    SELECT 
        rr.receiveReceipt_id,
        rr.item_id,
        rr.user_id,
        rr.receiveReceipt_status,
        rr.receiveReceipt_time
    FROM 
        receiveReceipt rr
    WHERE 
        rr.user_id = input_user_id;
END$$

DELIMITER ;

DELIMITER $$

CREATE PROCEDURE GetReceiveReceiptsByDateRange(
    IN start_date DATETIME,    -- 输入参数：开始时间
    IN end_date DATETIME       -- 输入参数：结束时间
)
BEGIN
    -- 查询在指定时间段内的领回单信息
    SELECT 
        rr.receiveReceipt_id,
        rr.item_id,
        rr.user_id,
        rr.receiveReceipt_status,
        rr.receiveReceipt_time
    FROM 
        receiveReceipt rr
    WHERE 
        rr.receiveReceipt_time BETWEEN start_date AND end_date;
END$$

DELIMITER ;

DELIMITER $$

CREATE TRIGGER userLog_receiveReceipt_creation
AFTER INSERT ON receiveReceipt
FOR EACH ROW
BEGIN    
    -- 在用户操作日志表中插入记录
    INSERT INTO userLog (user_id, operation_type, operation_content, operation_time)
    VALUES (NEW.user_id, '创建领回单', CONCAT('创建领回单，领回单ID: ', NEW.receiveReceipt_id, '，物品ID: ', 
            NEW.item_id, '，领回人ID: ', NEW.user_id), NOW());
END$$

DELIMITER ;

DELIMITER $$

CREATE TRIGGER administratorLog_dishonestPerson_addition
AFTER INSERT ON dishonestPerson
FOR EACH ROW
BEGIN
    INSERT INTO administratorLog (admin_id, operation_type, operation_content, operation_time)
    VALUES (NEW.admin_id, '添加用户至失信名单', 
        CONCAT('用户 ', NEW.user_id, ' 因 ', NEW.blacked_reason, ' 被添加至失信名单。'), NOW());
END$$

DELIMITER ;