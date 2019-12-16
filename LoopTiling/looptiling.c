#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define k 1024

float (*MakeRandom_kXk_Array())[k];
//k by k random matrix A generation
void printfunc(float (*arr)[k]);
//matrix printing funcion
float (*Experiment_1(float (*A)[k], float (*B)[k]))[k];
//Matmul function without using loop tiling
float (*Experiment_2(float (*A)[k], float (*B)[k], int B0))[k];
//Matmul function using lopp tiling

int main()
{
	int B1 = 16;
	int B2 = 64;
	int B3 = 256;
	float (*a)[k] = MakeRandom_kXk_Array();
	float (*b)[k] = MakeRandom_kXk_Array();
	clock_t start0, start1, start2, start3, end0, end1, end2, end3;
	float expt0, expt1, expt2, expt3; 
	//CPU time check variables
	
	printf("%dby%d A random matrix is generated\n",k,k);
	printf("%dby%d B random matrix is generated\n",k,k);
	//printf("A\n");
	//printfunc(a);
	//printf("B\n");
	//printfunc(b);
	printf("Sanghyeon Lee (UIN: 729007123)------------------------\n");
	
	start0 = clock();
	float (*y0)[k] = Experiment_1(a, b);
	end0 = clock();
	expt0 = (float)(end0 - start0)/CLOCKS_PER_SEC;
	//printf("Experiment 1) With tiling\nA*B=Y:\n");
	//printfunc(y0);
	
	printf("Experiment 0 ( when K = %d, B = %d ) runtime : %f sec\n\n", k, k, expt0);
	
	start1 = clock();
	float (*y1)[k] = Experiment_2(a, b, B1);
	end1 = clock();
	expt1 = (float)(end1 - start1)/CLOCKS_PER_SEC;
	printf("Experiment 1 ( when K = %d, B = %d ) runtime : %f sec\n\n", k, B1, expt1);
	
	start2 = clock();
	float (*y2)[k] = Experiment_2(a, b, B2);
	end2 = clock();
	expt2 = (float)(end2 - start2)/CLOCKS_PER_SEC;
	printf("Experiment 2 ( when K = %d, B = %d ) runtime : %f sec\n\n", k, B2, expt2);
	
	start3 = clock();
	float (*y3)[k] = Experiment_2(a, b, B3);
	end3 = clock();
	expt3 = (float)(end3 - start3)/CLOCKS_PER_SEC;
	printf("Experiment 3 ( when K = %d, B = %d ) runtime : %f sec\n\n", k, B3, expt3);
	
	return 0;
}

float (*MakeRandom_kXk_Array())[k]
{
	static float arr[k][k];
	for(int i = 0; i<k;i++)
	{
		for (int j = 0; j<k;j++)
		{
			arr[i][j] = rand()%k;
		}
	}
	return arr;
}

void printfunc(float (*arr)[k])
{
	for(int i = 0; i<k;i++)
	{
		for (int j = 0; j<k;j++)
		{
			printf("%3f ",arr[i][j]);
		}
		printf("\n");
	}
}

float (*Experiment_1(float (*A)[k], float (*B)[k]))[k]
{
	float sum1 = 0;
	static float y[k][k];
	
	for(int i=0;i<k;i++)
	{
		for(int j=0;j<k;j++)
		{
			sum1 = 0;
			for(int m=0;m<k;m++)
			{
				sum1 += A[i][m]*B[m][j];
			}
			y[i][j] = sum1;
		}
	}
	return y;
}

float (*Experiment_2(float (*A)[k], float (*B)[k], int B0))[k]
{
	float sum2 = 0;
	int B1 = B0;
	int B2 = B0;
	int B3 = B0;
	
	static float y[k][k];
	
	for(int ii=0;ii<k;ii+=B1)
	{
		for(int jj=0;jj<k;jj+=B2)
		{
			for(int mm=0;mm<k;mm+=B3)
			{
				for(int i=ii; i<ii+B1;i++)
				{
					for(int j=jj;j<jj+B2;j++)
					{
						sum2 = 0;
						for(int m=mm;m<mm+B3;m++)
						{
							sum2 += A[i][m]*B[m][j];
						}
						y[i][j] += sum2;
					}	
				}
			}
		}
	}	
	return y;
}

