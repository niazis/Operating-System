/*
Name: Muhammad Niazi
Project 2

Compile: gcc Niazi_23812328.c -o Niazi_23812328.exe -lpthread -lm
Execute: ./Niazi_23812328 [input file] [output file]
*/


#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>

// Global Variables for the program
typedef struct comp_t_args {
    int start;
    int end;
    float avg;
    float sum;
    float sqrtSum;
} comp_t_args;


// Number of entries in the input file
#define COUNT 10000

// Number of worker threads
#define THREAD_COUNT 10

// Number of operations per thread
#define OP (COUNT/THREAD_COUNT)

// Array holding all numbers
float arr[COUNT];

// Array holding the results of computations
comp_t_args computations[THREAD_COUNT];

// File descriptor
int fd;

// Mutex for threads
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;


// Calculae the average
void* avgThread(void* _args) {
    // Get args
    comp_t_args* args = (comp_t_args*)_args;
    // The sum of the sub array
    float sum = 0;
    // Calculate the sum
    for(int i = args->start; i < args->end; i++) {
        sum += arr[i];
    }
    // Get average by dividing the sum by the number of operations by thread
    args->avg = sum/OP;
}

// Calculate the sum
void* sumThread(void* _args) {
    comp_t_args* args = (comp_t_args*)_args;
    // THe sum of the sub array
    float sum = 0;
    // Calcualte the sum
    for(int i = args->start; i < args->end; i++) {
        sum += arr[i];
    } 
    args->sum = sum;
}

// Calculate the square root of the sum of squares
void* sqrtThread(void* _args) {
    comp_t_args* args = (comp_t_args*)_args;
    int sum = 0;
    for(int i = args->start; i < args->end; i++) {
        // Add the sum of the current number square
        sum += arr[i] *  arr[i];
    } 
    // Get the square root
    args->sqrtSum = pow(sum, 0.5f);
}

void* workerThread(void* args)
{
    // Get the id of the current thread
    long id = (long)args;
    // Allocate memory for the arguments
    comp_t_args* compArgs = malloc(sizeof(comp_t_args));
    // Set the position to the first element in the array
    compArgs->start = id * OP;
    // Set the position of the last element in the in array
    compArgs->end = compArgs->start + OP;
    // If This is the only thread, set the position of the last element to the last element in the array
    if (id == (THREAD_COUNT - 1))
    {
        compArgs->end = COUNT;
    }

    pthread_t threads[3];
    // Create three computation threads
    pthread_create(&threads[0], NULL, avgThread, compArgs);
    pthread_create(&threads[1], NULL, sumThread, compArgs);
    pthread_create(&threads[2], NULL, sqrtThread, compArgs);
    // Wait for threads to finish executing
    for(int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }
    // Lock mutex
    pthread_mutex_lock(&mutex);
    char buff[1024];
    // Write out thread output
    int count = sprintf(buff, "Worker Child Pthread Number = %ld\t", id);
    write(fd, buff, count);
    //print a tab after each number
    write(fd, "\t", 1);
    
    count = sprintf(buff, "Average = %f\t", compArgs->avg);
    write(fd, buff, count);
    
    count = sprintf(buff, "Sum = %f\t", compArgs->sum);
    write(fd, buff, count);
    write(fd, "\t", 1);

    count = sprintf(buff, "Root of the Sum of Squares = %f\n", compArgs->sqrtSum);
    write(fd, buff, count);
    
    computations[id] = *compArgs;
    // Release mutex
    pthread_mutex_unlock(&mutex);
}



int main(int argc, char** argv) {
    if(argc < 3) {
        printf("Please provide input and output files\n");
        return 1;
    }
    // Get input filename
    char* filename = argv[1];
    // Get output filename
    char* outfile = argv[2];
    // Open output file for writing 
    fd = open(outfile, O_CREAT | O_WRONLY);
    // Open input file for reading
    FILE* f = fopen(filename, "r");

    // Check if input file opened correctly
    if(f == NULL) {
        printf("Error opening input file\n"); 
        return 1;  
    }
    // Read input file
    for(int i = 0; i < COUNT; i++) {
        fscanf(f, "%f", &arr[i]);
    }
    pthread_t threads[THREAD_COUNT];

    // Create worker threads
    for(long i = 0; i < THREAD_COUNT; i++) {
        pthread_create(&threads[i], NULL, workerThread, (void*)(i));
    }

    float minAvg = INT_MAX;
    float minSum = INT_MAX;
    float minSqrt = INT_MAX;

    for(int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
        // Find min of average
        if(computations[i].avg < minAvg)
        {
            minAvg = computations[i].avg;
        }
        // Find min of sum
        if(computations[i].sum < minSum)
        {
            minSum = computations[i].sum;
        }
        // Find min of sqrt of squared suums
        if(computations[i].sqrtSum < minSqrt)
        {
            minSqrt = computations[i].sqrtSum;
        }
    }
    
    // Write to file
    char buff[1024];  

    // Write out min output to a file and console
    int count = sprintf(buff, "Main program thread: Min of the Arithmetic Averages = %f\n", minAvg);
    write(fd, buff, count);
    write(1, buff, count);
    
    count = sprintf(buff, "Main program thread: Min of the Sums = %f\n", minSum);
    write(fd, buff, count);
    write(1, buff, count);

    count = sprintf(buff, "Main program thread: Min of the root of the sum of the squares = %f\n", minSqrt);
    write(fd, buff, count);
    write(1, buff, count);

    count = sprintf(buff, "Main program thread: Terminating.\n");
    write(fd, buff, count);
    write(1, buff, count);
    close(fd);
}