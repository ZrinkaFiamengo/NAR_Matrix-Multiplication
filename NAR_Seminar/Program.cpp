#include <string>
#include <iostream>
#include <thread>
#include <math.h>
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
		return std::chrono::duration_cast<std::chrono::microseconds>
			(clock_::now() - beg_).count();
	}
private:
	typedef std::chrono::high_resolution_clock clock_;
	typedef std::chrono::duration<double, std::ratio<1> > second_;
	std::chrono::time_point<clock_> beg_;
};


short main()
{
	vector<short> broj_niti = { 2, 4,8, M, N};
	short **A, *x, *b;	//A*x=b
	short **AT; //A transposed
	double timer_start, timer_end;
	Timer tmr;

	A = MakeMatrix(M, N);
	x = MakeVector(N);
	b = MakeVector(M);
	AT = MakeMatrix(N, M);

	srand(time(NULL));

#pragma region Inicijalizacije podataka
	cout << "Inicijalizacija matrica:" << endl;
	timer_start = tmr.elapsed();

	initRandomNumbers(M, N, 10, A);
	initRandomNumbers(N, 10, x);

	if (printING_ENABLED)
	{
		cout << "Matrica A:" << endl;
		printMatrix(M, N, A);
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

	initZero(M, b);
	multiplyByRows(0, M, N, A, x, b);

	if (printING_ENABLED)
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

	for (unsigned short nit = 0; nit < broj_niti.size(); nit++)
	{
		if (broj_niti[nit] <= MAX_NUMBER_OF_THREADS)
		{
			thread threads[MAX_NUMBER_OF_THREADS];
			short segment_size = M / broj_niti[nit];
			
			cout << "Broj niti: " << broj_niti[nit] << endl;

			timer_start = tmr.elapsed();

			initZero(M, b);

			unsigned short i = 0;
			for (i=0; i < broj_niti[nit] - 1; i++)
			{
				threads[i] = thread(multiplyByRows, i *segment_size, i*segment_size + segment_size, N, A, x, b);
			}
			threads[i] = thread(multiplyByRows, i *segment_size, M, N, A, x, b);

			for (short i = 0; i < broj_niti[nit]; i++)
			{
				threads[i].join();
			}

			if (printING_ENABLED)
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

	timer_start = tmr.elapsed();

	initZero(M, b);
	multiplyByColumns(0, N, M, A, x, b);

	if (printING_ENABLED)
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
	std::cout << endl << "******************" << endl;

	for (unsigned short nit = 0; nit < broj_niti.size(); nit++)
	{
		if (broj_niti[nit] <= MAX_NUMBER_OF_THREADS)
		{
			thread threads[MAX_NUMBER_OF_THREADS];
			short result[MAX_NUMBER_OF_THREADS][M];
			short segment_size = N / broj_niti[nit];

			cout << "Broj niti: " << broj_niti[nit] << endl;

			timer_start = tmr.elapsed();

			initZero(M, b);

			unsigned short i = 0;
			for (; i < broj_niti[nit] - 1; i++)
			{
				initZero(M, result[i]);
				threads[i] = thread(multiplyByColumns, i *segment_size, i*segment_size + segment_size, M, A, x, result[i]);
			}
			initZero(M, result[i]);
			threads[i] = thread(multiplyByColumns, i *segment_size, N, M, A, x, result[i]);

			for (i = 0; i < broj_niti[nit]; i++)
			{
				threads[i].join();
			}

			for (short i = 0; i < broj_niti[nit]; i++)
			{
				addVectorToVector(M, b, result[i]);
			}

			if (printING_ENABLED)
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

#pragma region Sekvencijalno mnozenje Ax=b po redcima matrice A (podjeljene po stupcima)
	cout << "Sekvencijalno mnozenje Ax=b po redcima matrice A (podjeljene po stupcima):" << endl;

	timer_start = tmr.elapsed();

	initZero(M, b);
	multiplyByRowsDivideByCollumns(0, N, M, A, x, b);

	if (printING_ENABLED)
	{
		cout << "Vektor b:" << endl;
		printVector(M, b);
	}

	timer_end = tmr.elapsed();
	std::cout << "->proteklo vrijeme: " << timer_end - timer_start << endl;
	std::cout << endl << "--------------------------------------------" << endl << endl;
#pragma endregion

#pragma region Paralelno mnozenje Ax=b po redcima matrice A (podjeljene po stupcima)
	cout << "Paralelno mnozenje Ax=b po redcima matrice A (podjeljene po stupcima):" << endl;
	std::cout << endl << "******************" << endl;

	for (unsigned short nit = 0; nit < broj_niti.size(); nit++)
	{
		if (broj_niti[nit] <= MAX_NUMBER_OF_THREADS)
		{
			thread threads[MAX_NUMBER_OF_THREADS];
			short result[MAX_NUMBER_OF_THREADS][M];
			short segment_size = N / broj_niti[nit];

			cout << "Broj niti: " << broj_niti[nit] << endl;

			timer_start = tmr.elapsed();

			initZero(M, b);

			unsigned short i = 0;
			for (; i < broj_niti[nit] - 1; i++)
			{
				initZero(M, result[i]);
				threads[i] = thread(multiplyByRowsDivideByCollumns, i *segment_size, i*segment_size + segment_size, M, A, x, result[i]);
			}
			initZero(M, result[i]);
			threads[i] = thread(multiplyByRowsDivideByCollumns, i *segment_size, N, M, A, x, result[i]);

			for (i = 0; i < broj_niti[nit]; i++)
			{
				threads[i].join();
			}

			for (short i = 0; i < broj_niti[nit]; i++)
			{
				addVectorToVector(M, b, result[i]);
			}

			if (printING_ENABLED)
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

#pragma region Sekvencijalno mnozenje Ax=b po stupcima matrice A (s transponiranjem)
	cout << "Sekvencijalno mnozenje Ax=b po stupcima matrice A(s transponiranjem):" << endl;
	transposeMatrix(M, N, A, AT);

	timer_start = tmr.elapsed();

	initZero(M, b);
	multiplyByColumnsTransposed(0, N, M, AT, x, b);

	if (printING_ENABLED)
	{
		cout << "Vektor b:" << endl;
		printVector(M, b);
	}

	timer_end = tmr.elapsed();
	std::cout << "->proteklo vrijeme: " << timer_end - timer_start << endl;
	std::cout << endl << "--------------------------------------------" << endl << endl;
#pragma endregion

#pragma region Paralelno mnozenje Ax=b po stupcima matrice A (s transponiranjem)
	cout << "Paralelno mnozenje Ax=b po stupcima matrice A (s transponiranjem):" << endl;
	std::cout <<endl << "******************" << endl;

	for (unsigned short nit = 0; nit < broj_niti.size(); nit++)
	{
		if (broj_niti[nit] <= MAX_NUMBER_OF_THREADS)
		{
			thread threads[MAX_NUMBER_OF_THREADS];
			short result[MAX_NUMBER_OF_THREADS][M];
			short segment_size = N / broj_niti[nit];

			cout << "Broj niti: " << broj_niti[nit] << endl;

			timer_start = tmr.elapsed();

			initZero(M, b);

			unsigned short i = 0;
			for (; i < broj_niti[nit] - 1; i++)
			{
				initZero(M, result[i]);
				threads[i] = thread(multiplyByColumnsTransposed, i *segment_size, i*segment_size + segment_size, M, AT, x, result[i]);
			}
			initZero(M, result[i]);
			threads[i] = thread(multiplyByColumnsTransposed, i *segment_size, N, M, AT, x, result[i]);

			for (i = 0; i < broj_niti[nit]; i++)
			{
				threads[i].join();
			}

			for (short i = 0; i < broj_niti[nit]; i++)
			{
				addVectorToVector(M, b, result[i]);
			}

			if (printING_ENABLED)
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