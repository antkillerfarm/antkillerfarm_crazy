library(rjson)
library(RMySQL)

data_file_path<-"/home/data/tj/my/antkillerfarm_crazy/helloworld/scrapy/tutorial/data/"

get_date_from_file_name<-function(file_name){
  rule <- regexpr("/[^:punct:]*.json", file_name)
  date<-substr(file_name,rule[1]+1,rule[1]+attr(rule,"match.length")-6)
  date1<-gsub("_", "-", date)
  return(date1)
}

handle_file<-function(file_name){
  lines<-readLines(file_name)
  len<-length(lines)
  lines[1]<-paste("{\"data\":[",lines[1],",")
  for (i in 2:(len-1)) {
    lines[i]<-paste(lines[i],",")
  }
  lines[len]<-paste(lines[len],"]}")
  json_data<-fromJSON(paste(lines, sep="",collapse=""))
  json_data$file_name<-get_date_from_file_name(file_name)
  return(json_data)
}

json_to_db<-function(conn,json){
  for (i in 1:length(json$data)){
    sql<-paste("INSERT INTO csdn(name,s_time,read_cnt) values('",
               json$data[[i]]$title,"','",json$file_name,"','",
               json$data[[i]]$readCount,"')",sep="")
    print(sql)
    dbSendQuery(conn,sql)
  }
}

conn<-dbConnect(MySQL(), dbname = "ml", username="root", password="123456")
summary(conn)

file_list<-list.files(data_file_path)

for(i in file_list) {
  file_name<-paste(data_file_path,i,sep="")
  print(file_name)
  json_data<-handle_file(file_name)
  json_to_db(conn,json_data)
}

#dbSendQuery(conn,"INSERT INTO csdn(name,s_time,read_cnt) values('R插入的新文章','1937-7-7',50)");
query<-dbSendQuery(conn, "SELECT * FROM csdn where read_cnt>250")
data<-fetch(query, n = -1)
dbClearResult(query)
print(data)
dbDisconnect(conn)
