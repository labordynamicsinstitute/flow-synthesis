#include <math.h>
#include "mex.h"
#include "MersenneTwister.h"

unsigned int seed = 0;
//duplicated code, move later
void UpperTruncatedPoissonPDF(double *lambda, double *p, int N, int n) {
    for (int i = 0; i < N;i++) {
        int base = i * n;
        p[base] = 1.0;
        double dsum = p[base];
        int j;
        for (j = 1; j < n; j++) {
            p[base + j] = p[base + j-1] * (lambda[i] / double(j)); 
            dsum += p[base + j];
        }
        for (j = 0; j < n; j++) {
            p[base + j] = p[base + j] / dsum; 
        }
    }
}  

int sample(double *p, int n, double d) {
    double dsum;
    int i,k;
    dsum = 0;
    double *myw;
    myw = new double[n];
    for (i = 0; i < n;i++) {
        dsum+=p[i];
    }
    myw[0] = p[0] / dsum;
    for (i = 1; i < n;i++) {
        myw[i] = p[i] / dsum + myw[i-1];
    }
    
    for(k=0;k < n && d>myw[k];k++)
        ;
    delete [] myw;
    return k+1;
}


/*
 *draw samples from a list of lambdas
Input:
    lambds:     a 1 by m vector of lambdas
    m:          Number of lambdas
    n:          Number of samples per lambda
    upperlimt:  the upper truncated limit
    mtr:        the random number generator
 Output:        
    result: the random samples. Caller is responable for allocating memory
 */
void sampleLambdas(double *lambdas, double *result, int m, int n, int upperlimit, MTRand & mtr) {
    int i,j;
    double *p = new double[m*(upperlimit+1)];
    UpperTruncatedPoissonPDF(lambdas, p, m, upperlimit+1);

    for (i=0; i< m; i++) {
        for (j =0; j < n; j++) {
            double d = mtr.rand();
            result[i*n+j] = sample(p + i*(upperlimit+1), (upperlimit+1), d);
        }
    }
    delete [] p;               
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    double *lambdas, *syntheticdata,*y_obs, *result;
    
    int m,n; //number of lambdas
    int upperlimit = 8;
    
    MTRand mt;
    
    lambdas = mxGetPr(prhs[0]);
    m = (int)(mxGetM(prhs[0]) * mxGetN(prhs[0]));
    n = (int)mxGetScalar(prhs[1]);
    if (seed  == 0) {
        seed = (unsigned int)mxGetScalar(prhs[1]);
        mt.seed(seed);
    }
    
    
    plhs[0] = mxCreateDoubleMatrix((mwSize)n,(mwSize)m,  mxREAL);
    result = mxGetPr(plhs[0]);
   
    sampleLambdas(lambdas, result, m, n, upperlimit, mt);
    
}
