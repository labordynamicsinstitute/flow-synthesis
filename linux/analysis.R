data <- read.delim("data.txt", header=FALSE)
summary(data)
# replace the observed values
l <-length(data$V3)
data$V3 <- sample(1:20, l, replace=T)
write.table(data,file="data2.txt",sep = "\t",row.names = FALSE, col.names = FALSE)
# Call out to mtcd
system("./mtcd -n=100 -m=10 data2.txt")
syntheticdata <- read.table("syntheticdata.txt", header=TRUE, quote="\"")
# have a look at the first implicate
summary(syntheticdata$sd1)
table(syntheticdata$sd1)
