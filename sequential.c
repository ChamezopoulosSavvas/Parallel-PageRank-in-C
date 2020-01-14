#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

struct timeval startwtime, endwtime;
double seq_time;

int main(int argc, char** argv){

	gettimeofday (&startwtime, NULL);

	if(argc!=2){
		printf("Usage: %s data_filename\n", argv[0]);
		exit(1);
	}

	FILE *fp;

	fp = fopen(argv[1],"r");
	if(fp == NULL) {
        fprintf(stderr,"ERROR: Cannot open the file");
        exit(1);
    }

    // Read the data set and get the number of nodes (n)
    int N,E;
    char ch;
    char str[100];
    ch = getc(fp);
    while(ch =='#'){
        if(fgets(str,100-1,fp)==NULL){
        	fprintf(stderr,"ERROR: Cannot read from the file");
        	exit(1);
        }
        //Debug: print title of the data set
        //printf("%s",str);
        sscanf (str,"%*s %d %*s %d", &N, &E); 
        ch = getc(fp);
    }

    printf("Number of nodes: %d\nNumber of Edges: %d\n", N, E);

    float **A;
	int i, j, fromNodeId, toNodeId;

	// Preallocate the adjacency matrix 'a' and allocate each element to 0   
	A = (float **) malloc(N*sizeof(float*));
	for(i = 0; i < N; i++) {
	  	A[i] = calloc(N, sizeof(float));
	}

	printf("Reading graph...\n");
	//read and store graph
	while(!feof(fp)){

	    if(fscanf(fp,"%d%d", &toNodeId, &fromNodeId)<2 && !feof(fp)){
	    	printf("ERROR: Cannot read graph\n");
	    	exit(1);
	    }
	    //printf("fromNodeId: %d, toNodeId: %d\n", fromNodeId, toNodeId);
	    //if there is a connection between the nodes, mark it in the adjency matrix
	    A[fromNodeId][toNodeId] = 1.0;
	    //printf("In matrix A[%d][%d]: %f\n", fromNodeId, toNodeId, A[fromNodeId][toNodeId]);
	}

	printf("Preparing PR, outBound, AT arrays...\n");
	float *PR;
	
	PR = (float *) malloc(N*sizeof(float));
	// Initialize the PR[] vector
	//each node gets an initial PR of 1/N
	for(i=0; i<N; i++) {
		PR[i] = 1.0/N;
	}

	int *outBound;

	//initialize outBound[] vector
	outBound = calloc(N, sizeof(int));
	
	//set outBound
	for(i=0; i<N; i++){
		for(j=0; j<N; j++){

			if(A[i][j] != 0.0){
				outBound[i]++;
			}

		}
	}

	// Make the matrix stochastic
	for(i=0; i<N; i++){

		if(outBound[i] == 0){
			// Deal with dangling nodes
			for(j=0; j<N; j++){
				A[i][j] = 1.0/N;
			}

		}
		//Distribute the PR along the outBound links
		else{
			for(j=0; j<N; j++){
				if (A[i][j] != 0) {
					//if not 0, its 1 at this point
					A[i][j] = A[i][j]/outBound[i];
				}

			}
		}
	}

	
	float **AT;
	//transpose A[][]. This way we get the array AT[how am i?][who points to me?]
	//as opposed to the previous format A[how am i?][who do i point to?]	
	AT = (float **) malloc(N*sizeof(float*));
	for(i=0; i<N; i++) {
	  	AT[i] = (float *) malloc(N*sizeof(float));
	}
	for(i=0; i<N; i++){
		for(j=0; j<N; j++){
	  		AT[j][i]=A[i][j];
	  		if(i==j) AT[i][j] = 0;
	  	}
	}
	//A is no longer needed
	free(A);

	printf("Initializing PageRank algorithm...\n");
	//damping factor
	float a=0.85;

	// Set the looping condition and the number of iterations
	int loop = 1;
	int niter = 0;
	float error;
	
	// Initialize PR_new[] vector
	float *PR_new;

	PR_new = (float *) malloc(N*sizeof(float));

	while(loop){

		//reset PR_new
		for(i=0; i<N; i++){
			PR_new[i] = 0;
		}

		//calculate PR_new using Gauss Seidel Method
		for(i=0; i<N; i++){

			//add (1-a) 
			PR_new[i] += 1-a;
			for(j=0; j<N; j++){
				
				if(j<i){

					PR_new[i] += AT[i][j]*PR_new[j];
				}
				else if(j>i){

					PR_new[i] += AT[i][j]*PR[j];
				}
			}
			PR_new[i] *= a;
		}

		//check if we have to stop
	    error = 0.0;
	    for(i=0; i<N; i++){
	        error += fabs(PR_new[i] - PR[i]);
	    }
	    //if two consecutive instances of pagerank vector are almost identical, stop
	    if (error < 0.000001){
	        loop = 0;
	    }

	    // Update PR[]
	    for (i=0; i<N; i++){
	    	PR[i] = PR_new[i];
		}

		niter++;
	}

	printf("Finished.\n");

	gettimeofday (&endwtime, NULL);

	seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
		      + endwtime.tv_sec - startwtime.tv_sec);

	printf("Number of iterations: %d\n", niter);

	printf("Wall clock time = %f\n", seq_time);

	free(AT);
	free(PR);
	free(PR_new);
	free(outBound);

	return 0;
}