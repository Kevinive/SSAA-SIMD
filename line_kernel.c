#include <immintrin.h>

#include <stdio.h>

const double indexes[] = {0.0,1.0,2.0,3.0,
                          4.0,5.0,6.0,7.0,
                          8.0,9.0,10.0,11.0,
                          12.0,13.0,14.0,15.0};
const double one = 1.0;
const double scalar256 = 1.0/256.0;

void line_kernel(double *target,
                double *x,
                double *y,
                double color,
                double gradient,
                int n){

    double tempStore[40*4] = {0};
    __m256d t0,t1,t2,t3;
    __m256d t4,t5,t6,t7;
    __m256d t8,t9,t10,t11;
    __m256d t12,t13,t14,t15;
    double xGrad[16] = {0};
    double flStore[5*4] = {0};
    

    

    //t0 has delta = x*gradient for first, rest are free
    
    for (int i = 0; i < 4; ++i){
        //add delta + y_in
        
        t0 = _mm256_load_pd(&indexes[i*4]);
        t1 = _mm256_broadcast_sd(&x[0]);
        t2 = _mm256_broadcast_sd(&x[1]);
        t3 = _mm256_broadcast_sd(&x[2]);
        t4 = _mm256_broadcast_sd(&x[3]);
        t5 = _mm256_broadcast_sd(&x[4]);

        t6 = _mm256_broadcast_sd(&x[5]);
        t7 = _mm256_broadcast_sd(&x[6]);
        t8 = _mm256_broadcast_sd(&x[7]);
        t9 = _mm256_broadcast_sd(&x[8]);
        t10 = _mm256_broadcast_sd(&x[9]);

        t1 = _mm256_cmp_pd(t0, t1, 29);
        t2 = _mm256_cmp_pd(t0, t2, 29);
        t3 = _mm256_cmp_pd(t0, t3, 29);
        t4 = _mm256_cmp_pd(t0, t4, 29);
        t5 = _mm256_cmp_pd(t0, t5, 29);

        t6 = _mm256_cmp_pd(t0, t6, 18);
        t7 = _mm256_cmp_pd(t0, t7, 18);
        t8 = _mm256_cmp_pd(t0, t8, 18);
        t9 = _mm256_cmp_pd(t0, t9, 18);
        t10 = _mm256_cmp_pd(t0, t10, 18);
        

        t11 = _mm256_and_pd(t6, t1);
        t12 = _mm256_and_pd(t7, t2);
        t13 = _mm256_and_pd(t8, t3);
        t6 = _mm256_broadcast_sd(&gradient);
        t14 = _mm256_and_pd(t9, t4);
        t15 = _mm256_and_pd(t10, t5);

        t1 = _mm256_broadcast_sd(&y[0]);
        t2 = _mm256_broadcast_sd(&y[1]);
        t3 = _mm256_broadcast_sd(&y[2]);
        t4 = _mm256_broadcast_sd(&y[3]);
        t5 = _mm256_broadcast_sd(&y[4]);

        t11 = _mm256_and_pd(t0, t11);
        t12 = _mm256_and_pd(t0, t12);
        t13 = _mm256_and_pd(t0, t13);
        t14 = _mm256_and_pd(t0, t14);
        t15 = _mm256_and_pd(t0, t15);
        t0 = t6;
        t7 = _mm256_mul_pd(t6,t11);
        t8 = _mm256_mul_pd(t6,t11);
        t9 = _mm256_mul_pd(t6,t11);

        t1 = _mm256_add_pd(t7,t1);
        t2 = _mm256_add_pd(t8,t2);
        t3 = _mm256_add_pd(t9,t3);

        // t1 = _mm256_fmadd_pd (t6,t11, t1);
        // t2 = _mm256_fmadd_pd (t6,t12, t2);
        // t3 = _mm256_fmadd_pd (t6,t13, t3);
    
        //y_fl 6-10
        t6 = _mm256_floor_pd(t1);
        t4 = _mm256_fmadd_pd (t0,t14, t4);
        t7 = _mm256_floor_pd(t2);
        t5 = _mm256_fmadd_pd (t0,t15, t5);
        t8 = _mm256_floor_pd(t3);
    
        t9 = _mm256_floor_pd(t4);
        t15 = _mm256_broadcast_sd(&one);
    
        //can start doing y_fr = y - y_fl
        t1 = _mm256_sub_pd(t1,t6);
        t10 = _mm256_floor_pd(t5);
        t11 = _mm256_add_pd (t6, t15);
    
    
    
        t2 = _mm256_sub_pd(t2,t7);
        t12 = _mm256_add_pd (t7, t15);
    
    
        t3 = _mm256_sub_pd(t3,t8);
        _mm256_store_pd(&flStore[0],t6);
    
        t13 = _mm256_add_pd (t8, t15);
    
    
        t4 = _mm256_sub_pd(t4,t9);
        _mm256_store_pd(&flStore[4],t7);
    
        t14 = _mm256_add_pd (t9, t15);
    
        t5 = _mm256_sub_pd(t5,t10);
        _mm256_store_pd(&flStore[8],t8);
        t0 = _mm256_add_pd (t10, t15);
        _mm256_store_pd(&flStore[12],t9);
        _mm256_store_pd(&flStore[16],t10);
        t6 = _mm256_broadcast_sd(&y[5]);
        t7 = _mm256_broadcast_sd(&y[6]);
        t8 = _mm256_broadcast_sd(&y[7]);
        t9 = _mm256_broadcast_sd(&y[8]);
        t10 = _mm256_broadcast_sd(&y[9]);
    
        t11 = _mm256_cmp_pd(t6, t11, 29);
        t12 = _mm256_cmp_pd(t7, t12, 29);
        t13 = _mm256_cmp_pd(t8, t13, 29);
        t14 = _mm256_cmp_pd(t9, t14, 29);
        t0 = _mm256_cmp_pd(t10, t0, 29);
    
    
        t11 = _mm256_and_pd(t1, t11);
        t12 = _mm256_and_pd(t2, t12);
        t13 = _mm256_and_pd(t3, t13);
        t14 = _mm256_and_pd(t4, t14);
        t0 = _mm256_and_pd(t5, t0);
    
        _mm256_store_pd(&tempStore[0+(i*40)],t11);
        _mm256_store_pd(&tempStore[4+(i*40)],t12);
        _mm256_store_pd(&tempStore[8+(i*40)],t13);
        _mm256_store_pd(&tempStore[12+(i*40)],t14);
        _mm256_store_pd(&tempStore[16+(i*40)],t0);
    
        t1 = _mm256_sub_pd(t15,t1 );
        t2 = _mm256_sub_pd(t15,t2 );
        t3 = _mm256_sub_pd(t15,t3 );
        t4 = _mm256_sub_pd(t15,t4 );
        t5 = _mm256_sub_pd(t15,t5 );
    
        t11 = _mm256_load_pd(&flStore[0]);
        t12 = _mm256_load_pd(&flStore[4]);
        t13 = _mm256_load_pd(&flStore[8]);
        t14 = _mm256_load_pd(&flStore[12]);
        t0 = _mm256_load_pd(&flStore[16]);
    
        t6 = _mm256_cmp_pd(t11, t6,2);
        t7 = _mm256_cmp_pd(t12, t7,2);
        t8 = _mm256_cmp_pd(t13, t8,2);
        t9 = _mm256_cmp_pd(t14, t9,2);
        t10 = _mm256_cmp_pd(t0, t10,2);
    
        t11 = _mm256_and_pd(t6, t1);
        t12 = _mm256_and_pd(t7, t2);
        
        t13 = _mm256_and_pd(t8, t3);
        t14 = _mm256_and_pd(t9, t4);
        t15 = _mm256_and_pd(t10, t5);
    
    
        _mm256_store_pd(&tempStore[20+(i*40)],t11);
        _mm256_store_pd(&tempStore[24+(i*40)],t12);
        _mm256_store_pd(&tempStore[28+(i*40)],t13);
        _mm256_store_pd(&tempStore[32+(i*40)],t14);
        _mm256_store_pd(&tempStore[36+(i*40)],t15);
    }


    






    
    //resolve
    //load all stored values
    
    t0= _mm256_load_pd(&tempStore[0]);
    t1= _mm256_load_pd(&tempStore[4]);
    t2= _mm256_load_pd(&tempStore[8]);
    t3= _mm256_load_pd(&tempStore[12]);
    t4= _mm256_load_pd(&tempStore[16]);

    t5= _mm256_load_pd(&tempStore[20]);
    t6= _mm256_load_pd(&tempStore[24]);
    t7= _mm256_load_pd(&tempStore[28]);
    t8= _mm256_load_pd(&tempStore[32]);
    t9= _mm256_load_pd(&tempStore[36]);

    t10= _mm256_load_pd(&tempStore[40]);
    t11= _mm256_load_pd(&tempStore[44]);
    t12= _mm256_load_pd(&tempStore[48]);
    t13= _mm256_load_pd(&tempStore[52]);
    t14= _mm256_load_pd(&tempStore[56]);

    //t15= _mm256_load_pd(&tempStore[60]);
    
    t0 = _mm256_add_pd(t0,t5);
    t1 = _mm256_add_pd(t1,t6);
    t2 = _mm256_add_pd(t2,t7);
    t3 = _mm256_add_pd(t3,t8);
    t5= _mm256_load_pd(&tempStore[15*4]);
    t4 = _mm256_add_pd(t4,t9);
    t6= _mm256_load_pd(&tempStore[16*4]); 
    t0 = _mm256_add_pd(t0,t10);
    t7= _mm256_load_pd(&tempStore[17*4]);
    t1 = _mm256_add_pd(t1,t11);
    t8= _mm256_load_pd(&tempStore[18*4]);
    t2 = _mm256_add_pd(t2,t12);
    t9= _mm256_load_pd(&tempStore[19*4]);
    t3 = _mm256_add_pd(t3,t13);
    t10= _mm256_load_pd(&tempStore[20*4]);
    t4 = _mm256_add_pd(t4,t14);
    t11= _mm256_load_pd(&tempStore[21*4]);
    t0 = _mm256_add_pd(t0,t5);
    t12= _mm256_load_pd(&tempStore[22*4]);
    t1 = _mm256_add_pd(t1,t6);
    t13= _mm256_load_pd(&tempStore[23*4]);
    t2 = _mm256_add_pd(t2,t7);
    t14= _mm256_load_pd(&tempStore[24*4]);
    t3 = _mm256_add_pd(t3,t8);
    t5= _mm256_load_pd(&tempStore[25*4]);
    t4 = _mm256_add_pd(t4,t9);
    t6= _mm256_load_pd(&tempStore[26*4]); 
    t0 = _mm256_add_pd(t0,t10);
    t7= _mm256_load_pd(&tempStore[27*4]);
    t1 = _mm256_add_pd(t1,t11);
    t8= _mm256_load_pd(&tempStore[28*4]);
    t2 = _mm256_add_pd(t2,t12);
    t9= _mm256_load_pd(&tempStore[29*4]);
    t3 = _mm256_add_pd(t3,t13);
    t10= _mm256_load_pd(&tempStore[30*4]);
    t4 = _mm256_add_pd(t4,t14);
    t11= _mm256_load_pd(&tempStore[31*4]);
    t0 = _mm256_add_pd(t0,t5);
    t12= _mm256_load_pd(&tempStore[32*4]);
    t1 = _mm256_add_pd(t1,t6);
    t13= _mm256_load_pd(&tempStore[33*4]);
    t2 = _mm256_add_pd(t2,t7);
    t14= _mm256_load_pd(&tempStore[34*4]);
    t3 = _mm256_add_pd(t3,t8);
    t5= _mm256_load_pd(&tempStore[35*4]);
    t4 = _mm256_add_pd(t4,t9);
    t6= _mm256_load_pd(&tempStore[36*4]); 
    t0 = _mm256_add_pd(t0,t10);
    t7= _mm256_load_pd(&tempStore[37*4]);
    t1 = _mm256_add_pd(t1,t11);
    t8= _mm256_load_pd(&tempStore[38*4]);
    t2 = _mm256_add_pd(t2,t12);
    t9= _mm256_load_pd(&tempStore[39*4]);
    t3 = _mm256_add_pd(t3,t13);
    t10 = _mm256_broadcast_sd(&scalar256);
    t4 = _mm256_add_pd(t4,t14);
    t11 = _mm256_broadcast_sd(&color);
    t0 = _mm256_add_pd(t0,t5);
    t1 = _mm256_add_pd(t1,t6);
    t2 = _mm256_add_pd(t2,t7);
    t3 = _mm256_add_pd(t3,t8);
    t4 = _mm256_add_pd(t4,t9);



    //could probably do a color * scalar then do fmas, but bubbles


    t0 = _mm256_mul_pd(t0,t10);
    t1 = _mm256_mul_pd(t1,t10);
    t2 = _mm256_mul_pd(t2,t10);
    t3 = _mm256_mul_pd(t3,t10);
    t4 = _mm256_mul_pd(t4,t10);

    t0 = _mm256_mul_pd(t0,t11);
    t1 = _mm256_mul_pd(t1,t11);
    t2 = _mm256_mul_pd(t2,t11);
    t3 = _mm256_mul_pd(t3,t11);
    t4 = _mm256_mul_pd(t4,t11);

    _mm256_store_pd(&target[0],t0);
    _mm256_store_pd(&target[4],t1);
    _mm256_store_pd(&target[8],t2);
    _mm256_store_pd(&target[12],t3);
    _mm256_store_pd(&target[16],t4);

    // for (int i = 0; i < 4; ++i){
    //     target[0]+=tempStore[0+i];
    //     target[1]+=tempStore[4+i];
    //     target[2]+=tempStore[8+i];
    //     target[3]+=tempStore[12+i];
    //     target[4]+=tempStore[16+i];
    // }

    
    



}













