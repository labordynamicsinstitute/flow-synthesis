// This program creates a function (main_model) which takes a 2-column matrix as input and outputs a vector of row sums

//1. Premable required for Rcpp
#include <Rcpp.h>
using namespace Rcpp;

//2. Function definition (Note: the comment below, which is called an "attribute", is required for Rcpp)
// [[Rcpp::export(name="main_model")]]
NumericVector rowsumsC(NumericMatrix x) {
  int nrow = x.nrow(), ncol = x.ncol();
  NumericVector out(nrow);

  for (int i = 0; i < nrow; i++) {
    double total = 0;
    for (int j = 0; j < ncol; j++) {
      total += x(i, j);
    }
    out[i] = total;
  }
  return out;
}
