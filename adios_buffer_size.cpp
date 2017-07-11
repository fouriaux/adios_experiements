
/*
 * This demonstrate Sub Blocks of array writting using no XML API
 * Each rank will have 2 chunk of data to write in data array
 * Each rank is writting is own rank ID on file to easy visualize results
 *
 */


#include <iostream>
#include <cstdlib>
#include <mpi.h>
#include <adios.h>
#include <vector>
#include <math.h>
#define MASTER 0

static int64_t                               adios_handle;          // adios file handle
static int64_t                               adios_group_id;        // adios group id 
static std::vector<int64_t>                  data_ids, offset_ids;  // adios declared variables ids
static int                                   max_buffer_size;       // adios internal buffer size in megabytes
static const char*                           method;                // adios method

static int                                   rank;                  // mpi rank id of a process
static int                                   nb_ranks;              // nb ranks in MPI_COMM_WORLD 
static int                                   total_size;            // total_size to be written
static int*                                  buffer;                // allocated buffer to write of file per rank
static int                                   batch_size;            // size of one write in 
static int                                   nb_batchs;             // number of batchs to write per rank

static MPI_Comm                              comm = MPI_COMM_WORLD; // MPI Communicator
static char*                                 out_file = "";         // file name to write

extern void stop_time  (int rank, int batch_size, int nb_batchs);
extern void start_time ();


void open (const char* filename) {
  adios_open (&adios_handle, "report", filename, "w", comm);
}

void write (int* buffer){
  adios_write (adios_handle, "global_size", (void*) &total_size);
  adios_write (adios_handle, "batch_size",  (void*) &batch_size);
  int offset = batch_size * rank;
  for (int i = 0 ; i < nb_batchs; i++) {
    adios_write (adios_handle, "offset", (void*) &offset);
    adios_write (adios_handle, "data",  buffer);
    offset += batch_size * nb_ranks;
    buffer += batch_size;
  }
}

void close () {
  adios_close(adios_handle);
}

void initAdios (const char* method, int max_buffer_size) {
  adios_init_noxml          ( comm);
  // adios_set_max_buffer_size ( max_buffer_size);
  adios_declare_group       ( &adios_group_id,"report", "", adios_stat_no);
  adios_select_method       ( adios_group_id, method,    "verbose=0", "");
  adios_define_var          ( adios_group_id, "global_size",  "", adios_integer, "", "", "");
  adios_define_var          ( adios_group_id, "batch_size",   "", adios_integer, "", "", "");
  for (int i = 0; i < nb_batchs; i++) {
    offset_ids.push_back  ( adios_define_var ( adios_group_id, "offset", "", adios_integer, "", "", ""));
    data_ids.push_back    ( adios_define_var ( adios_group_id, "data",   "", adios_integer, "batch_size", "global_size", "offset"));
  }
}

void initBuffer (int*& buffer) {
  buffer = (int*) malloc (batch_size * nb_batchs * sizeof(int));
  for (int i = 0 ; i < nb_batchs; i++) {
    for (int j = 0; j < batch_size; j++)
      buffer[i*batch_size+j] = rank;
  }
}

int main (int argc, char** argv) {
  if (argc != 6) {
    std::cerr << "usage: " << argv[0] << " <nb bytes per write per rank> <ADIOS buffer size> <transport method> <nb writes per rank> <output filename>" << std::endl;
    return -1;
  }

  total_size      = 0;
  buffer          = NULL;
  batch_size      = atoi(argv[1]);
  max_buffer_size = atoi(argv[2]);
  method          = argv[3];
  nb_batchs       = atoi(argv[4]);
  out_file        = argv[5];

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(comm, &rank);
  MPI_Comm_size(comm, &nb_ranks);
  initAdios(method, max_buffer_size);

  total_size = batch_size * nb_ranks * nb_batchs;
  initBuffer (buffer);
  open (out_file);

  MPI_Barrier (MPI_COMM_WORLD);
  start_time();

  write (buffer);
  MPI_Barrier (MPI_COMM_WORLD);
  close();

  stop_time(rank, batch_size, nb_batchs);
  adios_finalize(rank);

  MPI_Finalize();
  return 0;
}
