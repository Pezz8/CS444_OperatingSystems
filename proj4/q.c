#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/time.h>
#include <math.h>
#include <unistd.h>
#include <stdbool.h>

// GLOBAL VARIABLES
double lambda = 5.0;
double mu = 7.0;
int numberCustomers = 1000;
int numberServer = 1;

int costumerServed = 0;
bool serverDone = false;
pthread_mutex_t counter_mutex;
pthread_mutex_t server_done_mutex;
pthread_mutex_t queue_mutex;
pthread_cond_t server_done_cond;
pthread_cond_t queue_cond;

typedef struct customer customer;
struct customer
{
    struct timeval arrivalTime;
    customer *next;
};

customer *qHead = NULL;
customer *qTail = NULL;
unsigned qLength = 0;

// Rand function using drand48 not working
// double rndExp(struct drand48_data *randData, double lambda)
// {
//     double tmp;

//     drand48_r(randData, &tmp);
//     return -log(1.0 - tmp) / lambda;
// }

// Alternative to using rand_r insted of drand48
double thread_safe_rand(unsigned int *seed)
{
    return (double)rand_r(seed) / (double)RAND_MAX;
}

double rand_exp(double lambda, unsigned int *seed)
{
    double u = thread_safe_rand(seed);
    return -log(1 - u) / lambda;
}

// Generating the customers
void *thread1(void *p)
{
    unsigned int seed = time(NULL);

    while (1)
    {
        // thread1 listening to see if server is done
        pthread_mutex_lock(&server_done_mutex);
        if (serverDone)
        {
            pthread_mutex_unlock(&server_done_mutex);
            break;
        }
        pthread_mutex_unlock(&server_done_mutex);
        double interArrivalTime = rand_exp(lambda, &seed);
        // Sleep for that much time
        sleep((unsigned int)interArrivalTime);

        customer *newCustomer = (customer *)malloc(sizeof(customer));
        if (newCustomer == NULL)
        {
            perror("Failed to allocate memory");
            exit(EXIT_FAILURE);
        }
        // gettimeofday() timestamp the arrival time
        gettimeofday(&newCustomer->arrivalTime, NULL);
        newCustomer->next = NULL;

        // LOCK MUTEX
        pthread_mutex_lock(&queue_mutex);
        // Consider two cases
        // Insert into a nonempty queue
        if (qTail)
        {
            qTail->next = newCustomer;
            qTail = newCustomer;
        }
        // Insert into a empty queue -- signal thread2
        else
        {
            qHead = qTail = newCustomer;
            pthread_cond_signal(&queue_cond);
        }
        qLength++;
        // Unlock qMutex
        pthread_mutex_unlock(&queue_mutex);
    }
    return NULL;
}

// Serving the customers
void *thread2(void *p)
{
    customer *aCustomer;
    struct timeval tv;
    double service_time;
    double waiting_time;
    unsigned int seed = time(NULL) + 1;
    // struct drand48_data *rand_buffer;
    // srand48_r(time(NULL) + 1, &rand_buffer);

    // Loop
    while (1)
    {
        // pthread_mutex_lock(&server_done_mutex);
        // if (serverDone)
        // {
        //     pthread_mutex_unlock(&server_done_mutex);
        //     break;
        // }
        // pthread_mutex_unlock(&server_done_mutex);

        // lock mutex
        pthread_mutex_lock(&queue_mutex);
        while (qHead == NULL)
        {
            pthread_cond_wait(&queue_cond, &queue_mutex);
        }

        aCustomer = qHead;
        qHead = qHead->next;
        if (qHead == NULL)
        {
            qTail = NULL;
        }
        qLength--;
        pthread_mutex_unlock(&queue_mutex);

        gettimeofday(&tv, NULL);
        waiting_time = tv.tv_sec - aCustomer->arrivalTime.tv_sec + (tv.tv_usec - aCustomer->arrivalTime.tv_usec) / 1000000.0;

        // service_time = rndExp(&rand_buffer, mu);
        service_time = rand_exp(mu, &seed);
        sleep((unsigned int)service_time);
        free(aCustomer);

        // Increase the number of custumer served
        pthread_mutex_lock(&counter_mutex);
        costumerServed++;
        // When we serve the total number of customers end the program
        if (costumerServed >= numberCustomers)
        {
            printf("Costumer served: %d\n", costumerServed);
            pthread_mutex_lock(&server_done_mutex);
            serverDone = true;
            pthread_cond_broadcast(&server_done_cond);
            pthread_mutex_unlock(&server_done_mutex);
            break;
        }
        pthread_mutex_unlock(&counter_mutex);
    }

    return NULL;
}

// Observing the queue
void *thread3(void *p)
{
    while (1)
    {
        // thread3 listening to see if server is done
        pthread_mutex_lock(&server_done_mutex);
        if (serverDone)
        {
            pthread_mutex_unlock(&server_done_mutex);
            break;
        }
        pthread_mutex_unlock(&server_done_mutex);
        // sleeping for 0.005 seconds
        usleep(5000);

        pthread_mutex_lock(&queue_mutex);
        unsigned current_queue_lenght = qLength;
        pthread_mutex_unlock(&queue_mutex);

        printf("Current queue lenght: %u\n", current_queue_lenght);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    // Using getopt.h to grab the values and flags used in the command line
    int opt;
    while ((opt = getopt(argc, argv, "l:m:c:s:")) != -1)
    {
        switch (opt)
        {
        case 'l':
            lambda = atof(optarg);
            break;
        case 'm':
            mu = atof(optarg);
            break;
        case 'c':
            numberCustomers = atoi(optarg);
            break;
        case 's':
            numberServer = atoi(optarg);
            printf("Number of servers %d\n", numberServer);
            if (numberServer < 1 || numberServer > 5)
            {
                fprintf(stderr, "Number of servers must be between 1 and 5.\n");
                exit(EXIT_FAILURE);
            }
            break;
        default:
            fprintf(stderr, "Usage: %s [-l lambda] [-m mu] [-c numberCustomers] [-s numberServer]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    // Checking if queueing system is unstable
    if (lambda >= mu * numberServer)
    {
        fprintf(stderr, "Unstable system: lambda must be less than mu times number of servers\n");
        exit(EXIT_FAILURE);
    }

    // Inizializing mutex and condition variable
    pthread_mutex_init(&queue_mutex, NULL);
    pthread_mutex_init(&counter_mutex, NULL);
    pthread_mutex_init(&server_done_mutex, NULL);
    pthread_cond_init(&queue_cond, NULL);
    pthread_cond_init(&server_done_cond, NULL);

    // Creating threads
    pthread_t thread_1, thread_2, thread_3;
    pthread_create(&thread_1, NULL, thread1, NULL);
    pthread_create(&thread_2, NULL, thread2, NULL);
    pthread_create(&thread_3, NULL, thread3, NULL);

    // Join threads
    pthread_join(thread_1, NULL);
    pthread_join(thread_2, NULL);
    pthread_join(thread_3, NULL);

    // Cleaning up mutex and condition variable
    pthread_mutex_destroy(&queue_mutex);
    pthread_mutex_destroy(&counter_mutex);
    pthread_mutex_destroy(&server_done_mutex);
    pthread_cond_destroy(&server_done_cond);
    pthread_cond_destroy(&queue_cond);

    return 0;
}
