library(ggplot2)

print(women)

p <- qplot(women$height, women$weight, xlab="height",
           ylab="weight",col='blue',geom = "point") 
p + geom_vline(col="red") 

p <- qplot(women$height, women$weight, xlab="height",
           ylab="weight",col='blue',geom = c("point","smooth")) 
p + geom_vline(col="red") 

###

x<-seq(-2*pi,2*pi,by=0.01)

s1<-data.frame(x,y=sin(x),type=rep('sin',length(x)))
s2<-data.frame(x,y=cos(x),type=rep('cos',length(x)))
s3<-data.frame(x,y=tan(x),type=rep('tan',length(x)))
s4<-data.frame(x,y=1/tan(x),type=rep('cot',length(x)))
s5<-data.frame(x,y=1/sin(x),type=rep('sec',length(x)))
s6<-data.frame(x,y=1/cos(x),type=rep('csc',length(x)))
df<-rbind(s1,s2,s3,s4,s5,s6)


g<-ggplot(df,aes(x,y))
g<-g+geom_line(aes(colour=type,stat='identity'))
g<-g+scale_y_continuous(limits=c(0, 2))
g<-g+scale_x_continuous(breaks=seq(-2*pi,2*pi,by=pi),labels=c("-2*pi","-pi","0","pi","2*pi"))
g

###

num=8
x<-seq(0,num*pi,by=0.01)

index<-"0"
for (i in 1:num) {
  str<-paste(as.character(i),"*pi",sep="")
  index<-c(index,str)
}

s1<-data.frame(x,y=sin(x/5-pi/4)/(x/5),type=rep('sin0',length(x)))
df<-rbind(s1)

g<-ggplot(df,aes(x,y))
g<-g+geom_line(aes(colour=type,stat='identity'))
g<-g+scale_y_continuous(limits=c(-1, 1))
g<-g+scale_x_continuous(breaks=seq(0,8*pi,by=pi),labels=index)
g
