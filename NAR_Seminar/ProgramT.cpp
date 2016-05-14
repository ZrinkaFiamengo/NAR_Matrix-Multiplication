#include <string>
#include <iostream>
#include <thread>
#include <math.h>
#include <chrono>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
typedef std::chrono::high_resolution_clock Clock;

#define defaultM 10000
#define defaultN 10000
#define defaultThreads 4

using namespace std;

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

void Initiate(int** Array, int* Vector, long* ResultVector, int M, int N){
	for(int i = 0; i < M; i++){
		for(int j = 0; j< N; j++){
			Array[i][j] = rand() % 99 + 1;
		}
		ResultVector[i] = 0;
	}
	for(int i = 0; i< N; i++){
		Vector[i] = rand() % 99 + 1;
	}
}

int** Make2DIntArray(int arraySizeX, int arraySizeY) {
	int** theArray;
	theArray = (int**) malloc(arraySizeX*sizeof(int*));
	for (int i = 0; i < arraySizeX; i++)
	   theArray[i] = (int*) malloc(arraySizeY*sizeof(int));
   return theArray;
} 

void MultiplyByVector(long* ResultVector, int** theArray, int* Vector, int M, int N, int length, int offset){
	int limit = 0 + offset + length;
	limit = (limit > M) ? M : limit;
	long temp = 0;
	for(int i = 0 + offset; i < limit; i++){
		for(int j = 0; j < N; j++){
			temp += theArray[i][j] * Vector[j];
		}
		ResultVector[i] = temp;
		temp = 0;
	}
}

void VerticalMultiplyByVector(long* ResultVector, int** theArray, int* Vector, int M, int N, int length, int offset){
	int limit = 0 + offset + length;
	limit = (limit > N) ? N : limit;
	long temp = 0;
	for(int i = 0; i < M; i++){
		for(int j = 0 + offset; j < limit; j++){
			temp += theArray[i][j] * Vector[j];
		}
		ResultVector[i] += temp;
		temp = 0;
	}
}

void DumbVerticalMultiplyByVector(long* ResultVector, int** theArray, int* Vector, int M, int N, int length, int offset){
	int limit = 0 + offset + length;
	limit = (limit > N) ? N : limit;
	long temp = 0;

	for(int j = 0 + offset; j < limit; j++){
		int quick = Vector[j];
		for(int i = 0; i< M; i++){
			temp += theArray[i][j] * quick;
			ResultVector[i] += temp;
			temp = 0;
		}
	}
}

int main(int argc, char* argv[])
{
	int M = (argc >= 2) ? atoi(argv[1]) : defaultM;
	int N = (argc >= 3) ? atoi(argv[2]) : defaultN;
	int Threads = (argc >= 4) ? atoi(argv[3]) : defaultThreads;
	Threads = (Threads >= 32) ? 32 : Threads;
	cout << "Zeljena matrica: [" << M << "][" << N << "]" << endl;
	cout << "Broj threadova: " << Threads << endl;
	
	double t1, t2, tt1, tt2;
	int length, offset;
	thread p[33];
	Timer tmr;
 	srand (time(NULL));

// Inicijalizacija array-a MxN i vektora N

	t1 = tmr.elapsed();
	int** Array = Make2DIntArray(M, N);
	int* Vector = (int*) malloc( N *sizeof(int));
	long* ResultVector = (long*) malloc( M *sizeof(long));
	Initiate(Array, Vector, ResultVector, M, N);
	t2 = tmr.elapsed();
	cout << "Inicijalizacija (vrijeme): " << (t2-t1) << endl;

// Thread incrementing and looping:
	for(int t = 1; t< Threads; t = t*2){
		cout << "========================================= \n" << "Broj niti: " << t << " \n=========================================" << endl;
		tt1 = tmr.elapsed();

// Mnozenje matrice s vektorom - sekvencijalno
		Initiate(Array, Vector, ResultVector, M, N);
		t1 = tmr.elapsed();
		MultiplyByVector(ResultVector, Array, Vector, M, N, M, 0);
		t2 = tmr.elapsed();
		cout << "Horizontalno - sekvencijalno (vrijeme): " << (t2-t1) << endl;
		cout << "Par rezultata mnozenja: 					" << ResultVector[1] << ", " << ResultVector[2] << endl;

// Mnozenje matrice s vektorom - paralelno

		Initiate(Array, Vector, ResultVector, M, N);
		length = M / t;
		t1 = tmr.elapsed();
		for( int z = 0; z < t; z++){
			offset = z * length;
			p[z] = std::thread(MultiplyByVector, ResultVector, Array, Vector, M, N, length, offset);
			p[z].join();
		}
		for( int z = 0; z < t; z++){
		}
		t2 = tmr.elapsed();
		cout << "Horizontalno - paralelno (vrijeme): " << (t2-t1) << endl;
		cout << "Par rezultata mnozenja: 					" << ResultVector[1] << ", " << ResultVector[2] << endl;

// Mnozenje matrice s vektorom - vertikalno - sekvencijalno

		Initiate(Array, Vector, ResultVector, M, N);
		t1 = tmr.elapsed();
		VerticalMultiplyByVector(ResultVector, Array, Vector, M, N, N, 0);
		t2 = tmr.elapsed();
		cout << "Vertikalno - sekvencijalno (vrijeme): 	" << (t2-t1) << endl;
		cout << "Par rezultata mnozenja: 					" << ResultVector[1] << ", " << ResultVector[2] << endl;

// Mnozenje matrice s vektorom - vertikalno - paralelno

		Initiate(Array, Vector, ResultVector, M, N);
		length = N / t;
		t1 = tmr.elapsed();
		for( int z = 0; z < t; z++){
			offset = z * length;
			p[z] = std::thread(VerticalMultiplyByVector, ResultVector, Array, Vector, M, N, length, offset);
		}
		for( int z = 0; z < t; z++){
			p[z].join();
		}
		t2 = tmr.elapsed();
		cout << "Vertikalno - paralelno (vrijeme): 	" << (t2-t1) << endl;
		cout << "Par rezultata mnozenja: 					" << ResultVector[1] << ", " << ResultVector[2] << endl;

// Mnozenje matrice s vektorom - vertikalno - sekvencijalno - glupo

		Initiate(Array, Vector, ResultVector, M, N);
		t1 = tmr.elapsed();
		DumbVerticalMultiplyByVector(ResultVector, Array, Vector, M, N, N, 0);
		t2 = tmr.elapsed();
		cout << "Ne-ef. vertikalno - sekv (vrijeme): " << (t2-t1) << endl;
		cout << "Par rezultata mnozenja: 					" << ResultVector[1] << ", " << ResultVector[2] << endl;

// Mnozenje matrice s vektorom - vertikalno - paralelno

		Initiate(Array, Vector, ResultVector, M, N);
		length = N / t;
		t1 = tmr.elapsed();
		for( int z = 0; z < t; z++){
			offset = z * length;
			p[z] = std::thread(DumbVerticalMultiplyByVector, ResultVector, Array, Vector, M, N, length, offset);
		}
		for( int z = 0; z < t; z++){
			p[z].join();
		}
		t2 = tmr.elapsed();
		cout << "Ne-ef. vertikalno - prll (vrijeme): " << (t2-t1) << endl;
		cout << "Par rezultata mnozenja: 					" << ResultVector[1] << ", " << ResultVector[2] << endl;

		tt2 = tmr.elapsed();
		cout << "========================================= \n" << "Za " << t << " niti potrebno je totalno " << tt2 - tt1 << " \n=========================================" << endl;
	}
}
