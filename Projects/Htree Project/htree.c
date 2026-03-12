/*
 * Ryan Bae RMB210005
 * Keaton Ylanan KKY210002
*/

#include <stdio.h>     
#include <stdlib.h>   
#include <stdint.h>  
#include <inttypes.h>  
#include <errno.h>     // for EINTR
#include <fcntl.h>     
#include <unistd.h>    
#include <sys/types.h>
#include <sys/stat.h>
#include "common.h"
#include "common_threads.h"
#include <sys/mman.h>
#include <string.h>

typedef struct ThreadArgs {
  uint8_t* filebuf; // Pointer to file buffer
  uint32_t 	      max_threads; // Max number of threads (command line argument)
  uint32_t	      num_blocks; // Number of blocks
  uint32_t	      num_parent; // Parent's number in tree
  uint32_t         num_self; // Current number in tree
  
} Args;

// Print out the usage of the program and exit.
void Usage(char*);
uint32_t jenkins_one_at_a_time_hash(const uint8_t* , uint64_t );
void build_arguments(Args*, uint8_t*, uint32_t, uint32_t, uint32_t, uint32_t); 
void* thread_function(void* args);
// block size
#define BSIZE 4096

int 
main(int argc, char** argv) 
{
  int32_t fd;
  uint32_t nblocks;
  
  // input checking 
  if (argc != 3)
    Usage(argv[0]);

  // open input file
  fd = open(argv[1], O_RDWR);
  if (fd == -1) {
    perror("open failed");
    exit(EXIT_FAILURE);
  }
  // use fstat to get file size
 struct stat statbuf;
   if (fstat(fd, &statbuf) != 0)
     {
     	printf("fstat error!");
	  }
 // calculate nblocks
uint32_t max_threads = atoi(argv[2]);
if (max_threads <= 0) {
  printf("Error: Threads must be a postive integer\n");
  return EXIT_FAILURE;
}
nblocks = statbuf.st_size / BSIZE;

printf("num Threads = %" PRIu32 "\n", max_threads);

  printf("Blocks per Thread = %u\n", nblocks / max_threads);

  double start = GetTime();

 uint8_t *filebuf = (uint8_t*) malloc(nblocks * BSIZE);
 filebuf = mmap(NULL, statbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
 if (filebuf == NULL)
 {
      printf("mmap error!");
 }

// Build structure to use as arguments in thread function
Args *args = (Args*) malloc(sizeof(Args));
build_arguments(args, filebuf, max_threads, nblocks, 0, 0);

// Create root thread
uint32_t *final_hash;
pthread_t root;
Pthread_create(&root, NULL, thread_function, args);
Pthread_join(root, (void**) &final_hash);
printf("hash value = %" PRIu32 "\n", *final_hash);

  double end = GetTime();
  printf("time taken = %f \n", (end - start));
  close(fd);
  return EXIT_SUCCESS;
}

//takes in args pointer and sets values for use in child threads
void build_arguments (Args* a, uint8_t* f, uint32_t m, uint32_t n, uint32_t i, uint32_t j)
{
  a->filebuf = f;
  a->max_threads = m;
  a->num_blocks = n;
  a->num_parent = i;
  a->num_self = j;    //current thread's number
  return;
}

//executed by threads
void* thread_function(void* arg)
{
 pthread_t left_child, right_child;
 Args *args = (Args*) arg;
 
 //arguments passed to threads
 Args *left_args = (Args*) malloc(sizeof(Args));
 Args *right_args = (Args*) malloc(sizeof(Args));

 //calculating child threads' numbers
 uint32_t *left_num = (uint32_t*) malloc(sizeof(uint32_t));
 *left_num = 2*(args->num_self) + 1;
 uint32_t *right_num = (uint32_t*) malloc(sizeof(uint32_t));
 *right_num = *left_num + 1;
 
 
 // allocate return values
 uint32_t *ret_left = (uint32_t*) malloc(sizeof(uint32_t));
 uint32_t *ret_right = (uint32_t*) malloc(sizeof(uint32_t));
 uint32_t *ret_self = (uint32_t*) malloc(sizeof(uint32_t));
 
 //do individual hash
 uint64_t blocks_per_thread = (args->num_blocks) / (args->max_threads);
 uint8_t *key = (args->filebuf) + ((uint64_t)args->num_self * blocks_per_thread * BSIZE);
 uint32_t hash_val = jenkins_one_at_a_time_hash(key, blocks_per_thread * BSIZE);

 // Make left thread (and right thread) if needed
 if (*left_num < args->max_threads)
 {
  
   build_arguments(left_args, args->filebuf, args->max_threads, args->num_blocks, args->num_self, *left_num);
   Pthread_create(&left_child, NULL, thread_function, left_args);
   
   // Make right thread if needed
   if (*right_num < args->max_threads)
   {
     build_arguments(right_args, args->filebuf, args->max_threads, args->num_blocks, args->num_self, *right_num);
     Pthread_create(&right_child, NULL, thread_function, right_args);
    
   }
  
  else
   {
     *right_num = args->max_threads;    	//No right child
   }
 }
 else
 {
   *left_num = args->max_threads;         //No children
   *right_num = args->max_threads;
 }

 if (*left_num != args->max_threads)
 {
   Pthread_join(left_child, (void**) &ret_left);
   
   //Concatenate self and left child
   char self_str[128];
   sprintf(self_str, "%u", hash_val);
   char cat_str[128];
   sprintf(cat_str, "%u", *ret_left);
   strcat(self_str, cat_str);

   free(left_num);
  
   if (*right_num != args->max_threads)
   {
     Pthread_join(right_child, (void**) &ret_right);
     
     //Concatenate self and right child
     sprintf(cat_str, "%u", *ret_right);
     strcat(self_str, cat_str);
     
     free(right_num);
     
   }

   //hash after concatenate
   hash_val = jenkins_one_at_a_time_hash((uint8_t*)self_str, strlen(self_str));
 }

 //set return value
 *ret_self = hash_val;
 
 free(left_args);
 free(right_args);
 pthread_exit(ret_self);
 return NULL;

}

uint32_t 
jenkins_one_at_a_time_hash(const uint8_t* key, uint64_t length) 
{
  uint64_t i = 0;
  uint32_t hash = 0;

  while (i != length) {
    hash += key[i++];
    hash += hash << 10;
    hash ^= hash >> 6;
  }
  hash += hash << 3;
  hash ^= hash >> 11;
  hash += hash << 15;
  return hash;
}

void 
Usage(char* s) 
{
  fprintf(stderr, "Usage: %s filename num_threads \n", s);
  exit(EXIT_FAILURE);
}
