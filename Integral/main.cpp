// This code was greatfuly provided by MMiknich

#include<stdio.h>
#include<iostream>
#include<stdlib.h>
#include<chrono> 
#include<map>
#include<mpi.h>          // заголовочный файл mpi
using namespace std::chrono;

// Defining calculation function
double func (double x){
	if(x == 1.0)
	{
		//printf("Fault\n");
		return func(0.99999);
	}
	else{
		return 4.0/(1.0+(x*x));
	}
}

// Calculation of one segment via Trapezoidal rule
double trapeze(double le, double re, double (*f)(double)){
	if(le > re){
		return -1.0;
	}
	else{
		return (((*f)(le)+(*f)(re))/2.0)*(re - le);
	}
}

int main(int argc, char *argv[]){
	std::cout.setf(std::ios::fixed);
	std::cout.precision(10);
	//printf("HelloWorld\n");
	int myrank, p_num;
	MPI_Status Status;     // тип данных mpi
	if(argc < 5){
		std::cout << "Incorrect use of function";
		return -1;
	}
	double a = atof(argv[2]);
	double b = atof(argv[3]);
	bool OUT_TYPE = atoi(argv[4]);
	MPI_Init(&argc, &argv);// MPI программа начинается с MPI_Init; все остальные 'N' процессов начинают работу после этого   
	MPI_Comm_size(MPI_COMM_WORLD, &p_num); /* переменной size присваивается число, равное кол-ву процессов */ 
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank); /* каждому процессу присваивается уникальный номер */

	if(myrank == 0){
		int N = atoi(argv[1]);
		if(!OUT_TYPE) std::cout <<"N " <<N<<"\n";
		double quant = (b - a) / N;
		double result = 0;

		if(OUT_TYPE) printf("Quant size is %f\n",quant);
	
		if(quant == 0)  //TODO: Отправить ошибку в др процессы
			return -1;

		//-----Solving with single-------
		auto start =high_resolution_clock::now(); // timer start

		for(int i=0; i<N;i++)
			result += trapeze(quant*i, quant*(i+1), func);
		
		auto stop = high_resolution_clock::now(); // timer stop
		auto duration = duration_cast<microseconds>(stop - start); 
		if(OUT_TYPE) std::cout << "\nSingle core calculation gave result:"<< result <<" Calc_Time is:" << duration.count() << "\n\n";
		else std::cout << "SCR: " << result << " SCT: " << duration.count() << '\n'; 
		//---------------------------------------

		//-----Now let's start multicore calc-----------
		start = high_resolution_clock::now(); // timer start

		int calc_len = N / (p_num - 1);
		if(OUT_TYPE) std::cout <<"Calculation len of 1 proc" <<calc_len << "\n";

		// Sending Data |
		//              V
		for(int i = 1; i < p_num; i++)
		{
			// for i-process:
			// integr start point is -- (i-1) * calc_len
			// integr end point is -- i * calc_len
			double out_data[3];
			out_data[0] = quant*(i-1) * calc_len;
			out_data[1] = quant * i * calc_len;
			out_data[2] = double(calc_len);
			MPI_Send(out_data, 3, MPI_DOUBLE, i, 0, MPI_COMM_WORLD); // sending of a, b and calc_len           
		}

		//Self calc
		double a = quant*calc_len*(p_num - 1);
		double b = 1.0;
		calc_len = N - (calc_len * (p_num -1));
		result = 0;
		double k = (b - a)/calc_len;
		for(int i = 0; i< calc_len; i++){
			result += trapeze(a + i*k, a + (i + 1)*k, func);
		}
		if(OUT_TYPE) printf("My rank is %d, and integr from %lf to %lf on %d segments is %lf\n",myrank, a, b, calc_len, result);


		for(int i = 1; i < p_num; i++)
		{
			double temp_res = 0;
			MPI_Recv(&temp_res, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &Status);
			result += temp_res;
		}
		stop = high_resolution_clock::now(); // timer stop
		duration = duration_cast<microseconds>(stop - start); 
		if(OUT_TYPE) std::cout << "\nAll core calculation gave result:"<< result <<" Total Calc. Time is:" << duration.count() << "\n\n";	
		else std::cout << "MCQ: "<<	p_num << " MCR: " <<result <<" MCT: " << duration.count() <<"\n\n";

	}else{  
		//-----#############--------- Non ZERO process--------------################-----------------
	double a;
	double b;
	int calc_len;
	double input_data[3];
	//receving a , b and calc_len
	MPI_Recv(input_data, 3, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &Status); 

	a = input_data[0];
	b = input_data[1];
	calc_len = int(input_data[2]);


	//------calculating from a to b in calc_len segments--------------
	double result = 0;
	double k = (b - a)/calc_len;
	for(int i = 0; i< calc_len; i++)
		result += trapeze(a + i*k, a + (i + 1)*k, func);



	//-------Sending result--------------
	if(OUT_TYPE) printf("My rank is %d, and integr from %lf to %lf on %d segments is %lf\n",myrank, a, b, calc_len, result);
	MPI_Send(&result, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD); // sending result
	}
	
	MPI_Finalize();
	return 0;
}

