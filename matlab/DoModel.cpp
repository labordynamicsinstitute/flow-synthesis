
#include "mex.h"
#include "model.cpp"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    double *data, *pAcc, *result;
    double *models;
    int i,numberofmodels,npara,n;
    int betaAcc, gammaAcc;
    
    unsigned int seed;
    int niters,burnin,stride;
    
    data = mxGetPr(prhs[0]);
    n = (int)mxGetN(prhs[0]);
    if (mxGetM(prhs[0]) != 3) {
        mexErrMsgTxt( "Data martrix must have 3 rows.");
    }
    seed = (unsigned int)mxGetScalar(prhs[1]);
    niters = (int)mxGetScalar(prhs[2]);
    burnin = (int)mxGetScalar(prhs[3]);
    stride = (int)mxGetScalar(prhs[4]);
    models = DoModel(data,n, seed, niters, burnin, stride, 
            betaAcc, gammaAcc, numberofmodels,npara);
    plhs[0] = mxCreateDoubleMatrix((mwSize)npara, (mwSize)numberofmodels, mxREAL);
    result = mxGetPr(plhs[0]);
    for ( i = 0; i <numberofmodels * npara;i++) {
        result[i] = models[i];
    }
   
    plhs[1] = mxCreateDoubleMatrix(1, 2, mxREAL);
    pAcc = mxGetPr(plhs[1]);
    pAcc[0] = betaAcc;
    pAcc[1] = gammaAcc;
 
}
