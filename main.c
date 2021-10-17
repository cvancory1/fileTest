
/*
Chloe VanCory and Kalyn Howes
420 Project 1
10.17.21
*/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>

#define ROOT 0
#include "testingCheckWord.c"



typedef struct Users {
  char *  username;
  char *  id;
  char *  alg;
  char *  pwd;
} Users;

int main(int argc, char** argv) {
  
  // OLD VERSION 
  MPI_Init(&argc, &argv);
  MPI_Comm world = MPI_COMM_WORLD;

  char name[MPI_MAX_PROCESSOR_NAME];
  int worldSize, rank, nameLen;

  MPI_Comm_size(world, &worldSize);
  MPI_Comm_rank(world, &rank);
  // MPI_Get_processor_name(name, &nameLen); 

  // MPI_Init(&argc, &argv);
  // world = MPI_COMM_WORLD;


  // MPI_Comm_size(world, &worldSize);
  // MPI_Comm_rank(world, &rank);
  // MPI_Get_processor_name(name, &nameLen); 

  
  
  // MPI_Comm world;
  // int worldSize, rank;
  // char name[MPI_MAX_PROCESSOR_NAME];
  // int nameLen;
  // MPI_File fh;

  // // open file
  MPI_File_open(
    world,                             // comm
    "crackedPasswords.txt",            // filename
    MPI_MODE_CREATE | MPI_MODE_RDWR,   // mode
    MPI_INFO_NULL,                     // info structure
    &fh                                // file handle
  );

  /*
    read and parse the shadow txt file to get the username,id,salt, password
  */

  // malloc the actual pointers then malloc each of the arrays 
  FILE * shadowPtr;
  shadowPtr = fopen ("testShadow.txt", "r");
  char * line= malloc(255* sizeof(char));
  int numUsers = 4;
  Users shadowUsers[numUsers]; // 48 bytes 
  int index = 0;

  
  /*  
    READING THE WORDS.TXT file 
  */

  int * sendcnt;
  if(rank == ROOT){
    sendcnt = malloc(worldSize * sizeof(int));
      for(int i =0; i< worldSize ;i++){
        sendcnt[i] =0;
      // printf("sendcnt[%d]=%d\n",i ,sendcnt[i]);
    }
  }

  // int WORDCOUNT =100;
  int WORDCOUNT = 235888;
  int * offset;
  int fd;
  fd= open("testWords.txt",O_RDONLY);
  if (rank == ROOT) {
    int numBytes;
    int index =0;
    int lineCount=0;
    int buffersize=1;
    char buff[buffersize];

    if(fd !=-1 ){
      while((numBytes=read(fd,buff,buffersize))>0){
        // printf("%c",buff[0]);
        sendcnt[index]++;
        if(buff[0] == '\n'){
          lineCount ++;
          // printf("linecount= =%d\n",lineCount);

          if(lineCount == (WORDCOUNT / worldSize)){
            // printf("here - line count =%d\n",lineCount);
            // printf("sendcnt[%d]=%d\n",index ,sendcnt[index]);
            lineCount = 0;
            index++;
          }
        }
        // printf(" %c",buff[0]);
      }
    } else {
      printf("Error opening the testwords.txt\n");
    }
  }

  // printf("checkpt 2\n ");

  /* ROOT Calculated how much every node needs to read into their local dictionary 
      use scatter to send this amount 
  */

  if (rank != ROOT) {
    sendcnt = malloc(worldSize * sizeof(int));
  }

  MPI_Bcast(sendcnt, worldSize, MPI_INT, ROOT, world);
  // printf("rank = %d sendcnt= %d\n",rank ,sendcnt[rank]);

  /*
    calcs the displacement for each proccessor to lseek ( move file pointer ) to a specific place
    in the file
  */

  int * displc = malloc(worldSize * sizeof(int));
  displc[0] = 0;
  for(int i =1; i < worldSize ; i++){
    displc[i] = sendcnt[i-1] + displc[i-1];
    // if(rank ==0)
      // printf("i =%d displc= %d\n",i, displc[i]);
  }

  // printf("checkpt 3 ");

  /*
     use lseek to position file pointers and then read into then in a portion of the words.txt into a local dictionary 
  */ 
  char * localDict = malloc ( sendcnt[rank]+1 * sizeof(char));
  lseek(fd ,displc[rank], SEEK_SET);
  int numRead = read(fd , localDict ,sendcnt[rank] );
  localDict[sendcnt[rank]+1]= '\0'; // places the NULL term @ the end
  //printf(" rank = %d \nstring= %sEND\n",rank ,localDict);


  //  ERROR CHECKING
  // if(rank == 3){
    // printf("rank = %d \nstring= %sEND\n",rank ,localDict);
  // printf("Rank = %d numRead = %d   MALLOC  = %d  strlen(localDict)  =%lu\n",rank, numRead ,sendcnt[rank] , strlen(localDict ) );
  // }

  // if(rank == ROOT){
  //     for(int i =0; i < worldSize ;i++){
  //   // printf("rank = %d \nstring= %c%c%c END\n",rank ,localDict[displc[i-1]], localDict[displc[i]] , localDict[displc[i+1]]);
  //     }
    
  // }

  // printf("checkpt 4 ");

  // /*
  //   set up "shared array " that will indicate whether we have found a users password or not
  // */

  int * usrPwd = malloc(numUsers * sizeof(int)); 
  for(int i=0; i< numUsers ;i++){
    usrPwd[i] = 0;
  }
  int pswdIndex = 0; // index of the current users paswds all nodes are trying to find 

  printf("checkpt 5 ");

  // /*
  //  *ALL NODES - Parse every words from the nodes local dictionary to crpyt and test ater 
  // * Do this for every username we have
  //
  // */


  fscanf(shadowPtr,"%s", line );
  // printf("line=%s\n", line);
  // printf("len of line: %d\n", strlen(line));

  char * username = strtok(line, ":" );
  char * pwd = strtok(NULL, "\n" );

  // // ------ DO CHECK WORD -------
    int test;
  while( pswdIndex != numUsers-1 ){
    int check;
    char *currentWord = malloc(100);
    memset(currentWord, 0, 100);
    test = sscanf(localDict, "%s\n", currentWord);
    printf("Current word: %s\n", currentWord);
    check = checkWord(pwd, currentWord);
    // printf("check %s for word  %s\n", check, currentWord);

    int offset = strlen(currentWord) + 1;
    int localDict_len= strlen(localDict);
    while(test != EOF && offset< localDict_len){
      test = sscanf(localDict + offset, "%s\n", currentWord);
      offset += strlen(currentWord) + 1;
      printf("Rank %d checking: %s\n", rank, currentWord);
      check = checkWord(pwd, currentWord); 
      
    }
  
    pswdIndex++;
    fscanf(shadowPtr,"%s", line );
    username = strtok(line, ":" );
    pwd = strtok(NULL, "\n" );
    printf("pwd=%s\n", pwd);
    printf("username=%s\n", username);
  }

  MPI_File_close(&fh);
  MPI_Finalize();
  return 0;
}
