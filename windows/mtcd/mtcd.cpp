//#pragma warning (disable:4146)
//#pragma warning (disable:4996)

#include "stdafx.h"
#include "cmdline.h"
#include "MersenneTwister.h"
#include "model.h"
using namespace std;

void usage_exit(const char* app_name) {
    cerr << "Usage: " << app_name << " [options] <datafile>" << endl;
    cerr << endl;

    cerr << "Options:" << endl;

    cerr << " -h --help			Display this message"                       << endl;
    cerr << " -n --nobs			Number of observations"						<< endl;
	cerr << " -r --randomseed		Random seed"						<< endl;
	cerr << " -i --iters			Number of iterations"							<< endl;
	cerr << " -b --burnin			Number of burnins"							<< endl;
	cerr << " -s --stride			Stride"							<< endl;
	cerr << " -o --output			Output file name"							<< endl;
	cerr << " -m --nsynthetic		Number of synthetic datasets"							<< endl;
	cerr << " -v --verbose			Output debug information"							<< endl;
	cerr << " -u --upperlimit		Upper limit for truncated Poisson distribution"							<< endl;
    cerr << endl;

    cerr << endl;
	//getc(stdin);
    exit(1);
}

int main(int argc,char* argv[]) {
	CmdLine cmdline(argc, argv);
	
	// for model fitting
	unsigned int seed;
    int m,n, niters,burnin,stride, UPPERLIMIT;
    string  modelfile,riskfile,synfile;

	bool verbose = false;
	m = 10;
	n = 0; 
	niters = 100000;
	burnin = 30000;
	stride = 500;
	seed = 0;
	UPPERLIMIT = 8;

	modelfile = "models.txt";
	synfile = "syntheticdata.csv";
	riskfile = "riskmeasure.txt";
	
    if (argc < 2)
        usage_exit(cmdline.app_name());
	try {
        while (cmdline.next()) {
            if (cmdline.is_option()) {
                if (cmdline.match(NULL, "modelfile", true))
                    modelfile.assign(cmdline.value());
				else if (cmdline.match(NULL, "syntheticdatafile", true))
                    synfile.assign(cmdline.value());
				else if (cmdline.match(NULL, "riskmeasurefile", true))
                    riskfile.assign(cmdline.value());
                else if (cmdline.match("n", "nobs", true))
                    n = (int)cmdline.value_as_long(0, 100000);
                else if (cmdline.match("r", "randomseed", true))
                    seed = (unsigned int) cmdline.value_as_long();
                else if (cmdline.match("i", "iters", true))
                    niters = (int) cmdline.value_as_long();
                else if (cmdline.match("b", "burnin", true))
					burnin = (int) cmdline.value_as_long();
                else if (cmdline.match("s", "stride", true)) 
					stride = (int) cmdline.value_as_long();
				else if (cmdline.match("m", "nsynthetic", true)) 
					m = (int) cmdline.value_as_long();
				else if (cmdline.match("u", "upperlimit", true)) 
					UPPERLIMIT = (int) cmdline.value_as_long();
				else if (cmdline.match("v", "verbose", false)) 
					verbose = true;
                else
                    usage_exit(cmdline.app_name());
            } else {
                const char* infile = cmdline.argument();
				if (NULL != infile) {
					Run(infile, n, seed, niters, burnin, stride, modelfile, synfile, riskfile, m,verbose, UPPERLIMIT);
				}
            }
        } 
    }
    catch (std::exception& e) {
        cerr << e.what() << endl;
        return -1;
    }

	//std::cout << "done." << endl;
	//getc(stdin);
	return 0;
}