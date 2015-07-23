This file contains a minimal working example of the Rcpp wrapper.

The "src" folder contains the "main model" which is written in C++. In this example, the model simply takes a 2-column matrix as an input, and outputs a vector of each of the row sums.

The main.R script takes data.txt as an input, runs the input through the C model (using the Rcpp package) and then prints the resulting "syntheticdata.txt".
