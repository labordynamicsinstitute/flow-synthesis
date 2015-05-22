#include <math.h>
#include "mex.h"
#include "MersenneTwister.h"
#include <set>
#include <map>
using namespace std;

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

void reformatData(double *data, int N, int &B, int &G, double *new_data) { //one based columns
    set<int> UniqueSource;
    set<int> UniqueDest; 
    map<int,int> SourceMap;
	map<int,int> DestMap;
	set<int>::iterator it;
	int i;
	int p = 3; //number of er o
    for (i = 0; i < N; i++) {
        UniqueSource.insert((int)data[i*p]);
        UniqueDest.insert((int)data[i*p+1]);
    }
	B = 0;
	for (it=UniqueSource.begin(); it!=UniqueSource.end(); ++it) {
		B++;
		SourceMap[*it] = B;
	}
	G = 0;
	for (it=UniqueDest.begin(); it!=UniqueDest.end(); ++it) {
		G++;
		DestMap[*it] = G;
	}
	int newcol = p + 2;
	for (i = 0; i < N; i++) {
		int base = i * newcol;
		new_data[base] = data[i *p];
		new_data[base+1] = data[i*p+1];
		new_data[base+2] = data[i*p+2];
		new_data[base+3] = SourceMap[(int)data[i *p]];
		new_data[base+4] = DestMap[(int)data[i *p+1]];
	}
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
void calculateLambda(double *data, double *Beta, double *Gamma, double *Lambda, int n) {
    int Ncol = 5;
    for (int i = 0; i < n; i++) {
        int bindex = (int)data[i * Ncol + 3] - 1; //from one based to zero based
        int gindex = (int)data[i * Ncol + 4] - 1; 
        Lambda[i] = exp(Beta[bindex] + Gamma[gindex]);
    }
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    double *model, *data, *result;
    double *lambda;
    int upperlimit = 8;
    int n;
    int B, G;
    int ndraw;
    
    MTRand mt;
    
    data = mxGetPr(prhs[0]);
    n = (int)mxGetN(prhs[0]);
    if (mxGetM(prhs[0]) != 3) {
        mexErrMsgTxt( "Data martrix must have 3 rows.");
    }
    
    double *new_data = new double[n*5];
    reformatData(data,n ,B,G,new_data);
    int npara = B + G + 4;
    
    model = mxGetPr(prhs[1]);
    if (npara != (mxGetM(prhs[1])*mxGetN(prhs[1]))) {
        mexErrMsgTxt( "Model dimmension does not match with data.");
    }
    
    ndraw = (int)mxGetScalar(prhs[2]);
    
    if (seed  == 0) {
        seed = (unsigned int)mxGetScalar(prhs[3]);
        mt.seed(seed);
    }
    
    plhs[0] = mxCreateDoubleMatrix((mwSize)n,1,  mxREAL);
    lambda = mxGetPr(plhs[0]);
    
    plhs[1] = mxCreateDoubleMatrix((mwSize)ndraw,(mwSize)n,  mxREAL);
    result = mxGetPr(plhs[1]);
    
    double *BETA = model + 4;
    double *GAMMA = BETA + B;
    calculateLambda(new_data, BETA, GAMMA, lambda, n);
    sampleLambdas(lambda, result, n, ndraw, upperlimit, mt);
    
}
