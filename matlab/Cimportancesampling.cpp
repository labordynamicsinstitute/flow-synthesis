#include <math.h>
#include "mex.h"

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

void importancesampling(double *lambdas, double *data, double *yobs, 
        double *result,int L, int m, int n, int upperlimit) {
    int t,y,h,l,s,k;
    
    double *P1 = new double[L*n * (upperlimit+1)];
    UpperTruncatedPoissonPDF(lambdas, P1, L*n, upperlimit+1);
    
    double *P2 = new double[L* n * (upperlimit+1)];
    int SizeYL = (upperlimit+1) * L; 
    for (t = 0; t < n; t++) {
        for (y=1;y <= upperlimit+1; y++) {
            double yminusyt = y - yobs[t];
            double dsum = 0;
            for (h =0; h < L; h++) {
                int index = t * SizeYL + (y-1) * L + h;
                P2[index] = pow(lambdas[h*n+t],yminusyt);
                dsum += P2[index];
            }
            for (h =0; h < L; h++) {
                int index = t * SizeYL + (y-1) * L + h;
                P2[index] /= dsum;
            }
        }
    }
    
    //calculate the left-hand side of 1.4.5, which has 3 indexes (y,l,t) for
    //possible y values, saved lambda draws and each cell(obs)
    double *P_YMN = new double[(upperlimit+1)* m * n];
    for (y=1;y <= upperlimit+1; y++) {
        for (l=0; l < m; l++) {
            for (t=0; t< n; t++) { //outer loops
                
                //start of inner loop
                double dylt = 0.0;
                for (s=0; s < L; s++) {
                    double dprod = 1.0;
                    for (k=0; k < n; k++) {
                        dprod *= P1[(s*n+k)*(upperlimit+1) + (int)data[l*n+k] - 1];
                    }
                    dprod *= P2[t * SizeYL + (y-1) * L + s];
                    dylt += dprod;
                }
                P_YMN[(y-1)* m * n + l * n + t] = dylt / (double)L;
                //end of inner loop
                
            }
        }
    }
    
    //calculate the left-hand side of 1.4.3, which has 2 indexes (y,t) 
    for (t=0; t< n; t++) {
        double dsum = 0.0;
        for (y=1;y <= upperlimit+1; y++) {
            double dprod = 1.0;
            for (l=0; l < m; l++) {
                dprod *= P_YMN[(y-1)* m * n + l * n + t];
            }
            result[t * (upperlimit+1) + y -1] = dprod;
            dsum += dprod;
        }
        for (y=1;y <= upperlimit+1; y++) {
            result[t * (upperlimit+1) + y -1] /= dsum;
        }
    }
                 
    delete [] P1;
    delete [] P2;
    delete [] P_YMN;
                    
}
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    double *lambdas, *syntheticdata,*y_obs, *result;
    
    int L,m,n;
    int upperlimit = 8;
    
    lambdas = mxGetPr(prhs[0]);
    n = (int)mxGetM(prhs[0]);
    L = (int)mxGetN(prhs[0]);
    
    syntheticdata = mxGetPr(prhs[1]);
    m = (int)mxGetN(prhs[1]);
    if ((int)mxGetM(prhs[1]) != n) {
        mexErrMsgTxt( "syntheticdata:Dimmensions do not match.");
    }
    
    y_obs = mxGetPr(prhs[2]);
    if ((int)(mxGetM(prhs[2]) * mxGetN(prhs[2])) != n) {
        mexErrMsgTxt( "yobs:Dimmensions do not match.");
    }
    plhs[0] = mxCreateDoubleMatrix((mwSize)(upperlimit+1),(mwSize)n,  mxREAL);
    result = mxGetPr(plhs[0]);
    
    importancesampling(lambdas, syntheticdata, y_obs, result,L, m,n,upperlimit);
    
}
