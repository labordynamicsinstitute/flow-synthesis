%%
%This is an example Matlab using MTCD package 
%Authors: Quanli Wang, Jerry Reiter & Sam Hawala

%% compile the mex/c++ code used by the example
mex DoModel.cpp
mex CsampleFromModel.cpp
mex Cimportancesampling.cpp


%%
clear all
%%data preparation
data = importdata('data/data.txt');


%%model fitting 
randseed = 12345;
[result Acc] = DoModel(data',randseed,150000,30000,1000);
result = result';

%%
ndraw = 100;
samples = zeros(size(data,1),ndraw * length(result(:,1)));
lambdas = zeros(size(data,1), length(result(:,1)));
for i = 1: length(result(:,1))
    [lambda draw ] = CsampleFromModel(data',result(i,:),ndraw,randseed);
    samples(:,(i-1) * ndraw + 1:i*ndraw) = draw';
    lambdas(:,i) = lambda;
end
lambdas = lambdas';

m = 10;  %3 synthetic datasets
samples = samples(:,1:m)';
yobs = data(:,3)';
outputdata = [lambdas' samples' yobs'];
result1 = Cimportancesampling(lambdas',samples',yobs)';
