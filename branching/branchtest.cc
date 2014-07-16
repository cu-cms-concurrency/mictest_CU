#include <iostream>
#include <cmath>
#include <cstring>
#include <vector>
#include <numeric>
#include <iterator>
#include <sys/time.h>
#include <time.h>
#include <omp.h>

//#include "rand.c" // ugly

//#include "BasicBenchmarks.h"

// From Ian, adapted

enum FillType{ZERO,LINEAR};

template<typename T>
void newArray(T*& array, unsigned int length, int allignBytes = -1, 
	      FillType filltype = LINEAR){
  if(allignBytes < 0){
	array = (T*) malloc(sizeof(T)*length);
  }else{
	array = (T*) _mm_malloc(sizeof(T)*length, allignBytes);
  }
  // alignbytes I think just shifts the memory so this should work
  memset(array, 0, length*sizeof(T));
}

// end From Ian



#define PFUNC apb3
#define NFUNC amb3

double get_wall_time(){
    struct timeval time;
    if (gettimeofday(&time,0)){
        //  Handle error
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}
double get_cpu_time(){
    return (double)clock() / CLOCKS_PER_SEC;
}

inline float apb3(float *a,float *b, int i)
{
  const float asq = a[i] * a[i];
  const float bsq = b[i] * b[i];
  return a[i]*bsq + b[i]*bsq + 3*(asq*b[i] + bsq*a[i]);
}
inline float amb3(float *a,float *b, int i)
{
  const float asq = a[i] * b[i];
  const float bsq = b[i] * a[i];
  return a[i]*asq - b[i]*bsq - 3*(asq*b[i] - bsq*a[i]);
}

inline float apb4 (float *a,float *b, int i) 
{
  const float asqr = a[i]*a[i], acub = a[i]*asqr;
  const float bsqr = b[i]*b[i], bcub = b[i]*bsqr;
  
  return a[i]*acub + 4*(acub*b[i] + a[i]*bcub) + 6*asqr*bsqr + b[i]*bcub;
}

inline float amb4 (float *a,float *b, int i) 
{
  const float asqr = a[i]*a[i], acub = a[i]*asqr;
  const float bsqr = b[i]*b[i], bcub = b[i]*bsqr;
  
  return a[i]*acub - 4*(acub*b[i] + a[i]*bcub) - 6*asqr*bsqr + b[i]*bcub;
}

inline float apb5(float *a, float *b, int i)
{
    const float asqr = a[i]*a[i], acub = a[i]*asqr, aqud = asqr*asqr;
    const float bsqr = b[i]*b[i], bcub = b[i]*bsqr, bqud = bsqr*bsqr;

    return a[i]*aqud + 5*(aqud*b[i] + a[i]*bqud) 
      + 10*(acub*bsqr + asqr*bcub) + b[i]*bqud;
}


inline float amb5(float *a, float *b, int i)
{
    const float asqr = a[i]*a[i], acub = a[i]*asqr, aqud = asqr*asqr;
    const float bsqr = b[i]*b[i], bcub = b[i]*bsqr, bqud = bsqr*bsqr;

    return a[i]*aqud + 5*(-aqud*b[i] + a[i]*bqud) 
      + 10*(acub*bsqr - asqr*bcub) + b[i]*bqud;
}


// these are bullocks
inline float b1(float *a, float *b, int i )
{

  return a[i]*b[i] + b[i]*b[i] + 1/b[i];

}

inline float b2(float *a, float *b, int i )
{
  return a[i]*a[i] + b[i]*a[i] + 1/a[i];

}


int main(int argc, char **argv)
{

  const unsigned int NENTRIES = 2<<24;
  const int ntries = 100;
  std::cout << "nentries = " << NENTRIES << std::endl;
  std::cout << "CLOCKS_PER_SEC = " << CLOCKS_PER_SEC << std::endl;
  std::cout << "ntries = " << ntries << std::endl;
  const int ALIGN = 64;

  srand(323467791);

  float threshold = 0.5;
  if ( argc > 1 ) {
    threshold = atof(argv[1]);
  }
  std::cout << "threshold = " << threshold << std::endl;

  float *a, *b, *c, *d, *e, *f;

  newArray(a, NENTRIES, ALIGN, ZERO);
  newArray(b, NENTRIES, ALIGN, ZERO);
  newArray(c, NENTRIES, ALIGN, ZERO);
  newArray(d, NENTRIES, ALIGN, ZERO);
  newArray(e, NENTRIES, ALIGN, ZERO);
  newArray(f, NENTRIES, ALIGN, ZERO);
  __assume_aligned(a, 64);
  __assume_aligned(b, 64);
  __assume_aligned(c, 64);
  __assume_aligned(d, 64);
  __assume_aligned(e, 64);
  __assume_aligned(f, 64);
  __assume(NENTRIES%16==0);




  std::vector<double> walldiff, cpudiff;
  for ( int  j = 0; j < ntries; ++j ) {
    // prefill
#pragma ivdep
    for ( unsigned int i = 0; i < NENTRIES; ++i ) {
      a[i] = 1000.0*rand()/RAND_MAX; // btw 0-1000
      b[i] = 1000.0*rand()/RAND_MAX; // btw 0-1000
      c[i] = 1.0*rand()/RAND_MAX; // btw 0-1
    }    
    double wall_start = get_wall_time();
    double cpu_start = get_cpu_time();
#pragma ivdep
    for(unsigned int i = 0; i < NENTRIES; ++i){
#define WITHBRANCHES
#ifdef WITHBRANCHES
      if ( c[i] < threshold ) {
	f[i] = PFUNC(a,b,i);
      }
      else {
       	f[i] = NFUNC(a,b,i);
      }
#else // WITHBRANCHES
      // const float asq = a[i] * a[i];
      // const float bsq = b[i] * b[i];
      // f[i] = a[i]*asq + b[i]*bsq + 3*(asq*b[i] + bsq*a[i]);
      f[i] = PFUNC(a,b,i);
#endif // WITHBRANCHES
    }
    double cpu_stop = get_cpu_time();
    double wall_stop = get_wall_time();
    cpudiff.push_back(cpu_stop-cpu_start);
    walldiff.push_back(wall_stop-wall_start);
  }

  // c++11 fanciness
  for (auto cc : cpudiff)
    std::cout << cc << ' ';
  std::cout << std::endl;


  double sum = std::accumulate(cpudiff.begin(), cpudiff.end(), 0.0);
  std::cout << "sum = " << sum << std::endl;
  double mean = sum / cpudiff.size();

  double sq_sum = std::inner_product(cpudiff.begin(), cpudiff.end(), 
				     cpudiff.begin(), 0.0);
  double stdev = std::sqrt(sq_sum / cpudiff.size() - mean * mean);
  
  printf("Thresh %5.2f CPU time: %5.3g +- %5.3g\n", threshold, 
	 mean/NENTRIES, stdev/NENTRIES);
  //printf("Wall time: %5.2g\n", wall_stop-wall_start);


  _mm_free(a);
  _mm_free(b);
  _mm_free(c);
  _mm_free(d);
  _mm_free(e);
  _mm_free(f);


  return 0;
}
