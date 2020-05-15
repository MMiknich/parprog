#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define OUTPUT_INFO 0
#define MAX_THREADS 32

#define C 1
#define h 0.0001
#define tau 0.0005
#define right_lim 10
#define left_lim -4

void *slave(void *args);
double g(double x);

struct slave_tag
{
    double *data_l;
    double *data_h;
    int p_num;
    double llim;
    double *pre_node_l;
    double *pre_node_h;
    double T;
};

pthread_barrier_t barrier;

int main(int argc, char *argv[])
{
    int slave_num = atoi(argv[1]);
    double T = atof(argv[2]);

    struct timespec begin, end; //setting timer
    clock_gettime(CLOCK_REALTIME, &begin);

    // slave information compilation
    int err = 0;
    pthread_t pthr[MAX_THREADS];
    struct slave_tag tags[MAX_THREADS];

    int p_num = (int)((right_lim - left_lim) / h) + 1;
    double *data_l = new double [p_num];
    double *data_h = new double [p_num];
    int p_arr[MAX_THREADS];

    for (int i = 0; i < slave_num; i++)
        p_arr[i] = p_num / slave_num;

    for (int i = 0; i < (p_num % slave_num); i++)
        p_arr[i]++;

    pthread_barrier_init(&barrier, NULL, slave_num);

    for (int i = 0; i < slave_num; i++)
    {
        tags[i].data_l = data_l + (i * p_arr[i]);
        tags[i].data_h = data_h + (i * p_arr[i]);
        tags[i].p_num = p_arr[i];
        tags[i].llim = left_lim + (h * i * p_arr[i]);
        tags[i].pre_node_l = i == 0 ? NULL : data_l + (i * p_arr[i]) - 1;
        tags[i].pre_node_h = i == 0 ? NULL : data_h + (i * p_arr[i]) - 1;
        tags[i].T = T;

        err = pthread_create(pthr + i, NULL, slave, (void *)(tags + i));
        if (err)
        {
            fprintf(stderr, "Slave thread creation failure\n");
            return -1;
        }
    }

    for (int i = 0; i < slave_num; i++)
    {
        err = pthread_join(pthr[i], NULL);
        if (err)
        {
            fprintf(stderr, "Slave thread joining failure\n");
            return -1;
        }
    }

    clock_gettime(CLOCK_REALTIME, &end);

    double time_total;
    time_total = end.tv_sec - begin.tv_sec;
    time_total += (end.tv_nsec - begin.tv_nsec) / 1000000000.0;

    if (OUTPUT_INFO)
    {
        printf("%d threads\n%d points\ntime is %f\n", slave_num, p_num, time_total);
        for (int i = 0; i < p_num; i++)
        {
            if (left_lim + (h * i) >= 0)
                printf("[%f]->%0.8f\n", left_lim + (h * i), data_l[i]);
                // printf("[%f]->%0.8f\n", left_lim + (h * i), data_l[i] - g(left_lim + (h * i) - T));
        }
    }
    else
        printf("%d; %f;\n", slave_num, time_total);
    delete[] data_h;
    delete[] data_l;
    return 0;
}

void *slave(void *args)
{
    struct slave_tag *my_tag;
    my_tag = (struct slave_tag *)args;

    double t = 0;
    // initialize
    for (int i = 0; i < my_tag->p_num; i++)
    {
        *(my_tag->data_h + i) = g(my_tag->llim + i * h);
        *(my_tag->data_l + i) = g(my_tag->llim + i * h);
    }
    pthread_barrier_wait(&barrier);

    while (t < my_tag->T)
    {
        int life_end = 0;
        if (my_tag->pre_node_h == NULL)
        {
            if (my_tag->p_num > 1)
                *(my_tag->data_h + 1) = (*(my_tag->data_l + 0) - *(my_tag->data_l + 1)) * (C * tau / h) + *(my_tag->data_l + 1);
            else
                life_end = 1; // make sipuku

            for (int i = 2; i < my_tag->p_num; i++)
                *(my_tag->data_h + i) = (*(my_tag->data_l + i - 1) - *(my_tag->data_l + i)) * (C * tau / h) + *(my_tag->data_l + i);

            my_tag->p_num--;
            double *temp = my_tag->data_h + 1;
            my_tag->data_h = my_tag->data_l + 1;
            my_tag->data_l = temp;
        }
        else
        {
            for (int i = 0; i < my_tag->p_num; i++)
                *(my_tag->data_h + i) = (*(my_tag->data_l + i - 1) - *(my_tag->data_l + i)) * (C * tau / h) + *(my_tag->data_l + i);
            double *temp = my_tag->data_h;
            my_tag->data_h = my_tag->data_l;
            my_tag->data_l = temp;
        }
        t += tau;
        pthread_barrier_wait(&barrier);
        if (life_end >= 1)
        {
            (my_tag + 1)->pre_node_h = NULL;
            (my_tag + 1)->pre_node_l = NULL;
        }
        pthread_barrier_wait(&barrier);
    }
    pthread_exit(NULL);
}

double g(double x)
{
    if (x < 0)
        return 0;
    else if (x <= 2)
        return x * (2 - x);
    else
        return 0;
}