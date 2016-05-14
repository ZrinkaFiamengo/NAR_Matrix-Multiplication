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
#define N 10000
#define M 8000
#define MAX_NUMBER_OF_THREADS 20	//everything above this is ignored
#define printING_ENABLED false //set true for displaying data

short* MakeVector(short size)
{
	short *b;
	b = (short*)malloc(size * sizeof(short));
	return b;
}

short** MakeMatrix(short sizeX, short sizeY)
{
	short **A;
	A = (short**)malloc(sizeX * sizeof(short*));
	for (short i = 0; i < sizeX; i++)
		A[i] = MakeVector(sizeY);
	return A;
}

void printMatrix(short sizeX, short sizeY, short **A)
{
	for (short i = 0; i < sizeX; i++)
	{
		for (short j = 0; j < sizeY; j++)
		{
			std::cout << A[i][j] << "\t";
		}
		std::cout << std::endl;
	}
}

void printVector(short size, short *vec)
{
	for (short i = 0; i < size; i++)
	{
		std::cout << vec[i] << std::endl;
	}
}

void initZero(short size, short *vec)
{
	for (short i = 0; i < size; i++)
	{
		vec[i] = 0;
	}
}

void initRandomNumbers(short size, short range, short *vec)
{
	for (short i = 0; i < size; i++)
	{
		vec[i] = rand() % range;
	}
}

void initRandomNumbers(short sizeX, short sizeY, short range, short **A)
{
	for (short i = 0; i < sizeX; i++)
	{
		for (short j = 0; j < sizeY; j++)
		{
			A[i][j] = rand() % range;
		}
	}
}

void transposeMatrix(short sizeX, short sizeY, short **A, short **AT)
{
	for (short i = 0; i < sizeX; i++)
	{
		for (short j = 0; j < sizeY; j++)
		{
			AT[j][i] = A[i][j];
		}
	}
}

void multiplyVectorWithConstant(short size, short *vec, short number, short *result)
{
	for (short i = 0; i < size; i++)
	{
		result[i] = vec[i] * number;
	}
}

void addVectorToVector(short size, short *vec1, short *vec2)
{
	for (short i = 0; i < size; i++)
	{
		vec1[i] += vec2[i];
	}
}

void multiplyByRows(short start, short end, short sizeY, short **A, short *x, short *b)
{
	for (short i = start; i < end; i++)
	{
		for (short j = 0; j < sizeY; j++)
		{
			b[i] += A[i][j] * x[j];
		}
	}
}

void multiplyByColumns(short start, short end, short sizeX, short **A, short *x, short *b)
{
	for (short i = start; i < end; i++)
	{
		short *tempA = MakeVector(sizeX);
		for (short j = 0; j < sizeX; j++)
		{
			tempA[j] = A[j][i] * x[i];
		}
		addVectorToVector(sizeX, b, tempA);
		free(tempA);
	}
}

void multiplyByRowsDivideByCollumns(short start, short end, short sizeX, short **A, short *x, short *b)
{
	for (short i = 0; i < sizeX; i++)
	{
		short temp = 0;
		for (short j = start; j < end; j++)
		{
			temp += A[i][j] * x[j];
		}
		b[i] += temp;
	}
}

void multiplyByColumnsTransposed(short start, short end, short sizeX, short **AT, short *x, short *b)
{
	short *temp_result = MakeVector(sizeX);
	for (short i = start; i < end; i++)
	{
		multiplyVectorWithConstant(sizeX, AT[i], x[i], temp_result);
		addVectorToVector(sizeX, b, temp_result);
	}
	free(temp_result);
}
