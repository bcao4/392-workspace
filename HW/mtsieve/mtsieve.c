/*******************************************************************************
 * Name        : mtsieve.c
 * Author      : Jerry Yu and Brandon Cao
 * Date        : 4/20/20
 * Description : mtsieve
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h>
#include <sys/sysinfo.h>

int total_count = 0; 
pthread_mutex_t lock; 

typedef struct arg_struct {     
    int start;     
    int end; 
} thread_args;

void SieveOfEratosthenes(int n, int low_primes[], int *num_primes) {
    bool prime[n+1];
    memset(prime, true, sizeof(prime));   
    for (int p=2; p*p<=n; p++) {
        if (prime[p] == true) {
            for (int i=p*p; i<=n; i += p) {
                prime[i] = false;
            }
        }
    }
    int j = 0;
    int i = 0;
    for (i=2; i <= n; i++) {
        if(prime[i] == true) {
            low_primes[j] = i;
            j++; 
        }
    }
    // Number of primes found and saves them in low_primes
    *num_primes = j;
    return;    
}

/*
void *seg_sieve(void *ptr) {
    thread_args *thread = (thread_args *)ptr;
    
    int low_primes[];
    int l = thread->end - thread->start + 1];
    bool high_primes[l];
    memset(high_primes, true, sizeof(high_primes)); 

    for (int i = 0; i < len(low_primes); i++) {
        int p = low_primes[i];
        int j = ceil((double)thread->start/p) * p - thread->start;
        if (thread->start <= p) {
            j = j + p;
        }
        for (int k = p*p; k < l; k += p) {
            high_primes[k] = false; 
        }
    }

    int retval;
    if ((retval = pthread_mutex_lock(&lock)) != 0) {
        fprintf(stderr, "Warning: Cannot lock mutex. %s.\n", strerror(retval));
    }

    if ((retval = pthread_mutex_unlock(&lock)) != 0) {
        fprintf(stderr, "Warning: Cannot unlock mutex. %s.\n", strerror(retval));
    }
    
    pthread_exit(NULL);
}
*/

void count_repeating_digits(int prime_num) {
    int find_3 = 0;
    int ret = 0;

    while(prime_num) {
       if(prime_num % 10 == 3) {
           find_3++;
           if(find_3 > 1) {
               if ((ret = pthread_mutex_lock(&lock)) != 0) {
                   fprintf(stderr, "Warning: Cannot lock mutex. %s.\n", strerror(ret));
               }
               total_count++;
               if ((ret = pthread_mutex_unlock(&lock)) != 0) {
                   fprintf(stderr, "Warning: Cannot unlock mutex. %s.\n", strerror(ret));
               }
               break;
           }
       }  
       prime_num = prime_num/10;
    }
}

void segmented_soe(int start_value, int end_value) {
    int sqrt_end_value = sqrt(end_value) + 1;
    int low_primes[sqrt_end_value];
    int elements_in_low_primes = 0;
    memset(low_primes, 0, sizeof(low_primes)); 
    SieveOfEratosthenes(sqrt_end_value, low_primes, &elements_in_low_primes); 

    bool *high_primes = NULL;
    high_primes = (bool *)malloc(end_value - start_value + 1);
    if(high_primes == NULL) {
        fprintf(stderr, "malloc() failed. \n");
        exit(EXIT_FAILURE);
    }

    memset(high_primes, true, end_value - start_value + 1); 

    int i = 0, k = 0, p = 0;
    for (i = 0; i < elements_in_low_primes; i++) {
        /* each prime p in low_primes[] */
        p = low_primes[i];
        int j = ceil((double)start_value/p) * p;
        for(k = j; k <= end_value; k+=p) { 
            high_primes[k-start_value] = false;
        }
    }

    /* Count total prime number having two or more digits that are '3' */
    int n = 0;
    for(n = 0; n < elements_in_low_primes; n++) {
        if(low_primes[n] >= start_value) {
            count_repeating_digits( low_primes[n] );
        }
    }
    for (k = start_value; k <= end_value; k++) {
        if(high_primes[k - start_value] == true) {
            count_repeating_digits( k );
        }
    }

    free(high_primes);
    return;
}

bool isint(char *in) {
    for (int i = 0; i < strlen(in); i++) {
        if (!isdigit(in[i])) {
            return false;
        }
    }
    return true;
}

// Fills segments into thread args
void fill_thread_args(int start_value, int end_value, int seg_members, int remainder, int segments, thread_args targs[]) {
    int pre_end = 0;
    int i = 0;

    for(i = 0; i < segments; i++) {
        if(pre_end == 0) {
            targs[i].start = start_value;
        }
        else {
            targs[i].start = pre_end + 1; 
        }
        if(remainder > 0) {
            targs[i].end = targs[i].start + (seg_members - 1) + 1; 
            remainder--;
        }
        else{
            targs[i].end = targs[i].start + (seg_members - 1);
        } 
        pre_end = targs[i].end; 
    }
}

void print_segments(thread_args targs[], int segments) {
    int i = 0;
    if(segments > 1) {
        fprintf(stderr, "%d segments: \n", segments); 
    }
    else {
        fprintf(stderr, "%d segment: \n", segments); 
    }
    for(i = 0; i < segments; i++) {
        fprintf(stderr, "   [%d, %d]\n", targs[i].start, targs[i].end); 
    }
    return;
}

int create_segments(int start_value, int end_value, int thread_num, int *segments, int *seg_members, int *remainder) {
    int total_members = end_value - start_value + 1;
    if(segments == NULL) {
        return EXIT_FAILURE;
    }
    *segments = thread_num;
    *seg_members = 0;
    *remainder = 0;

    if(*segments > total_members) {
        *segments = total_members;  
    }
    *seg_members = total_members / *segments;
    *remainder = total_members % *segments;    
    return 0;
}

int parse(int argc, char **argv, int *start, int *end, int *thread) {
    int c;
    int start_value = -1, end_value = -1, thread_num = -1;
    int num_processors = 0;

    while((c = getopt (argc, argv, ":s:e:t:")) != -1)
    {
        switch (c)
        {
            case 's':
                if(isint(optarg) == false) {
                    fprintf( stderr, "Error: Invalid input '%s' received for parameter '-%c'\n", optarg, c);
                    return EXIT_FAILURE;
                }
                start_value = atoi(optarg);
                long temp = atol(optarg);
                if(start_value != temp) {
                    fprintf( stderr, "Error: Integer overflow for parameter '-s'.\n");
                    return EXIT_FAILURE;
                }
                break;
            case 'e':
                if(isint(optarg) == false) {
                    fprintf( stderr, "Error: Invalid input '%s' received for parameter '-%c'\n", optarg, c);
                    return EXIT_FAILURE;
                }
                end_value = atoi(optarg);
                long tem = atol(optarg);
                if(end_value != tem) {
                    fprintf( stderr, "Error: Integer overflow for parameter '-e'.\n");
                    return EXIT_FAILURE;
                }
                break;
            case 't':
                if(isint(optarg) == false) {
                    fprintf(stderr, "Error: Invalid input '%s' received for parameter '-%c'\n", optarg, c);
                    return EXIT_FAILURE;
                }
                thread_num = atoi(optarg);
                long te = atol(optarg);
                if(thread_num != te) {
                    fprintf(stderr, "Error: Integer overflow for parameter '-t'.\n");
                    return EXIT_FAILURE;
                }
                break;
            case '?':
                if (optopt == 'e' || optopt == 's' || optopt == 't') {
                    fprintf( stderr, "Error: Option -%c requires an argument.\n", optopt);
                } else if (isprint(optopt)) {
                    fprintf(stderr, "Error: Unknown option '-%c'.\n", optopt);
                } else {
                    fprintf(stderr, "Error: Unknown option character '\\x%x'.\n", optopt);
                }
                return EXIT_FAILURE;
            case ':': 
                fprintf(stderr, "Error: Option -%c requires an argument.\n", optopt);
                return EXIT_FAILURE;
        } 
    }
    if(argv[optind] != NULL) {
        fprintf (stderr, "Error: Non-option argument '%s' supplied.\n", argv[optind]);
        return EXIT_FAILURE;
    }
    if(start_value == -1 && end_value == -1 && thread_num == -1) {
        fprintf(stderr, "Usage: ./mtsieve -s <starting value> -e <ending value> -t <num threads>\n");
        return EXIT_FAILURE;
    }
    if(start_value == -1) {
        fprintf( stderr, "Error: Required argument <starting value> is missing.\n" );
        return EXIT_FAILURE;
    }
    if(start_value < 2) {
        fprintf( stderr, "Error: Starting value must be >= 2.\n" );
        return EXIT_FAILURE;
    }
    if(end_value == -1) {
        fprintf( stderr, "Error: Required argument <ending value> is missing.\n" );
        return EXIT_FAILURE;
    }
    if( end_value < 2 ) {
        fprintf( stderr, "Error: Ending value must be >= 2.\n" );
        return EXIT_FAILURE;
    }
    if( end_value < start_value ) {
        fprintf( stderr, "Error: Ending value must be >= starting value.\n" );
        return EXIT_FAILURE;
    }
    if( thread_num == -1 ) {
        fprintf( stderr, "Error: Required argument <num threads> is missing.\n" );
        return EXIT_FAILURE;
    }
    if( thread_num < 1 ) {
        fprintf( stderr, "Error: Numbers of threads cannot be less than 1.\n" );
        return EXIT_FAILURE;
    }
    num_processors = get_nprocs();
    if( thread_num > 2 * num_processors ) {
        fprintf( stderr, "Error: Numbers of threads cannot exceed twice the number of processors(%d). \n", num_processors );
        return EXIT_FAILURE;
    } 

    if(start) {
        *start = start_value;
    }
    if(end) {
        *end = end_value;
    }
    if(thread) {
        *thread = thread_num;
    }
    return 0;
}

void *SegmentedSieveOfEratosthenes(void * t_args) {
    thread_args *targs = (thread_args *) t_args;
    segmented_soe(targs->start, targs->end);
    pthread_exit(NULL);
}

int main(int argc, char **argv) {
    int ret_val = 0;
    int start_value = -1, end_value = -1, num_threads = -1;
    int segments = 0, seg_members = 0, remainder = 0;

    ret_val = parse(argc, argv, &start_value, &end_value, &num_threads);
    if(ret_val == EXIT_FAILURE) {
        return EXIT_FAILURE;
    } else {
        printf("Finding all prime numbers between %d and %d.\n", start_value, end_value);
    }

    create_segments(start_value, end_value, num_threads, &segments, &seg_members, &remainder);
    pthread_t threads[segments];
    thread_args targs[segments];
    fill_thread_args(start_value, end_value, seg_members, remainder, segments, targs);
    print_segments(targs, segments);
    pthread_mutex_init(&lock, NULL);

    for(int i = 0; i < segments; i++) {
        ret_val = pthread_create(&threads[i], NULL, SegmentedSieveOfEratosthenes, (void *)(&targs[i]));
        if(ret_val != 0 ) {
            fprintf(stderr, "Error: Cannot create consumer thread %d. %s.\n", i, strerror(errno));
            return EXIT_FAILURE;
        }
    }
    for (int i = 0; i < segments; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            fprintf(stderr, "Warning: Thread %ld did not join properly.\n", threads[i]);
        }
    }

    pthread_mutex_destroy(&lock);
    printf("Total primes between %d and %d with two or more '3' digits: %d\n", start_value, end_value, total_count);

    return ret_val;
}
