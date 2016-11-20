library(rjson)
library(RMySQL)

filelist<-list.files("/home/data/my/antkillerfarm_crazy/helloworld/scrapy/tutorial/")

lines<-readLines("/home/data/my/antkillerfarm_crazy/helloworld/scrapy/tutorial/2016_10_11.json")
len<-length(lines)
lines[1]<-paste("{\"data\":[",lines[1],",")
for (i in 2:(len-1)) {
  lines[i]<-paste(lines[i],",")
}
lines[len]<-paste(lines[len],"]}")
json_data<-fromJSON(paste(lines, collapse=""))

add<-function(x,y){
  return (x+y)
}
fff<-add(3,4)

conn<-dbConnect(MySQL(), dbname = "ml", username="root", password="123456")
summary(conn)
query<-dbSendQuery(conn, "SELECT * FROM csdn")
data<-fetch(query, n = -1)
dbClearResult(query)
print(data)
dbDisconnect(conn)

