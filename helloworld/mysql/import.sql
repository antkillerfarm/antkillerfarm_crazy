use tc;
load data local infile '/home/data/tj/my/tianchi/dataset/shop_info.csv'   
into table shop_info    
fields terminated by ',' optionally enclosed by '"' escaped by '"'   
lines terminated by '\n';

load data local infile '/home/data/tj/my/tianchi/dataset/user_view.csv'   
into table user_view    
fields terminated by ',' optionally enclosed by '"' escaped by '"'   
lines terminated by '\n';

load data local infile '/home/data/tj/my/tianchi/dataset/user_pay.csv'   
into table user_pay    
fields terminated by ',' optionally enclosed by '"' escaped by '"'   
lines terminated by '\n';

load data local infile '/home/data/tj/my/tianchi/20170112/extra_user_view.csv'   
into table user_view    
fields terminated by ',' optionally enclosed by '"' escaped by '"'   
lines terminated by '\n';

load data local infile '/home/data/tj/my/tianchi/city_weather.csv'   
into table city_weather    
fields terminated by ',' optionally enclosed by '"' escaped by '"'   
lines terminated by '\r\n';
