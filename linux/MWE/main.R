library(Rcpp)

# Load Data
data <- read.table("data.txt", header=FALSE)

# Load model written in C
sourceCpp("./src/model.cpp")

# Extract Columns of Interest
input_data = as.matrix(data[,2:3])

# Run model on input data
synthetic = main_model(input_data)
fsyn=as.data.frame(synthetic)

# Bind model output with original data
output=cbind(data,fsyn)

# Export Data
write.table(output, file="syntheticdata.txt", sep="\t",quote=FALSE,row.names=F,col.names=F)

