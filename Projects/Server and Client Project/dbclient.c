/*
 *
 * Ryan Bae RMB210005
 * Keaton Ylanan KKY210002
 * 
 */

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <inttypes.h>
#include <ctype.h>
#include "msg.h"

// Used for overflow checking
#define MAX_ID_DIGITS 10

void Usage(char *progname);

int LookupName(char *name,
                unsigned short port,
                struct sockaddr_storage *ret_addr,
                size_t *ret_addrlen);

int Connect(const struct sockaddr_storage *addr,
             const size_t addrlen,
             int *ret_fd);

int 
main(int argc, char **argv) {
  if (argc != 3) {
    Usage(argv[0]);
  }

  unsigned short port = 0;
  if (sscanf(argv[2], "%hu", &port) != 1) {
    Usage(argv[0]);
  }

  // Get an appropriate sockaddr structure.
  struct sockaddr_storage addr;
  size_t addrlen;
  if (!LookupName(argv[1], port, &addr, &addrlen)) {
    Usage(argv[0]);
  }

  // Connect to the remote host.
  int socket_fd;
  if (!Connect(&addr, addrlen, &socket_fd)) {
    Usage(argv[0]);
  }
  
  // Buffers and size_t variables for getline 
  char *name_buf = NULL;
  size_t name_size = 0;
  char *id_buf = NULL;
  size_t id_size = 0;

  // Loop until user decides to quit
  while (1)
  {
  
  // User chooses 1 for put, 2 for get, 0 for quit
  int user_choice = -1;
 
  // Repeat until a valid choice is given
  do
    {
        // Prompt user
        printf("Enter your choice (1 to put, 2 to get, 0 to quit): ");
  
        // getline and null terminate the string
        char *choice_buf;
        size_t choice_size = 0;
        getline(&choice_buf, &choice_size, stdin);
        choice_buf[strlen(choice_buf) - 1 ] = '\0';
        
	// Check for valid input
        if (strlen(choice_buf) > 1 || isdigit(choice_buf[0]) == 0)
        {
            user_choice = -1;
        }
        
	// Convert string to integer
        else
        {
            user_choice = atoi(choice_buf);
            
	    // Check if the user choice is valid
	    if (user_choice > 2 || user_choice < 0)
            {
                user_choice = -1;
            }
        }
     	
	// Print error message if invalid input
        if (user_choice == -1)
        {
            printf("Error! Invalid choice.\n");
        }
            
    } while (user_choice == -1);    
  
  
  // If user choice is 0 then quit
  if (user_choice == 0)
  {
    close(socket_fd);
    exit(EXIT_SUCCESS);
  }
  
  // If choice is not 0 then allocate memory for structs to send the message
  struct msg *m = (struct msg*) malloc(sizeof(struct msg));
  struct record *rec = (struct record*) malloc(sizeof(struct record));
  
  
  // PUT
  if (user_choice == 1)
  {
    // Repeat until a valid name is given
    do {
      // Promp user input
      printf("Enter name: ");
      
      // Get input from user and add null terminator
      getline(&name_buf, &name_size, stdin);    
      name_buf[strlen(name_buf) -1] = '\0';
      
      // Check if input is valid
      if (strlen(name_buf) < 1||strlen(name_buf) > MAX_NAME_LENGTH)
      {
        printf("Error! Invalid name\n");
	      name_buf = NULL;
      }
      	
    } while (name_buf == NULL);

    // Copy input to the record
    strncpy(rec->name, name_buf, strlen(name_buf));

       
    // Repeat until valid id is given
    do {
    
      // Prompt user input
      printf("Enter id: ");
      
      // Get input from user and add null terminator
      getline(&id_buf, &id_size, stdin);
      id_buf[strlen(id_buf) -1] = '\0';

      // Check if input is valid
      /*
       *  if the first character is not a digit then it is invalid (no negative id is allowed)
       *  if the length of the string is larger than the maximum decimal digits a int32_t variable can store it is invalid
       *  if the atoii() function returns 0, but the length of the string is more than 1, then it is invalid (very large number that causes overflow)
       */
      if (isdigit(id_buf[0]) == 0 || strlen(id_buf) > MAX_ID_DIGITS || (atoi(id_buf) == 0 && strlen(id_buf) > 1))
      {
	       printf("Error! Invalid id.\n");
	       id_buf = NULL;
      }
      
    } while (id_buf == NULL);

    // Store id in record struct
    rec->id = atoi(id_buf);
  
    // Store record and message type in message struct
    m->type = user_choice;
    m->rd = *rec;
  }
  
  
  // GET
  else if (user_choice == 2)
  {
  
    // Repeat until valid id is given
    do {
      // Prompt user input
      printf("Enter id: ");
      
      // Get user input and add null terminator
      getline(&id_buf, &id_size, stdin);
      id_buf[strlen(id_buf) -1] = '\0';

      // Check if valid input is given
      if (isdigit(id_buf[0]) == 0 || strlen(id_buf) > MAX_ID_DIGITS || (atoi(id_buf) == 0 && strlen(id_buf) > 1))
      {
	       printf("Error! Invalid id.\n");
	       id_buf = NULL;
      }
      
    } while (id_buf == NULL);

    // Store id in record struct
    rec->id = atoi(id_buf);
  
  
    // Store record and message type in message struct
    m->type = user_choice;
    m->rd = *rec;
    }
  
    // If no valid choice is given, repeat
    else
    {
      printf("Error! Invalid choice.\n");
      continue;
    }  
   
    // Store number of bytes written to check for errors
    int res;
  
    // Write the message to the socket
    res = write(socket_fd, m, sizeof(struct msg));
    
    
    // Check if the message could not be sent
    if (res < sizeof(struct msg))
    {
        printf("socket closed prematurely \n");
        close(socket_fd);
        return EXIT_FAILURE;
    }
    
    // Read from the socket and store the number of bytes read to check for errors
    res = read(socket_fd, m, sizeof(struct msg));
    
    
    // Check for read errors
    if (res < sizeof(struct msg)) {
      printf("socket closed prematurely \n");
      close(socket_fd);
      return EXIT_FAILURE;
    }
      
      
    if (res == -1) {
      printf("socket read failure \n");
      close(socket_fd);
      return EXIT_FAILURE;
      }
  
      //printf("Number of bytes read: %d\n", res);
      //printf("Message type: %" PRIu8 "\n", minput->type);
      //printf("Record name: %s\n", minput->rd.name);
      //printf("Record id: %" PRIu32 "\n", minput->rd.id);
  
    // SUCCESS
    if (m->type == 4)
    {
      // Put success
    	if (user_choice == 1)
      	  printf("Put success.\n");
     
     // Get success print the record
     else
  	  {
  	    printf("name: %s\n", m->rd.name);
  	    printf("id: %" PRIu32 "\n", m->rd.id);
  
  	  }
    }
  
    // FAIL  
    else if (m->type == 5)
      {
        // Put fail
      	if (user_choice == 1)
        	  printf("Put fail.\n");
       
        // Get fail
  	    else if (user_choice == 2)
            printf("Get failed.\n");
      }
     
    // Invalid message 
    else
    {
      printf("Error! Invalid message.\n");
    }
    
  } // end of while loop
  
  // Clean up.
  close(socket_fd);
  return EXIT_SUCCESS;
}

void 
Usage(char *progname) {
  printf("usage: %s  hostname port \n", progname);
  exit(EXIT_FAILURE);
}

int 
LookupName(char *name,
                unsigned short port,
                struct sockaddr_storage *ret_addr,
                size_t *ret_addrlen) {
  struct addrinfo hints, *results;
  int retval;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  // Do the lookup by invoking getaddrinfo().
  if ((retval = getaddrinfo(name, NULL, &hints, &results)) != 0) {
    printf( "getaddrinfo failed: %s", gai_strerror(retval));
    return 0;
  }

  // Set the port in the first result.
  if (results->ai_family == AF_INET) {
    struct sockaddr_in *v4addr =
            (struct sockaddr_in *) (results->ai_addr);
    v4addr->sin_port = htons(port);
  } else if (results->ai_family == AF_INET6) {
    struct sockaddr_in6 *v6addr =
            (struct sockaddr_in6 *)(results->ai_addr);
    v6addr->sin6_port = htons(port);
  } else {
    printf("getaddrinfo failed to provide an IPv4 or IPv6 address \n");
    freeaddrinfo(results);
    return 0;
  }

  // Return the first result.
  assert(results != NULL);
  memcpy(ret_addr, results->ai_addr, results->ai_addrlen);
  *ret_addrlen = results->ai_addrlen;

  // Clean up.
  freeaddrinfo(results);
  return 1;
}

int 
Connect(const struct sockaddr_storage *addr,
             const size_t addrlen,
             int *ret_fd) {
  // Create the socket.
  int socket_fd = socket(addr->ss_family, SOCK_STREAM, 0);
  if (socket_fd == -1) {
    printf("socket() failed: %s", strerror(errno));
    return 0;
  }

  // Connect the socket to the remote host.
  int res = connect(socket_fd,
                    (const struct sockaddr *)(addr),
                    addrlen);
  if (res == -1) {
    printf("connect() failed: %s", strerror(errno));
    return 0;
  }

  *ret_fd = socket_fd;
  return 1;
}
