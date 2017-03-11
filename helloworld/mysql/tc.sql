create database tc;
use tc;
create table shop_info (
    shop_id int primary key not null,
    city_name varchar(32) character set utf8,
    location_id int,
    per_pay int,
    score int null,
    comment_cnt int null,
    shop_level int,
    cate_1_name varchar(32) character set utf8,
    cate_2_name varchar(32) character set utf8,
    cate_3_name varchar(32) character set utf8 null
);
create table user_pay (
    user_id int not null,
    shop_id int not null,
    time_stamp datetime
);
create table user_view (
    user_id int not null,
    shop_id int not null,
    time_stamp datetime
);

create table city_weather (
    city_name varchar(32) character set utf8,
    weather_date date,
    high_temp int,
    low_temp int,
    weather varchar(32) character set utf8,
    wind_direction varchar(32) character set utf8,
    wind_speed varchar(32) character set utf8
);

