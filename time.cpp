#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

static struct timeval tvBegin, tvEnd, tvDiff;

/* Return 1 if the difference is negative, otherwise 0.  */
extern int timeval_subtract(struct timeval *result, struct timeval *t2, struct timeval *t1)
{
    long int diff = (t2->tv_usec + 1000000 * t2->tv_sec) - (t1->tv_usec + 1000000 * t1->tv_sec);
    result->tv_sec = diff / 1000000;
    result->tv_usec = diff % 1000000;

    return (diff<0);
}


void start_time() {
  gettimeofday(&tvBegin, NULL);
}

void stop_time(int rank, int batch_size, int splitting) {

  gettimeofday(&tvEnd, NULL);
  timeval_subtract(&tvDiff, &tvEnd, &tvBegin);

  if (rank == 0)
    printf("ADIOS time: %ld.%06ld Size: %d, Nb writes: %d \n", tvDiff.tv_sec, tvDiff.tv_usec, batch_size, splitting);

}
