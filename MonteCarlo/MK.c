#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define FAIR_RESULT 3.14159265358979323846 * 3.14159265358979323846 / 8
#define FANSY_OUTPUT 1
#define SLAVE_MODE 1
#define LIMITS_X_LEFT 0.0
#define LIMITS_X_RIGHT 3.14159265358979323846
#define LIMITS_Y_LEFT 0.0
#define LIMITS_Y_RIGHT 1.0
#define MAX_THREADS 16
#define MAX_DEGREE 9

struct slave_tag
{
    double result;
    int point_num;
};

void *slave(void *args);
int is_in_space(double x, double y);

sem_t result_sem;

int correct_points = 0;

int main(int argc, char *argv[])
{
    // initializing
    int point_deg = 1;
    int slave_num = 1;
    slave_num = atoi(argv[1]);
    point_deg = atoi(argv[2]);

    if (point_deg > MAX_DEGREE || slave_num <= 0 || slave_num > MAX_THREADS)
    {
        fprintf(stderr, "Wrong input data\n");
        return -1;
    }

    struct timespec begin, end;
    clock_gettime(CLOCK_REALTIME, &begin);

    // slave information compilation
    int err = 0;
    pthread_t pthr[MAX_THREADS];
    struct slave_tag limits[MAX_THREADS];
    int points_per_slave = pow(10.0, (double)point_deg) / slave_num;

    sem_init(&result_sem, 0, 1);

    double left_lim = 0;
    for (int i = 0; i < slave_num; i++)
    {
        limits[i].result = 0;
        limits[i].point_num = points_per_slave;

        err = pthread_create(pthr + i, NULL, slave, (void *)(limits + i));
        if (err)
        {
            fprintf(stderr, "Slave thread creation failure\n");
            return -1;
        }
    }

    if (!SLAVE_MODE)
    {
        for (int i = 0; i < slave_num; i++)
        {
            err = pthread_join(pthr[i], NULL); // тупой c почему при create надо указатель
            if (err)                           // на объект pthread_r а при join сам объект??
            {
                fprintf(stderr, "Slave thread joining failure\n");
                return -1;
            }
        }
    }
    else
    {
        void *output;
        for (int i = 0; i < slave_num; i++)
        {
            err = pthread_join(pthr[i], &output);
            if (err)
            {
                fprintf(stderr, "Slave thread joining failure\n");
                return -1;
            }
            correct_points += *(int *)output;
        }
    }

    clock_gettime(CLOCK_REALTIME, &end);

    double time_total;
    time_total = end.tv_sec - begin.tv_sec;
    time_total += (end.tv_nsec - begin.tv_nsec) / 1000000000.0;

    sem_destroy(&result_sem);

    double sq = (LIMITS_X_RIGHT - LIMITS_X_LEFT) * (LIMITS_Y_RIGHT - LIMITS_Y_LEFT) * LIMITS_X_RIGHT;
    double result = sq * correct_points / pow(10.0, (double)point_deg);

    if (FANSY_OUTPUT)
        printf("%d threads\n%d points\ntime is %f\nnumber of correct points is %d\nresult is %f\nerror is %f\n",
               slave_num, (int)pow(10.0, (double)point_deg),  time_total, correct_points, result, result - FAIR_RESULT);
    else
    {
        printf("%f;%f;\n", time_total, result - FAIR_RESULT);
    }
}
int is_in_space(double x, double y)
{
    if (sin(x) < 0)
    {
        if (y <= 0 && y >= sin(x))
            return 1;
        else
        return 0;
    }
    else
    {
        if (y >= 0 && y <= sin(x))
            return 1;
        else
            return 0;
    }
}

void *slave(void *args)
{
    struct slave_tag *my_tag;
    my_tag = (struct slave_tag *)args;

    unsigned int seed = time(NULL);
    int sum = 0;
    int c_p = 0;

    double rand_kx = (LIMITS_X_RIGHT - LIMITS_X_LEFT) / RAND_MAX;
    double rand_ky = (LIMITS_Y_RIGHT - LIMITS_Y_LEFT) / RAND_MAX;
    double rand_kz = LIMITS_X_RIGHT / RAND_MAX;  //колзоз, работает только при этой задаче(где max f = xy = pi)
    for (int i = 0; i < my_tag->point_num; i++)
    {
        double x = LIMITS_X_LEFT + ((double)rand_r(&seed) * rand_kx);
        double y = LIMITS_Y_LEFT + ((double)rand_r(&seed) * rand_ky);
        double z = (double)rand_r(&seed) * rand_kz;
        if (is_in_space(x, y) && z <= x*y)
            c_p++;
    }

    if (SLAVE_MODE)
    {
        my_tag->point_num = c_p;
        pthread_exit((void *)&(my_tag->point_num));
    }
    else
    {
        sem_wait(&result_sem);
        correct_points += c_p;
        sem_post(&result_sem);
        pthread_exit(NULL);
    }
}