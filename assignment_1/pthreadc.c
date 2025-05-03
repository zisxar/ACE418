//pthrc

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>
#define NUM_THREADS 4

int m, n, l, index_A, k, num, i, j;
long tid;
int **A, **B, **hamming_distance;
double elapse_time;
FILE *fptr;
int count = 0;
char file_name[100];
pthread_t threads[NUM_THREADS];
int rc;
pthread_barrier_t barrier0;
pthread_mutex_t t_mutex;

struct thread_data{
    int posA,tid;
};

double gettime(void) {
    struct timeval ttime;
    gettimeofday(&ttime , NULL);
    return ttime.tv_sec + ttime.tv_usec * 0.000001;
}

struct thread_data thread_data_array[NUM_THREADS];

void *Hamming_Distance_Function(void *threadid) {

    int cnt;
    struct thread_data *mydata;

    cnt = 0;
    mydata = (struct thread_data *) threadid;
    for (int index_B = 0; index_B < n; index_B++) {
        for(int i = 0; i < l; i++) {
            if(A[mydata->posA][i] != B[index_B][i]) {
                cnt++;
            }
        }
        hamming_distance[mydata->posA][index_B] = cnt;
        cnt = 0;
    }

    
    pthread_barrier_wait(&barrier0);
    pthread_exit(NULL);
}

 int main (int argc, char *argv[]) {

    printf("Give me the number of the second string (m)");
    scanf("%d", &m);
    printf("Give me the number of the first string (n)");
    scanf("%d", &n);
    printf("Give me the number of the size of the strings (l)");
    scanf("%d", &l);

    // m rows and l columns, for each element goes for comparison
    A = (int**) malloc(m * sizeof(int*));
    for (i = 0; i < m; ++i) {
        A[i] = (int*) malloc(l * sizeof(int));
    }
    B = (int**) malloc(n * sizeof(int*));
    for (i = 0; i < n; ++i) {
        B[i] = (int*) malloc(l * sizeof(int));
    }
    hamming_distance = (int**) malloc(m * sizeof(int*));
    for (i = 0; i < m; ++i) {
        hamming_distance[i] = (int*) malloc(n * sizeof(int));
    }

    char s1[6],s2[6],s3[6];

    sprintf(s1, "%d", m);
    sprintf(s2, "%d", n);
    sprintf(s3, "%d", l);

    strcat(file_name, "z_m_");
    strcat(file_name, s1);
    strcat(file_name, "_n_");
    strcat(file_name, s2);
    strcat(file_name, "_l_");
    strcat(file_name, s3); 

    pthread_mutex_init(&t_mutex, NULL);

    if ((fptr = fopen(file_name,"r")) == NULL) {
        printf("Error! opening file\n");

        // Program exits if the file pointer returns NULL.
        exit(1);
    }
    for(i = 0 ; i < m ; i++) {
        for(j = 0 ; j < l ; j++) {
            fscanf(fptr, "%d\n", &num);
            A[i][j] = num;
        }
    }

    for(i = 0 ; i < n ; i++){
        for(j = 0 ; j < l ; j++){
            fscanf(fptr,"%d\n",&num);
            B[i][j] = num;
        }
    }

    srand(time(NULL));
    pthread_barrier_init(&barrier0, NULL, NUM_THREADS + 1);

    double time0 = gettime();
    /* *** code for threads *** */
    if(m ==1){
        count = 0;
        for(i = 0; i < n ; i++){
            for(j = 0; j < l ; j++) {
                if(A[0][j] != B[i][j]) {
                    count++;
                }
            }
            hamming_distance[0][j] = count;
        }
    }else{
        for(index_A = 0; index_A < m; index_A += NUM_THREADS) {
            for (tid = 0; tid < NUM_THREADS; tid++) {
                thread_data_array[tid].posA = index_A + tid;
                thread_data_array[tid].tid =  tid;
                
                rc = pthread_create(&threads[tid], NULL, Hamming_Distance_Function, (void *)&thread_data_array[tid]);
                if (rc) {
                    printf("ERROR; return code from pthread_create() is %d\n", rc);
                    exit(-1);
                }
            }
            pthread_barrier_wait(&barrier0);
            
            for (tid = 0; tid < NUM_THREADS; tid++) {
                pthread_join(threads[tid], NULL);
            }
        }
    }

    long h_count = 0;
    for ( i = 0 ; i < m ; i++ ) {
        for ( j = 0 ; j < n ; j++) {
            h_count += hamming_distance[i][j];
        }
    }
         
    double time1 = gettime();
    
    elapse_time = time1 - time0;

    printf("The sum of all Hamming distances is : %ld !!!!!\n", h_count);

    printf("Time to calculate distance hamming is : %f !!!!!\n",elapse_time);

    for (i = 0; i < m; ++i) {
        free(hamming_distance[i]);
    }
    free(hamming_distance);

    for (i = 0; i < m; ++i) {
        free(A[i]);
    }
    free(A);

    for (i = 0; i < n; ++i) {
        free(B[i]);
    }
    free(B);

    fclose(fptr);

    pthread_barrier_destroy(&barrier0);

    printf("\n*** Done! ***\n");
    return 0;
 }