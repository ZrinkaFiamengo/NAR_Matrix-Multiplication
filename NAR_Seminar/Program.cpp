#include <string>
#include <iostream>
#include <thread>
#include <chrono>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <vector>
#include "Functions.h"

//PROGRAM PARAMETERS-inside Functions.h!

using namespace std;

typedef std::chrono::high_resolution_clock Clock;

//Timers 
class Timer
{
public:
	Timer() : beg_(clock_::now()) {}
	void reset() { beg_ = clock_::now(); }
	double elapsed() const {
		return chrono::duration_cast<chrono::microseconds>
			(clock_::now() - beg_).count();
	}

private:
	typedef chrono::high_resolution_clock clock_;
	typedef chrono::duration<double, ratio<1> > second_;
	chrono::time_point<clock_> beg_;
};


int main()
{
	vector<int> broj_niti = { 2, 3, 4, M, N };
	int **A, *x, *b;	//A*x=b
	int **AT; //A transposed
	double timer_start, timer_end;
	Timer tmr;

	srand(time(NULL));

#pragma region Inicijalizacije podataka
	cout << "Inicijalizacija matrica:" << endl;
	timer_start = tmr.elapsed();

	A = MakeMatrix(M, N);
	initRandomNumbers(10, A);
	x = MakeVector(N);
	initRandomNumbers(N, 10, x);

	if (PRINTING_ENABLED)
	{
		cout << "Matrica A:" << endl;
		printMatrix(A);
		cout << "Vektor x:" << endl;
		printVector(N, x);
	}

	timer_end = tmr.elapsed();
	std::cout << "->proteklo vrijeme: " << timer_end - timer_start << endl;
	std::cout << endl << "--------------------------------------------" << endl<<endl;
#pragma endregion

#pragma region Sekvencijalno mnozenje Ax=b po redcima matrice A
	cout << "Sekvencijalno mnozenje Ax=b po redcima matrice A:" << endl;
	timer_start = tmr.elapsed();

	b = MakeVector(M);
	initZero(M, b);
	multiplyByRows(0, M, A, x, b);

	if (PRINTING_ENABLED)
	{
		cout << "Vektor b:" << endl;
		printVector(M, b);
	}

	timer_end = tmr.elapsed();
	std::cout << "->proteklo vrijeme: " << timer_end - timer_start << endl;
	std::cout << endl << "--------------------------------------------" << endl << endl;
#pragma endregion

#pragma region Paralelno mnozenje Ax=b po redcima matrice A
	cout << "Paralelno mnozenje Ax=b po redcima matrice A:" << endl;
	std::cout <<endl << "******************" << endl;

	for (unsigned int nit = 0; nit < broj_niti.size(); nit++)
	{
		if (broj_niti[nit] <= MAX_NUMBER_OF_THREADS)
		{
			thread threads[MAX_NUMBER_OF_THREADS];
			int segment_size = M / broj_niti[nit];
			
			cout << "Broj niti: " << broj_niti[nit] << endl;

			timer_start = tmr.elapsed();

			initZero(M, b);

			unsigned int i = 0;
			for (i=0; i < broj_niti[nit] - 1; i++)
			{
				threads[i] = thread(multiplyByRows, i *segment_size, i*segment_size + segment_size, A, x, b);
			}
			threads[i] = thread(multiplyByRows, i *segment_size, M, A, x, b);

			for (int i = 0; i < broj_niti[nit]; i++)
			{
				threads[i].join();
			}

			if (PRINTING_ENABLED)
			{
				cout << "Vektor b:" << endl;
				printVector(M, b);
			}

			timer_end = tmr.elapsed();
			std::cout << "->proteklo vrijeme: " << timer_end - timer_start << endl;
			std::cout << "******************" << endl;
		}
	}
	std::cout << endl << "--------------------------------------------" << endl << endl;
#pragma endregion

#pragma region Sekvencijalno mnozenje Ax=b po stupcima matrice A
	cout << "Sekvencijalno mnozenje Ax=b po stupcima matrice A:" << endl;
	AT = MakeMatrix(N, M);
	transposeMatrix(A, AT);

	timer_start = tmr.elapsed();

	initZero(M, b);
	multiplyByColumns(0, N, AT, x, b);

	if (PRINTING_ENABLED)
	{
		cout << "Vektor b:" << endl;
		printVector(M, b);
	}

	timer_end = tmr.elapsed();
	std::cout << "->proteklo vrijeme: " << timer_end - timer_start << endl;
	std::cout << endl << "--------------------------------------------" << endl << endl;
#pragma endregion
	
#pragma region Paralelno mnozenje Ax=b po stupcima matrice A
	cout << "Paralelno mnozenje Ax=b po stupcima matrice A:" << endl;
	std::cout <<endl << "******************" << endl;

	for (unsigned int nit = 0; nit < broj_niti.size(); nit++)
	{
		if (broj_niti[nit] <= MAX_NUMBER_OF_THREADS)
		{
			thread threads[MAX_NUMBER_OF_THREADS];
			int result[MAX_NUMBER_OF_THREADS][M];
			int segment_size = N / broj_niti[nit];

			cout << "Broj niti: " << broj_niti[nit] << endl;

			timer_start = tmr.elapsed();

			initZero(M, b);

			unsigned int i = 0;
			for (; i < broj_niti[nit] - 1; i++)
			{
				initZero(M, result[i]);
				threads[i] = thread(multiplyByColumns, i *segment_size, i*segment_size + segment_size, AT, x, result[i]);
			}
			initZero(M, result[i]);
			threads[i] = thread(multiplyByColumns, i *segment_size, N, AT, x, result[i]);

			for (i = 0; i < broj_niti[nit]; i++)
			{
				threads[i].join();
			}

			for (int i = 0; i < broj_niti[nit]; i++)
			{
				addVectorToVector(M, b, result[i]);
			}

			if (PRINTING_ENABLED)
			{
				cout << "Vektor b:" << endl;
				printVector(M, b);
			}

			timer_end = tmr.elapsed();
			std::cout << "->proteklo vrijeme: " << timer_end - timer_start << endl;
			std::cout << "******************" << endl;
		}
	}
#pragma endregion
}