// This code was greatfuly created by (c) MMiknich
#define SEND_WAY 1
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

double fair_solution(double U_0, double k, double x, double t, double epsilon = 0.0001)
{
	double result = 0;
	double iter = 0;
	double iter_sum = 1000;
	while (iter_sum >= epsilon)
	{
		int itt = (2 * iter + 1);
		iter_sum = exp(
					   -1 * k * M_PI * M_PI * itt * itt * t) /
				   itt * sin(M_PI * itt * x);
		result += iter_sum;
		iter++;
	}
	result *= 4 * U_0 / M_PI;
	return result;
}

double iter_method(double old_i, double old_ip, double old_in, double k, double h, double dt)
{
	return (old_i + ((k * dt / (h * h)) * (old_in + old_ip - 2 * old_i)));
}

int main(int argc, char *argv[])
{
	int myrank, p_num;
	MPI_Status Status;
	MPI_Request Request;

	if (argc < 8)
	{
		printf("Incorrect use of function");
		return -1;
	}
	double U_0 = atof(argv[1]);
	double U_e = atof(argv[2]);
	double k = atof(argv[3]);
	double h = atof(argv[4]);
	double dt = atof(argv[5]);
	double T = atof(argv[6]);
	int OUT_TYPE = atoi(argv[7]);

	if (OUT_TYPE == 2)
	{
		dt = 0.449 * h * h / k;
	}

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &p_num);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	if (myrank == 0)
	{
		if (OUT_TYPE == 1)
		{
			printf("--Theoretical Solution--\n");
			double time = MPI_Wtime();
			double x = 0;
			while (x + h < 1)
			{
				printf("%f->%f\n", x, fair_solution(U_0, k, x, T));
				x += h;
			}
			printf("%f->%f\n", 1.0, fair_solution(U_0, k, 1, T));
			time = MPI_Wtime() - time;
			printf("Single proc. time: %f\n", time);
			//printf("Streight result is: %f", fair_solution(U_o, k,))
		}

		// Multiprocs calc
		if (OUT_TYPE == 1)
			printf("--Algorithmical Solution--\n");
		double time = MPI_Wtime(); // setting timer

		int N, N_i = 0;
		double result = 0;
		int N_k[50] = {0};

		N_i = int(1.0 / h);
		double h_last = 0; // len of last section

		if (1.0 - N_i * h == 0)
		{
			N = N_i + 1;
			h_last = h;
		}
		else
		{
			N = N_i + 2;
			h_last = 1.0 - N_i * h;
		}

		for (int i = 0; i < p_num; i++)
			N_k[i] = N / p_num;
		for (int i = 0; i < N % p_num; i++)
			N_k[i]++;

		if (OUT_TYPE == 1)
		{
			int sum = 0;
			for (int i = 0; i < p_num; i++)
			{
				sum += N_k[i];
				printf("%d->%d ", i, N_k[i]);
			}
			printf("\nTotal num of points %d\n", sum);
		}

		// Sending       | Counting interval
		//              V
		for (int i = 1; i < p_num; i++)
		{
			MPI_Send(N_k + i, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
		}

		// Setting 2 data frames
		int size = N_k[0];
		double *data = (double *)malloc((size + 2) * sizeof(double));
		double *new_data = (double *)malloc((size + 2) * sizeof(double));
		int pre_proc = p_num - 1;
		double T_i = 0;

		//Initilising t=0 data
		for (int i = 0; i < size + 2; i++)
			data[i] = U_0;

		while (T_i < T)
		{
			if (pre_proc == 0)
				data[0] = U_e;
			else
			{
				MPI_Recv(data, 1, MPI_DOUBLE, pre_proc, 0, MPI_COMM_WORLD, &Status); // left edge send-receve
				MPI_Send(data + 1, 1, MPI_DOUBLE, pre_proc, 0, MPI_COMM_WORLD);
			}

			data[size + 1] = U_e; //right edge "edge condition"

			for (int i = 1; i < size; i++)
				new_data[i] = iter_method(data[i], data[i - 1], data[i + 1], k, h, dt);
			new_data[size] = iter_method(data[size], data[size - 1], data[size + 1], k, h_last, dt);
			double *tmp = new_data;
			new_data = data;
			data = tmp;
			T_i += dt;
			//and again
		}

		double *ans = (double *)malloc(N * sizeof(double));

		int pointer = 0;
		for (int i = 1; i < p_num; i++)
		{
			double temp_res = 0;
			MPI_Recv(ans + pointer, N_k[i], MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &Status);
			pointer += N_k[i];
			result += temp_res;
		}
		for (int i = 1; i <= size; i++)
			ans[N - size + (i - 1)] = data[i];
		if (OUT_TYPE == 1)
		{
			for (int i = 0; i < N; i++)
				printf("%d->%f\n", i, ans[i]);
		}
		free(data);
		free(new_data);
		free(ans);

		time = MPI_Wtime() - time;
		if (OUT_TYPE == 1)
			printf("Total Calc. Time is: %f\n\n", time);
		else
			printf("%d %f\n", p_num, time);
	}

	else
	{
		int size = 0;
		MPI_Recv(&size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &Status);

		/* 
			1) We send our right edge
			2) We get our left edge
			3) Then we cont
			4) 1->2, 2->1
		*/

		// Setting 2 data frames
		double *data = (double *)malloc((size + 2) * sizeof(double));
		double *new_data = (double *)malloc((size + 2) * sizeof(double));

		int pre_proc = (myrank - 1) % p_num;
		int next_proc = (myrank + 1) % p_num;

		//Initilising T_i=0 data
		double T_i = 0;
		for (int i = 0; i < size + 2; i++)
			data[i] = U_0;

		while (T_i < T)
		{
			if (pre_proc == 0)
			{
				data[0] = U_e;
				MPI_Send(data + size, 1, MPI_DOUBLE, next_proc, 0, MPI_COMM_WORLD);
				MPI_Recv(data + size + 1, 1, MPI_DOUBLE, next_proc, 0, MPI_COMM_WORLD, &Status);
			}
			else
			{
				if (SEND_WAY) // O(1) senting algorithm
				{
					if (myrank % 2 == 0)
					{
						MPI_Recv(data, 1, MPI_DOUBLE, pre_proc, 0, MPI_COMM_WORLD, &Status);
						MPI_Send(data + 1, 1, MPI_DOUBLE, pre_proc, 0, MPI_COMM_WORLD);

						MPI_Send(data + size, 1, MPI_DOUBLE, next_proc, 0, MPI_COMM_WORLD);
						MPI_Recv(data + size + 1, 1, MPI_DOUBLE, next_proc, 0, MPI_COMM_WORLD, &Status);
					}
					else
					{
						MPI_Send(data + size, 1, MPI_DOUBLE, next_proc, 0, MPI_COMM_WORLD);
						MPI_Recv(data + size + 1, 1, MPI_DOUBLE, next_proc, 0, MPI_COMM_WORLD, &Status);

						MPI_Recv(data, 1, MPI_DOUBLE, pre_proc, 0, MPI_COMM_WORLD, &Status);
						MPI_Send(data + 1, 1, MPI_DOUBLE, pre_proc, 0, MPI_COMM_WORLD);
					}
				}
				else // O(p) sending algorithm
				{
					MPI_Recv(data, 1, MPI_DOUBLE, pre_proc, 0, MPI_COMM_WORLD, &Status);
					MPI_Send(data + size, 1, MPI_DOUBLE, next_proc, 0, MPI_COMM_WORLD);
					MPI_Recv(data + size + 1, 1, MPI_DOUBLE, next_proc, 0, MPI_COMM_WORLD, &Status);
					MPI_Send(data + 1, 1, MPI_DOUBLE, pre_proc, 0, MPI_COMM_WORLD);
				}
			}

			for (int i = 1; i <= size; i++)
				new_data[i] = iter_method(data[i], data[i - 1], data[i + 1], k, h, dt);

			double *tmp = new_data;
			new_data = data;
			data = tmp;
			T_i += dt;
			//and again
		}

		MPI_Send(data + 1, size, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD); // sending result

		free(data);
		free(new_data);
	}

	MPI_Finalize();
	return 0;
}