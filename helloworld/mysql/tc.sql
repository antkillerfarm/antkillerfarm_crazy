create database tc;
use tc;
create table shop_info (
    shop_id int primary key not null,
    city_name varchar(256) character set utf8,
    location_id int,
    per_pay int,
    score int,
    comment_cnt int,
    shop_level int,
    cate_1_name varchar(256) character set utf8,
    cate_2_name varchar(256) character set utf8,
    cate_3_name varchar(256) character set utf8
);
