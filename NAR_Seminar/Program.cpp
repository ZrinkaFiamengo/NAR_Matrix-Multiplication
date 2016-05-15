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


vector<short> broj_niti = { 2, 4, 8, 16, M, N};
short **A, *x, *b;	//A*x=b
short **AT; //A transposed
double timer_start, timer_end;
Timer tmr;


void Sequential(string poruka, void(*fun)(short par1, short par2, short par3, short par4, short** par5, short* par6, short *par7), short par2, short** par5)
{
	if (!GRAPH_OUTPUT) cout << poruka << endl;
	timer_start = tmr.elapsed();

	initZero(M, b);
	(*fun)(0, par2, M, N, par5, x, b);

	if (PRINTING_ENABLED)
	{
		cout << "Vektor b:" << endl;
		printVector(M, b);
	}

	timer_end = tmr.elapsed();
	if (!GRAPH_OUTPUT) std::cout << "->proteklo vrijeme: " << timer_end - timer_start << endl;
	if (GRAPH_OUTPUT) std::cout << timer_end - timer_start << ", ";
	if (!GRAPH_OUTPUT) std::cout << endl << "--------------------------------------------" << endl << endl;
}


void Parallel(string poruka, void(*fun)(short par1, short par2, short par3, short par4, short** par5, short* par6, short *par7), short par22, short** par5, int par7)
{
	if (!GRAPH_OUTPUT) cout << poruka << endl;
	if (!GRAPH_OUTPUT) std::cout << endl << "******************" << endl;

	for (unsigned short nit = 0; nit < broj_niti.size(); nit++)
	{
		if (broj_niti[nit] <= MAX_NUMBER_OF_THREADS)
		{
			thread threads[MAX_NUMBER_OF_THREADS];
			short result[MAX_NUMBER_OF_THREADS][M];
			short segment_size = 0;

			if (par7 == 1)
			segment_size = M / broj_niti[nit];
			else if (par7 == 2)
			segment_size = N / broj_niti[nit];

			if (!GRAPH_OUTPUT) cout << "Broj niti: " << broj_niti[nit] << endl;

			timer_start = tmr.elapsed();

			initZero(M, b);

			unsigned short i = 0;
			for (i = 0; i < broj_niti[nit] - 1; i++)
			{
				initZero(M, result[i]);
				if(par7 == 1)
					threads[i] = thread((*fun), i *segment_size, i*segment_size + segment_size, M, N, par5, x, b);
				else if(par7 == 2)
					threads[i] = thread((*fun), i *segment_size, i*segment_size + segment_size, M, N, par5, x, result[i]);
			}
			initZero(M, result[i]);
			if (par7 == 1)
				threads[i] = thread((*fun), i *segment_size, par22, M, N, par5, x, b);
			else if (par7 == 2)
				threads[i] = thread((*fun), i *segment_size, par22, M, N, par5, x, result[i]);

			for (short i = 0; i < broj_niti[nit]; i++)
			{
				threads[i].join();
			}

			for (short i = 0; i < broj_niti[nit]; i++)
			{
				addVectorToVector(M, b, result[i]);
			}

			if (PRINTING_ENABLED)
			{
				cout << "Vektor b:" << endl;
				printVector(M, b);
			}

			timer_end = tmr.elapsed();
			if (!GRAPH_OUTPUT) std::cout << "->proteklo vrijeme: " << timer_end - timer_start << endl;
			if (GRAPH_OUTPUT) std::cout << timer_end - timer_start << ", ";
			if (!GRAPH_OUTPUT) std::cout << "******************" << endl;
		}
	}
	if (!GRAPH_OUTPUT) std::cout << endl << "--------------------------------------------" << endl << endl;
}


int main()
{
	A = MakeMatrix(M, N);
	x = MakeVector(N);
	b = MakeVector(M);
	AT = MakeMatrix(N, M);

	srand(time(NULL));
	if (GRAPH_OUTPUT) std::cout << std::fixed;

#pragma region Inicijalizacije podataka
	if (!GRAPH_OUTPUT) cout << "Inicijalizacija matrica:" << endl;
	timer_start = tmr.elapsed();

	initRandomNumbers(M, N, 10, A);
	initRandomNumbers(N, 10, x);

	if (PRINTING_ENABLED)
	{
		cout << "Matrica A:" << endl;
		printMatrix(M, N, A);
		cout << "Vektor x:" << endl;
		printVector(N, x);
	}

	timer_end = tmr.elapsed();
	if (!GRAPH_OUTPUT) std::cout << "->proteklo vrijeme: " << timer_end - timer_start << endl;
	if (GRAPH_OUTPUT) std::cout << timer_end - timer_start << ", ";
	if (!GRAPH_OUTPUT) std::cout << endl << "--------------------------------------------" << endl<<endl;
#pragma endregion

Sequential("Sekvencijalno mnozenje Ax=b po redcima matrice A:", &multiplyByRows, M, A);
Parallel("Paralelno mnozenje Ax=b po redcima matrice A:", multiplyByRows, M, A, 1);

Sequential("Sekvencijalno mnozenje Ax=b po stupcima matrice A:", multiplyByColumns, N, A);
Parallel("Paralelno mnozenje Ax=b po stupcima matrice A:", multiplyByColumns, N, A, 2);

Sequential("Sekvencijalno mnozenje Ax=b po redcima matrice A (podjeljene po stupcima):", multiplyByRowsDivideByCollumns, N, A);
Parallel("Paralelno mnozenje Ax=b po stupcima matrice A:", multiplyByRowsDivideByCollumns, N, A, 2);

transposeMatrix(M, N, A, AT);
Sequential("Sekvencijalno mnozenje Ax=b po stupcima matrice A(s transponiranjem):", multiplyByColumnsTransposed, N, AT);
Parallel("Paralelno mnozenje Ax=b po stupcima matrice A(s transponiranjem):", multiplyByColumnsTransposed, N, AT, 2);

if (GRAPH_OUTPUT) std::cout << "0 \n";
}