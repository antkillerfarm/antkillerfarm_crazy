library(rjson)
library(RMySQL)

handle_file<-function(file_name){
  lines<-readLines(file_name)
  len<-length(lines)
  lines[1]<-paste("{\"data\":[",lines[1],",")
  for (i in 2:(len-1)) {
    lines[i]<-paste(lines[i],",")
  }
  lines[len]<-paste(lines[len],"]}")
  json_data<-fromJSON(paste(lines, collapse=""))
  return(json_data)
}

json_data<-handle_file("/home/data/my/antkillerfarm_crazy/helloworld/scrapy/tutorial/2016_10_11.json")

json_to_db<-function(conn,json){
  for (i in 1:length(json$data)){
    sql<-paste("INSERT INTO csdn(name,s_time,read_cnt) values('",
          json$data[[i]]$title,"','2016-10-11','",
          json$data[[i]]$readCount,"')",collapse="")
    print(sql)
  }
}

json_to_db(conn,json_data)

conn<-dbConnect(MySQL(), dbname = "ml", username="root", password="123456")
summary(conn)



dbSendQuery(conn,"INSERT INTO csdn(name,s_time,read_cnt) values('R插入的新文章','1937-7-7',50)");
query<-dbSendQuery(conn, "SELECT * FROM csdn")
data<-fetch(query, n = -1)
dbClearResult(query)
print(data)
dbDisconnect(conn)
