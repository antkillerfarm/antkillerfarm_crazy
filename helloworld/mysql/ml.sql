create database ml;
use ml;
create table csdn (
    id int auto_increment primary key not null,
    name varchar(256),
    s_time date,
    read_cnt int
);
