create database ml;
use ml;
create table csdn (
    id int auto_increment primary key not null,
    name varchar(256) character set utf8,
    s_time date,
    read_cnt int
);
