#pragma once
#include <string>
#include <iostream>
#include <thread>
#include <chrono>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <vector>

//PROGRAM PARAMETERS!

//Matrix sizes: A[M][N], x[N], b[M] (A*x=b)
#define N 500
#define M 200
#define MAX_NUMBER_OF_THREADS 20	//everything above this is ignored
#define PRINTING_ENABLED false //set true for displaying data


void printMatrix(int A[M][N])
{
	for (int i = 0; i < M; i++)
	{
		for (int j = 0; j < N; j++)
		{
			std::cout << A[i][j] << "\t";
		}
		std::cout << std::endl;
	}
}

void printVector(int size, int *vec)
{
	for (int i = 0; i < size; i++)
	{
		std::cout << vec[i] << std::endl;
	}
}

void initZero(int size, int *vec)
{
	for (int i = 0; i < size; i++)
	{
		vec[i] = 0;
	}
}

void initRandomNumbers(int size, int range, int *vec)
{
	for (int i = 0; i < size; i++)
	{
		vec[i] = rand() % range;
	}
}

void initRandomNumbers(int range, int A[M][N])
{
	for (int i = 0; i < M; i++)
	{
		for (int j = 0; j < N; j++)
		{
			A[i][j] = rand() % range;
		}
	}
}

void transposeMatrix(int A[M][N], int AT[N][M])
{
	for (int i = 0; i < M; i++)
	{
		for (int j = 0; j < N; j++)
		{
			AT[j][i] = A[i][j];
		}
	}
}

void multiplyVectorWithConstant(int size, int *vec, int number, int *result)
{
	for (int i = 0; i < size; i++)
	{
		result[i] = vec[i] * number;
	}
}

void addVectorToVector(int size, int *vec1, int *vec2)
{
	for (int i = 0; i < size; i++)
	{
		vec1[i] += vec2[i];
	}
}

void multiplyByRows(int start, int end, int A[M][N], int *x, int *b)
{
	for (int i = start; i < end; i++)
	{
		for (int j = 0; j < N; j++)
		{
			b[i] += A[i][j] * x[j];
		}
	}
}


void multiplyByColumns(int start, int end, int AT[N][M], int *x, int *b)
{
	int temp_result[M];
	for (int i = start; i < end; i++)
	{
		multiplyVectorWithConstant(M, AT[i], x[i], temp_result);
		addVectorToVector(M, b, temp_result);
	}
}
