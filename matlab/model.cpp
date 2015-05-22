#include <math.h>
#include <string.h>
#include <cstddef>
#include <set>
#include <map>


#include "SpecialFunctions.cpp"
using namespace std;

SpecialFunctions sf;

void init(int n, int B, int G, double *para, double ga, 
        double gb, double sigma_square,MTRand & mtr ) {
    int i;
    para[0] = sqrt(sigma_square)*mtr.randNorm(0.0,1.0); //mu_beta
    para[1] = sf.gammarand(ga, 1.0/gb,mtr);  //phi_beta
    para[2] = sqrt(sigma_square)*mtr.randNorm(0.0,1.0); //mu_gamma
    para[3] = sf.gammarand(ga, 1.0/gb,mtr); //phi_gamma
    for (i = 4; i < 4+B; i++) {
        para[i] = para[0] + sqrt(para[1])*mtr.randNorm(0.0,1.0); //BETA
    }
    for (i = 4+B; i < 4+B+G; i++) {
        para[i] = para[2] + sqrt(para[3])*mtr.randNorm(0.0,1.0);//GAMMA
    }
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

void calculateLambda(double *data, double *Beta, double *Gamma, double *Lambda, int n) {
    int Ncol = 5;
    for (int i = 0; i < n; i++) {
        int bindex = (int)data[i * Ncol + 3] - 1; //from one based to zero based
        int gindex = (int)data[i * Ncol + 4] - 1; 
        Lambda[i] = exp(Beta[bindex] + Gamma[gindex]);
    }
}

int sampleBG(double *data, double *BETA, double *GAMMA, double MU, double PHI, int which, double scale, int upperlimit, int Acc,
        int n, int B, int G, double *BG, MTRand & mtr) {
    
    double *lambda = new double[n];
    double *lambda_new = new double[n];
    double *p = new double[n * (upperlimit+1)];
    double *p_new = new double[n * (upperlimit+1)];
    double *logdiff = NULL;
    int i;
    int nCol = 5;
    
    calculateLambda(data, BETA, GAMMA, lambda, n);
    UpperTruncatedPoissonPDF(lambda, p, n, upperlimit+1);
   
    int dataindex;
    int nBG;
    double *PARA = NULL;
    if (which == 0) {
        dataindex = 3;
        nBG = B;
        PARA = BETA;
    } else {
        dataindex = 4;
        nBG = G;
        PARA = GAMMA;
    }
    double *PARA_new = new double[nBG];
    for (i =0; i < nBG; i++) {
        PARA_new[i] = PARA[i] + scale * mtr.randNorm(0.0,1.0);
    }
    if (which == 0) {
        calculateLambda(data, PARA_new, GAMMA, lambda_new, n);
    } else {
        calculateLambda(data, BETA, PARA_new, lambda_new, n);
    }
    UpperTruncatedPoissonPDF(lambda_new, p_new, n, upperlimit+1);
    
    logdiff = new double[nBG];
    for (i =0; i < nBG; i++) {
        logdiff[i] =  -PHI*(PARA_new[i] - MU)* (PARA_new[i] - MU) / 2.0;
        logdiff[i] +=  PHI*(PARA[i] - MU)* (PARA[i] - MU) / 2.0; 
    }
    
    for (i = 0; i < n; i++) {
        int BGindex = (int)data[i * nCol + dataindex] - 1;
        int y  = (int)data[i * nCol + 2] - 1;
        logdiff[BGindex] +=  log(p_new[i*(upperlimit+1)+y]);
        logdiff[BGindex] -=  log(p[i*(upperlimit+1)+y]);
    }
    
    for (i =0; i < nBG; i++) {
        if (logdiff[i] > log(mtr.rand())) {
            BG[i] = PARA_new[i];
            Acc = Acc + 1;
        } else {
            BG[i] = PARA[i];
        }
    }
    
    delete [] logdiff;
    delete [] PARA_new;
    delete [] p;
    delete [] lambda_new;
    delete [] lambda;
    return Acc;
}

void sampleMuPhi(double *Para, int N, double PHI, double sigma_square, double ga, double gb, double *pMu, double *pPhi, MTRand & mtr) {
    
	double mean_para = 0.0;
	int i;
	for (i=0; i < N; i++) {
		mean_para += Para[i];
	}
    double phi_star = double(N) * PHI + 1.0 / sigma_square;
    double mu_star  = double(N) * PHI * mean_para / phi_star;
    double Mu = mu_star +  mtr.randNorm(0.0,1.0) / sqrt(phi_star);
    
    double a = ga + double(N)/2.0;
	double dsum = 0.0;
	for (i =0; i < N; i++) {
		dsum += (Para[i] - mean_para) * (Para[i] - mean_para);
	}
    double b = gb + dsum / 2.0;
    double Phi = sf.gammarand(a, 1.0/b,mtr);
	*pMu = Mu;
	*pPhi = Phi;
}

double* DoModel(double *data, int n, unsigned int seed, 
        int niters, int burnin, int stride, 
        int& betaAcc, int& gammaAcc, int& numberofmodels, int& npara) {
    int i;
    int B, G;
    MTRand mt;
	mt.seed(seed);
    
    double beta_c = 0.1; 
    double gamma_d = 0.1;;
    int upperlimit = 8;
    double sigma_square = 25.0;
    double ga = 0.01; 
    double gb = 0.01;
   
    int nsave = (niters - burnin) / stride + 2;
    double *new_data = new double[n*5];
    reformatData(data,n ,B,G,new_data);
    npara = B + G + 4;
    double *result = new double[npara * nsave];
    double  *current_para = result;
    init(n, B, G, current_para, ga, gb,sigma_square,mt);
    
    int count = 0;
    numberofmodels = 1;
    
    betaAcc  = 0;
    gammaAcc = 0;
    
    for (int iter = 0; iter < niters; iter++) {
        count++;
        double *BETA = current_para + 4;
        double *GAMMA = BETA + B;
        betaAcc  += sampleBG(new_data, BETA, GAMMA, current_para[0], current_para[1], 0, beta_c,  upperlimit, 0, n, B, G,BETA,mt);
        gammaAcc += sampleBG(new_data, BETA, GAMMA, current_para[2], current_para[3], 1, gamma_d, upperlimit, 0, n, B, G,GAMMA,mt);
        sampleMuPhi(BETA, B, current_para[1], sigma_square, ga, gb, current_para+0, current_para+1, mt);
        sampleMuPhi(GAMMA,G, current_para[3], sigma_square, ga, gb, current_para+2, current_para+3, mt);
        
        if (count == stride) {
            if (iter >= burnin) {
                numberofmodels++;
                for (i =0; i< npara; i++) {
                    current_para[i+npara] = current_para[i];   
                }
                current_para += npara;
            }
            count = 0;
        }
    }
    delete [] new_data;
    return result;
}
