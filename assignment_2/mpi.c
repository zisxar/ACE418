#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>
#include <mpi.h>
#include <math.h>

#include <xmmintrin.h>

double gettime(void) {
    struct timeval ttime;
    gettimeofday(&ttime , NULL);
    return ttime.tv_sec + ttime.tv_usec * 0.000001;
}

float randpval () {
    int vr = rand();
    int vm = rand()%vr;
    float r = ((float)vm)/(float)vr;
    assert(r>=0.0 && r<=1.00001);
    return r;
}

int main(int argc, char ** argv) {

/************************************************************/

    MPI_Status Stat;
    int numtasks, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
/************************************************************/

    int N = atoi(argv[1]);

/************************************************************/
    
    int source;
    
    int begin = rank*(N/numtasks);
    int size_of_N = (rank+1)*(N/numtasks);
    
    int newi;

/************************************************************/

    int iters = 1000;
    //int iters = 1;

    srand(1);
    
    float * mVec = (float*)_mm_malloc(sizeof(float)*N, 16);
    assert(mVec!=NULL);
    
    float * nVec = (float*)_mm_malloc(sizeof(float)*N, 16);
    assert(nVec!=NULL);
    
    float * LVec = (float*)_mm_malloc(sizeof(float)*N, 16);
    assert(LVec!=NULL);
    
    float * RVec = (float*)_mm_malloc(sizeof(float)*N, 16);
    assert(RVec!=NULL);
    
    float * CVec = (float*)_mm_malloc(sizeof(float)*N, 16);
    assert(CVec!=NULL);
    
    float * FVec = (float*)_mm_malloc(sizeof(float)*N, 16);
    assert(FVec!=NULL);
    
    for(int i=0;i<N;i++) {
        mVec[i] = (float)(2+rand()%10);
        nVec[i] = (float)(2+rand()%10);
        LVec[i] = 0.0;
        for(int j=0;j<mVec[i];j++) {
            LVec[i] += randpval();
        }
        RVec[i] = 0.0;
        for(int j=0;j<nVec[i];j++) {
            RVec[i] += randpval();
        }
        CVec[i] = 0.0;
        for(int j=0;j<mVec[i]*nVec[i];j++) {
            CVec[i] += randpval();
        }
        FVec[i] = 0.0;

        assert(mVec[i]>=2.0 && mVec[i]<=12.0);
        assert(nVec[i]>=2.0 && nVec[i]<=12.0);
        assert(LVec[i]>0.0 && LVec[i]<=1.0*mVec[i]);
        assert(RVec[i]>0.0 && RVec[i]<=1.0*nVec[i]);
        assert(CVec[i]>0.0 && CVec[i]<=1.0*mVec[i]*nVec[i]);
    }

    float maxF = 0.0f;
    double timeTotal = 0.0f;
    double ctimeTotal = 0;
     
    for(int j=0;j<iters;j++) {
        
        double time0=gettime();
/************************************************************/

        int i=0;

        for(i=begin ; i<size_of_N ; i+=4) {
            
            __m128 lv = _mm_load_ps(&LVec[i]);
            __m128 rv = _mm_load_ps(&RVec[i]);
            __m128 num_0_vec = _mm_add_ps(lv, rv);
            /////

            __m128 const_1 = _mm_set1_ps (1.0);
            __m128 const_2 = _mm_set1_ps (2.0);

            //float num_1 = mVec[i]*(mVec[i]-1.0)/2.0;
            __m128 mv = _mm_load_ps(&mVec[i]);
            __m128 t1_vec = _mm_sub_ps (mv, const_1);
            __m128 t2_vec = _mm_div_ps (t1_vec, const_2);
            __m128 num_1_vec = _mm_mul_ps (mv, t2_vec);
            /////
                
            //float num_2 = nVec[i]*(nVec[i]-1.0)/2.0;
            __m128 nv = _mm_load_ps(&nVec[i]);
            t1_vec = _mm_sub_ps (nv, const_1);
            t2_vec = _mm_div_ps (t1_vec, const_2);
            __m128 num_2_vec = _mm_mul_ps (nv, t2_vec);
            /////

            //float num = num_0/(num_1+num_2);
            __m128 num_add_vec = _mm_add_ps(num_1_vec, num_2_vec);
            __m128 num_vec = _mm_div_ps (num_0_vec, num_add_vec);
            /////
                
            //float den_0 = CVec[i]-LVec[i]-RVec[i];
            __m128 cv = _mm_load_ps(&CVec[i]);
            __m128 t3_vec = _mm_sub_ps (cv, lv);
            __m128 den_0_vec = _mm_sub_ps (t3_vec, rv);
            /////

            //float den_1 = mVec[i]*nVec[i];
            __m128 den_1_vec = _mm_mul_ps (mv, nv);
            /////

            //float den = den_0/den_1;
            __m128 den_vec = _mm_div_ps (den_0_vec, den_1_vec);
            /////
                
            //FVec[i] = num/(den+0.01);
            __m128 const_3 = _mm_set1_ps (0.01);
            __m128 t4_vec = _mm_add_ps(den_vec, const_3);
            __m128 fv = _mm_div_ps (num_vec, t4_vec);
            _mm_store_ps(&FVec[i], fv);
            /////

            //maxF = FVec[i]>maxF?FVec[i]:maxF;
            for (int k = 0; k < 4; k++) {
                maxF = FVec[i+k]>maxF?FVec[i+k]:maxF;
            }
            /////
        }

        i -= 4;

        for (newi = i; newi<size_of_N; newi++) {
            float num_0 = LVec[newi]+RVec[newi];
            float num_1 = mVec[newi]*(mVec[newi]-1.0)/2.0;
            float num_2 = nVec[newi]*(nVec[newi]-1.0)/2.0;
            float num = num_0/(num_1+num_2);
            
            float den_0 = CVec[newi]-LVec[newi]-RVec[newi];
            float den_1 = mVec[newi]*nVec[newi];
            float den = den_0/den_1;
            
            FVec[newi] = num/(den+0.01);
            
            maxF = FVec[newi]>maxF?FVec[newi]:maxF;
        }

/************************************************************/        
        double time1 = gettime();
        timeTotal += time1-time0;        
/************************************************************/

    }

    if (rank == 0) {
        double c_time0 = gettime();
        float maxFmaster = 0;
        double masterTime = 0;
        for (source = 1; source < numtasks; source++) {
            MPI_Recv(&maxFmaster, 1, MPI_FLOAT, source, 1, MPI_COMM_WORLD, &Stat);
            MPI_Recv(&masterTime, 1, MPI_DOUBLE, source, 2, MPI_COMM_WORLD, &Stat);
            maxF = maxFmaster>maxF?maxFmaster:maxF;
            timeTotal = masterTime>timeTotal?masterTime:timeTotal;
        }
        double c_time1 = gettime();
        ctimeTotal += c_time1-c_time0;
    }
    else {
        MPI_Send(&maxF, 1, MPI_FLOAT, 0, 1, MPI_COMM_WORLD);
        MPI_Send(&timeTotal, 1, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD);
    }

/************************************************************/

    if (rank == 0) {
        printf("Calculation Time %f + Communication Time %f = Total Time %f \n Max %f\n", timeTotal/iters, ctimeTotal/iters, timeTotal/iters + ctimeTotal/iters, maxF);
    }

    free(mVec);
    free(nVec);
    free(LVec);
    free(RVec);
    free(CVec);
    free(FVec);

    MPI_Finalize();
}