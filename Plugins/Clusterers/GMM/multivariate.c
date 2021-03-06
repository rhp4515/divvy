//
//  multivariate.c
//
//
//  Written in 2014 by Jeremy Karnowski at the UC San Diego Natural Computation Lab,
//  Based on code written in 2011 by Josh Lewis
//  PI Virginia de Sa, supported by NSF Award SES #0963071.
//  Copyright 2011, UC San Diego Natural Computation Lab. All rights reserved.
//  Licensed under the MIT License. http://www.opensource.org/licenses/mit-license.php
//
//  Find the Divvy project on the web at http://divvy.ucsd.edu

#include "multivariate.h"

void cholesky(double *L, double *A, int n) {
    
    // Taken from http://rosettacode.org/wiki/Cholesky_decomposition#C
    
    //double *L = (double*)calloc(n * n, sizeof(double));
    if (L == NULL)
        exit(EXIT_FAILURE);
    
    for (int i = 0; i < n; i++)
        for (int j = 0; j < (i+1); j++) {
            double s = 0;
            for (int k = 0; k < j; k++)
                s += L[i * n + k] * L[j * n + k];
            L[i * n + j] = (i == j) ?
            sqrt(A[i * n + i] - s) :
            (1.0 / L[j * n + j] * (A[i * n + j] - s));
        }
    
    //return L;
    //return 0;
}

void transpose(double *AT, double *A, int n) {
    //double *AT = (double*)calloc(n * n, sizeof(double));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            AT[j * n + i] = A[i * n + j];
        }
    }
    //return AT;
}

void *lowerTriangleInverse(double* lowTriInv, double *C, int dims) {
    
    // Conversion of MATLAB code from
    // http://stackoverflow.com/questions/12239409/matlab-algorithm-for-finding-inverse-of-matrix
    
    //double *lowTriInv = (double*)calloc(dims * dims, sizeof(double));
    double *identity = (double*)calloc(dims * dims, sizeof(double));
    
    // Create Identity Matrix and set lowTriInv to zeros
    // calloc sets things to zero!
    for(int i = 0; i < dims; i++) {
        identity[i * dims + i] = 1;
    }
    
    // Calculate the inverse
    double sum = 0;     // used for the dot product in every loop
    for(int k = 0; k < dims; k++) {
        for(int i = 0; i < dims; i++) {
            
            // Take the dot product of appropriate row and column
            for(int b = 0; b < k; b++) {
                sum += C[k * dims + b] * lowTriInv[b * dims + i];
            }
            
            lowTriInv[k * dims + i] = (identity[k * dims + i] - sum)/C[k * dims + k];
            
            sum = 0;    // have to reset sum
            
        }
    }
    
    //return lowTriInv;
    
    free(identity);
    
    return 0;
}

void dot(double *prod, double *A, double *B, int n, int m, int o, int p) {
    // Matrix A is of size nxm
    // Matrix B is of size oxp
    // m = o
    // So product is of size nxp
    //double *prod = (double*)calloc(n * p, sizeof(double));
    
    // Clear prod first
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < p; j++) {
            prod[i * p + j] = 0;
        }
    }
    
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < p; j++) {
            for(int k = 0; k < m; k++) {
                prod[i * p + j] += A[i * m + k] * B[k * p + j];
            }
        }
    }
}


void createpdfs(double* mus, double* covs, double* covInvs, double *constants, int k, int d) {
 
    double *tmpMean = (double*)calloc(d, sizeof(double));
    double *tmpCov = (double*)calloc(d * d, sizeof(double));
    
    double *cho = (double*)calloc(d * d, sizeof(double));
    double *choInv = (double*)calloc(d * d, sizeof(double));
    double *choInvTrans = (double*)calloc(d * d, sizeof(double));
    double *covInv = (double*)calloc(d * d, sizeof(double));
    
    // For each distribution, compute the inverse of covariance matrix and the constant term (includes determinant of cov)
    for(int i = 0; i < k; i++) {
        
//        printf("Cov\n");
//        for(int i = 0; i<d; i++) {
//            for(int j = 0; j<d; j++) {
//                printf("%f ", covs[i*d+j]);
//            }
//            printf("\n");
//        }
        
        double covDet = 1;
        double constant = 0;
        
        // Extract i-th mean
        for(int j = 0; j < d; j++) {
            tmpMean[j] = mus[i * d + j];
        }
        
        // Extract i-th cov
        for(int l = 0; l < d; l++) {
            for(int m = 0; m < d; m++) {
                tmpCov[l*d + m] = covs[i*(d*d) + l*d + m];
            }
        }
        
//        printf("Temp Cov\n");
//        for(int i = 0; i<d; i++) {
//            for(int j = 0; j<d; j++) {
//                printf("%f ", tmpCov[i*d+j]);
//            }
//            printf("\n");
//        }
        
        // Compute the inverse of the covariance matrix
        cholesky(cho, tmpCov,d);
        
//        printf("Cholesky\n");
//        for(int i = 0; i<d; i++) {
//            for(int j = 0; j<d; j++) {
//                printf("%f ", cho[i*d+j]);
//            }
//            printf("\n");
//        }
        
        lowerTriangleInverse(choInv,cho,d);
        
//        printf("Lower Triangle Inverse\n");
//        for(int i = 0; i<d; i++) {
//            for(int j = 0; j<d; j++) {
//                printf("%f ", choInv[i*d+j]);
//            }
//            printf("\n");
//        }
        
        transpose(choInvTrans,choInv,d);
        dot(covInv, choInvTrans, choInv, d, d, d, d);
        
//        printf("Covariance Inverse\n");
//        for(int i = 0; i<d; i++) {
//            for(int j = 0; j<d; j++) {
//                printf("%f ", covInv[i*d+j]);
//            }
//            printf("\n");
//        }
        
        // Compute the determinant of the covariance matrix
        // det(A) = sqrt(det(L)) where L is the cholesky decomposition
        // det(L) = product of diagonal
        for(int l = 0; l < d; l++) {
            covDet *= cho[l * d + l];
        }
        covDet = covDet*covDet;
        
        // Compute the constant term for multivariate gaussian
        constant = sqrt(pow(2 * M_PI, -d) * pow(covDet, -1));

        // Add the constant term and the inverse of covariance to the list for all distributions
        constants[i] = constant;
        for(int j = 0; j < d; j++) {
            for(int k = 0; k < d; k++) {
                covInvs[i*(d*d) + j*d + k] = covInv[j*d + k];
            }
        }
        
    }
    
    free(tmpMean);
    free(tmpCov);
    free(cho);
    free(covInv);
    free(choInv);
    free(choInvTrans);
}



void mvnpdf(double *dist, float *data, double *means, double *vecMinusMean, double *firstProd, double *invcov, double cnst, int n, int d, int j, int l) {
    
    // Compute the probability of a point under a Multivariate Gaussian
    
    double eExp = 0;
    //double *vecMinusMean = (double*)calloc(d, sizeof(double));
    //double *firstprod = (double*)calloc(d*d, sizeof(double));
    double expval = 0;
    
    for(int i = 0; i < d; i++) {
        vecMinusMean[i] = data[j*d + i] - means[l*d + i];
    }
    
    dot(firstProd, vecMinusMean, invcov, 1, d, d, d);
    dot(&expval, firstProd, vecMinusMean, 1, d, d, 1);
    
    eExp = exp(-0.5 * expval);
    
    //return (cnst * eExp);
    dist[j] = cnst * eExp;
    
    //free(vecMinusMean);
    //free(firstprod);
}







