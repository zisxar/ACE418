#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main() {

    int m,n,l,i,j,k;
    int *A,*B;
    FILE *fptr;
    char file_name[100];
    
    int mn_ar[] = {1, 100, 1000, 10000};
    int l_ar[] = {10, 100, 1000};

    for (int pm=0; pm<4; pm++) {
        for (int pn=0; pn<4; pn++) {
            for (int pl=0; pl<3; pl++) {
                
                m = mn_ar[pm];
                n = mn_ar[pn];
                l = l_ar[pl];

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

                //printf("%s\n", file_name);

                fptr = fopen(file_name,"w");
                if (fptr == NULL) {
                    printf("Error!");   
                    exit(1);             
                }
                
                for(i = 0 ; i < m ; i++){
                    for(j = 0 ; j < l ; j++){
                        fprintf(fptr, "%d\n", rand() % 2);
                    }
                }
                for(i = 0 ; i < n ; i++){
                    for(j = 0 ; j < l ; j++){
                        fprintf(fptr, "%d\n", rand() % 2);
                    }
                }
                
                fclose(fptr);
                
                for (k = 0; k < 100; k++) {
                    file_name[k] = NULL;
                }
            }
        }        
    }
    
    printf("\n*** Done! ***\n");
    return 0;
}
