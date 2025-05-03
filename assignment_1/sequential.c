#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

double gettime(void) {
    struct timeval ttime;
    gettimeofday(&ttime , NULL);
    return ttime.tv_sec + ttime.tv_usec * 0.000001;
}

int main(int argc, char *argv[]) {

    int m, n, l, i, j, k, num;
    int **A, **B, **hamming_distance;
    double elapse_time;
    FILE *fptr;
    int count = 0;
    char file_name[100];

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

    if ((fptr = fopen(file_name,"r")) == NULL){
        printf("Error! opening file");

        // Program exits if the file pointer returns NULL.
        exit(1);
    }

    for(i = 0 ; i < m ; i++) {
        for(j = 0 ; j < l ; j++) {
            fscanf(fptr, "%d\n", &num);
            A[i][j] = num;
        }
    }
    for(i = 0 ; i < n ; i++) {
        for(j = 0 ; j < l ; j++) {
            fscanf(fptr,"%d\n",&num);
            B[i][j] = num; 
        }
    }

    double time0 = gettime();
    
    for(i = 0 ; i < m ; i++ ) {
        for( j = 0 ; j < n ; j++) {
            for(k =0 ; k < l; k++) {
                if(A[i][k] != B[j][k]) {
                    count++;
                }
            }
            hamming_distance[i][j] = count;
            count = 0;
        }
    }

    long h_count = 0;

    for(i = 0 ; i < m ; i++ ) {
        for( j = 0 ; j < n ; j++) {
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

    printf("\n*** Done! ***\n");
    return 0;
}
