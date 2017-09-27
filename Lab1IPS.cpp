#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <cilk/reducer_max.h>
#include <cilk/reducer_min.h>
#include <cilk/reducer_vector.h>
#include <chrono>
#include <iostream>
using namespace std::chrono;
using namespace std;
/// ‘ункци€ ReducerMaxTest() определ€ет максимальный элемент массива,
/// переданного ей в качестве аргумента, и его позицию
/// mass_pointer - указатель исходный массив целых чисел
/// size - количество элементов в массиве
void ReducerMaxTest(int *mass_pointer, const long size)
{
	cilk::reducer<cilk::op_max_index<long, int>> maximum;
cilk_for(long i = 0; i < size; ++i)
	{
		maximum->calc_max(i, mass_pointer[i]);
	}
	printf("Maximal element = %d has index = %d\n\n",
		maximum->get_reference(), maximum->get_index_reference());
}

void ReducerMinTest(int *mass_pointer, const long size)
{
	cilk::reducer<cilk::op_min_index<long, int>> minimum;
	cilk_for(long i = 0; i < size; ++i)
	{
		minimum->calc_min(i, mass_pointer[i]);
	}
	printf("Minimal element = %d has index = %d\n\n",
		minimum->get_reference(), minimum->get_index_reference());
}

/// ‘ункци€ ParallelSort() сортирует массив в пор€дке возрастани€
/// begin - указатель на первый элемент исходного массива
/// end - указатель на последний элемент исходного массива

void ParallelSort(int *begin, int *end)
{

	if (begin != end) 
	{
		--end;
		int *middle = std::partition(begin, end, std::bind2nd(std::less<int>(), *end));
		std::swap(*end, *middle); 
		cilk_spawn ParallelSort(begin, middle);
		ParallelSort(++middle, ++end);
	    cilk_sync;
	}

	

}
void ParallelSortTime(int *begin, int *end)
{
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	ParallelSort(begin, end);
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	duration<double> duration = (t2 - t1);
	cout << "ParallelSort \n";
	cout << "Duration is: " << duration.count() << " seconds" << endl;
	cout << "\n";
}

void CompareForAndCilk_For(size_t sz)
{
	cout << "size of vectors " << sz << "\n";
	std::vector<int> v;

	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	for (int is = 0; is < sz; is++)
	{
		v.push_back(rand() % 20000 + 1);
	}

	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	duration<double> duration = (t2 - t1);
	cout << "Time of for is: " << duration.count() << " seconds" << endl;

	cilk::reducer<cilk::op_vector<int>>red_vec;

	high_resolution_clock::time_point t3 = high_resolution_clock::now();
	cilk_for(int is = 0; is < sz; is++)
	{
		red_vec->push_back(rand() % 20000 + 1);
	}

	high_resolution_clock::time_point t4 = high_resolution_clock::now();
	duration = (t4 - t3);
	cout << "Time of cilk_for is: " << duration.count() << " seconds" << endl;
	cout << "\n";
	
}

int main()
{
	srand((unsigned)time(0));

	// устанавливаем количество работающих потоков = 4
	__cilkrts_set_param("nworkers", "4");

	long i;
	const long mass_size = 10;
	int *mass_begin, *mass_end;
	int *mass = new int[mass_size]; 

	for(i = 0; i < mass_size; ++i)
	{
		mass[i] = (rand() % 25000) + 1;
		printf("%d    ", mass[i]);
	}
	cout << "\n";
	cout << "mass_size " << mass_size << "\n\n";
	mass_begin = mass;
	mass_end = mass_begin + mass_size;
	ReducerMaxTest(mass, mass_size);
	ReducerMinTest(mass, mass_size);
	ParallelSortTime(mass_begin, mass_end);
	

	ReducerMaxTest(mass, mass_size);
	ReducerMinTest(mass, mass_size);
	//CompareForAndCilk_For(1000000);
	//CompareForAndCilk_For(100000);
	//CompareForAndCilk_For(10000);
	//CompareForAndCilk_For(1000);
	//CompareForAndCilk_For(500);
	//CompareForAndCilk_For(100);
	//CompareForAndCilk_For(50);
	//CompareForAndCilk_For(10);
	delete[]mass;
	return 0;
}