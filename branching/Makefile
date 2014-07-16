# makefile for doing some benchmarks
# Based on Matevz's makefile
# needs icc


# -mavx -msse4.2
VECHOST  := -mavx -vec-report=3
VECMIC   := -mmic -vec-report=3

CXX=icc


### To disable vectorization set USER_CXXFLAGS := -no-simd -no-vec
# Setting only one of the above has little effect.
# Note, this also screws-up prefetching so it's a lousy deal.

OPT      := -O3

CPPFLAGS := -I. ${USER_CPPFLAGS}
CXXFLAGS := ${OPT} -Wall -openmp -std=gnu++0x ${USER_CXXFLAGS} 

all: branchtest branchtest-mic

%.o: %.cc 
	$(CXX) ${CPPFLAGS} ${CXXFLAGS} ${VECHOST} -c -o $@ $<


%.om: %.cc 
	$(CXX) ${CPPFLAGS} ${CXXFLAGS} ${VECMIC} -c -o $@ $<

BRANCHTEST_DEPS := branchtest 


branchtest: $(addsuffix .o, ${BRANCHTEST_DEPS})
	$(CXX) ${CXXFLAGS} ${VECHOST} ${LDFLAGS} -o $@ $^



branchtest-mic: $(addsuffix .om, ${BRANCHTEST_DEPS})
	$(CXX) ${CXXFLAGS} ${VECMIC} ${LDFLAGS} -o $@ $^
	scp $@ mic0:

echo:
	@echo CPPFLAGS = ${CPPFLAGS}
	@echo CXXFLAGS = ${CXXFLAGS}
	@echo LDFLAGS  = ${LDFLAGS}


clean:
	rm -f branchtest *.o *.om

