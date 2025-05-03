#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>

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

    int N = atoi(argv[1]);

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
    for(int j=0;j<iters;j++) {
        
        double time0=gettime();
/************************************************************/
        for(int i=0;i<N;i+=4) {

                
            //float num_0 = LVec[i]+RVec[i]; //
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
/************************************************************/        
        double time1=gettime();
        timeTotal += time1-time0;
    }
    
    printf("Time %f Max %f\n", timeTotal/iters, maxF);
    
    free(mVec);
    free(nVec);
    free(LVec);
    free(RVec);
    free(CVec);
    free(FVec);
}
