use tc;
create view shop1629 as select count(shop_id),date(time_stamp) as dates from user_pay where shop_id='1629' group by dates order by dates asc;
create table shop1629_1 (select count(shop_id) as shop_count,date(time_stamp) as dates from user_pay where shop_id='1629' group by dates order by dates asc);
create table shop58_1 (select count(shop_id) as shop_count,date(time_stamp) as dates from user_pay where shop_id='58' group by dates order by dates asc);
create table shop1010_1 (select count(shop_id) as shop_count,date(time_stamp) as dates from user_pay where shop_id='1010' group by dates order by dates asc);
select * from shop1629_1;
select sum(shop_count),weekday(dates) from shop1629_1 group by weekday(dates);
create table shopcount (select shop_id, count(shop_id) as shop_count,date(time_stamp) as dates from user_pay group by dates,shop_id order by dates asc);
select shop_count,dates from shopcount where shop_id='1629' order by dates asc;
