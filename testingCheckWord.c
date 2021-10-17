#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <crypt.h> //- mac doesn't know what this is

#define _DEFAULT_SOURCE
// MPI_Comm world;
// int worldSize, rank;
// char name[MPI_MAX_PROCESSOR_NAME];
// int nameLen;
// MPI_File fh;
int fileIndex =0;

// crypt function - returns char* of crypted input word
char* crypting(char *password) {
  // printf("In crypt function -- pasword to be encrypted is %s\n", password);
  char* result = crypt(password, "$1$ab$"); // hardcode the salt - on Mac salt is second!!
  // printf("In crypt function - your encrypted password is: %s\n", result);
  return result;
}


// checks if hashed word is same as hashed password
// returns 1 if they are the same and 0 if not
int compare(char *password, char *crypted, char *nonhashedpass) {
  // int ret;
  // printf("The password is: %s\n", password);
  // printf("The crypted is: %s\n", crypted);
  // printf("The nonhashedpass is: %s\n", nonhashedpass);
  // if(strcmp(nonhashedpass, "abase11")==0){
  //   printf("The nonhashedpass is: %s\n", nonhashedpass);
  // }

  if (strcmp(password, crypted) == 0) {
    puts("****** PASSWORD HAS BEEN CRACKED !******");
    printf("The password is: %s\n", nonhashedpass);
    printf("The crypted version is: %s\n", crypted);
                  // static int testingAmount =0;
                  // testingAmount++;
                  // if(testingAmount % 100000 == 0){
    // int length = strlen(nonhashedpass);
    // char * newLine="\n";
    // int end_length =1; // new line

    // char * teststr = malloc(length + end_length);

    // sprintf(teststr, "%s\n", nonhashedpass ); // puts null terminator after inserting word
    // static int offset =0;
    // MPI_File_write_at(
    //       fh,                // file handle
    //       offset,        // offset
    //       teststr,              // buf to be written
    //       length+end_length,                // size+/
    //       MPI_CHAR,          // type
    //       MPI_STATUS_IGNORE  // status
    //     );
      
    //   offset+=length + end_length;
       
    //   fileIndex++; //move to the next line in the file after we found the first pwd
      // printf("file index = %d word printed = %s offset=%d", fileIndex,teststr, offset );
      // printf(" word printed = %s offset=%d len = %d newLen =%d\n",teststr, offset,length, strlen(teststr) );
  }
}

// ------------------------------------ PREFIX CHECKS ------------------------------------
// checks password to dictionary word with prefixes 0-9
// use strcat to add orefix string to the word buf
int onesCheckPrefix(char *password, char *word) {
  // printf("password %s word= %s  ",password, word);

  int wordlen = strlen(word); // store dict word length
  int buflen = (2 + wordlen * sizeof(char)); // buffer length
  char *prefixword = malloc(2 + wordlen * sizeof(char)); // buffer to hold the dictionary word and the prefix/suffix
 
  char *prefix = malloc(2 * sizeof(char));  // one larger than prefix size for null
  prefix[0] = '0';
  prefix[1] = '\0';
  sprintf(prefixword, "%s%s", prefix ,word); // puts null terminator after inserting word
  // printf("prefixword = %s ", word);

  for (int i = 0; i < 10; i++) {
    //printf("(ones) prefixword: %s\n", prefixword);
    char *crypted = crypting(prefixword); // crypt
    int same = compare(password, crypted, prefixword); // compare
    if (same == 1) {
      return 1; 
    }
    prefixword[0]++;
  }
  //free(prefixword);
  return 0;
}

// checks password to dictionary word with prefixes 10-99
int tensCheckPrefix(char *password, char *word) {
  int wordlen = strlen(word); // store dict word length
  // int buflen = (2 + wordlen * sizeof(char)); // buffer length
  char *prefixword = malloc(3 + wordlen * sizeof(char)); // buffer to hold the dictionary word and the prefix/suffix
 
  char *prefix = malloc(3 * sizeof(char));  // one larger than prefix size for null
  prefix[0] = '0';
  prefix[1] = '0';
  prefix[2] = '\0';
  sprintf(prefixword, "%s%s", prefix ,word); // puts null terminator after inserting word
  // printf("prefixword = %s ", word);

  for (int i = 9; i <= 100; i++) {
    //printf("(tens) prefixword = %s\n", prefixword);
    char *crypted = crypting(prefixword); // crypt
    int same = compare(password, crypted, prefixword); // compare
    if (same == 1){
      return 1; 
    }
      
    prefixword[1]++;
    if (i % 10 == 0) {
      prefixword[0]++; // tens 
      prefixword[1]= '0' ; // ones 
    }
  }
  //free(prefixword);

  return 0;
}

// checks password to dictionary word with prefixes 100-999
int hundsCheckPrefix(char *password, char *word) {
  int wordlen = strlen(word); // store dict word length
  int buflen = (4 + wordlen * sizeof(char)); // buffer length
  char *prefixword = malloc(4 + wordlen * sizeof(char)); // buffer to hold the dictionary word and the prefix/suffix
 
  char *prefix = malloc(4 * sizeof(char));  // one larger than prefix size for null
  prefix[0] = '0';
  prefix[1] = '0';
  prefix[2] = '0';
  prefix[3] = '\0';
  sprintf(prefixword, "%s%s", prefix ,word); // puts null terminator after inserting word
  // printf("prefixword = %s ", word);
  
  for(int i = 0; i < 10; i++) {
    for(int j = 0; j < 10; j++) {
      for(int k = 0; k < 10; k++){ 
        //printf("(hundreds) prefixword = %s\n", prefixword);
        char *crypted = crypting(prefixword); // crypt
        //printf("crypted (hundreds): %s\n", crypted);
        int same = compare(prefixword,crypted , password); // compare
        //printf("SAME: %d", same);
        if (same == 1) { // compare returns 1 if found, not 0
          return 1; 
        }
        
        prefixword[2]++; // ones place 

        // hacking - stops the loop early 
        if (prefixword[2]==':' && prefixword[1]=='9' && prefixword[0]=='9') {
          return 0;
        }
      
      }
      prefixword[2]= '0'; // ones
      prefixword[1]++; // tens
    }
    prefixword[2]= '0'; // ones
    prefixword[1]='0';// tens
    prefixword[0]++; // hundreads
  }
  //free(prefixword);

  return 0;
}

// checks password to dictionary word with prefixes 1000-9999
int thousCheckPrefix (char *password, char *word) {
  int wordlen = strlen(word); // store dict word length
  // int buflen = (5 + wordlen * sizeof(char)); // buffer length
  char *prefixword = malloc(5 + wordlen * sizeof(char)); // buffer to hold the dictionary word and the prefix/suffix
 
  char *prefix = malloc(5 * sizeof(char));  // one larger than prefix size for null
  prefix[0] = '0';
  prefix[1] = '0';
  prefix[2] = '0';
  prefix[3] = '0';
  prefix[4] = '\0';
  sprintf(prefixword, "%s%s", prefix ,word); // puts null terminator after inserting word
  // printf("prefixword = %s ", word); 

  for(int i = 0; i < 10; i++) {
    for(int j = 0; j < 10; j++) {
      for(int k = 0; k < 10; k++) { 
        for (int l = 0; l < 10; l++) {
          //printf("(thousands) prefixword = %s\n", prefixword);
          char *crypted = crypting(prefixword); // crypt
          //printf("crypted (hundreds): %s\n", crypted);
          int same = compare(password, crypted, prefixword); // compare
          //printf("SAME: %d\n", same);
          if (same == 1) {
            return 1; 
          }
          
          prefixword[3]++; // ones place 

          // hacking - stops the loop early 
          if(prefixword[0]==':' && prefixword[1]=='0' && prefixword[2]=='0' && prefixword[3]=='0') {
            return 0;
          }
        }
        prefixword[3] = '0'; // ones
        prefixword[2]++; //tens
      }
      prefixword[3]= '0'; // ones
      prefixword[2]= '0';// tens
      prefixword[1]++; // hundreads
    }
    prefixword[3] = '0'; // ones
    prefixword[2]= '0'; // tens
    prefixword[1]= '0';// hundreds
    prefixword[0]++; // thousands
  }
  //free(prefixword);
  return 0;
}


// ------------------------------------ SUFFIX CHECKS ------------------------------------
// checks password to dictionary word with suffixes 0-10
int onesCheck(char *password, char *word) {
  int wordlen = strlen(word); // store dict word length
  int buflen = (2 + wordlen * sizeof(char)); // buffer length = 'tens' place + word length
  char *buf = malloc(buflen); // buffer to hold the dictionary word and the prefix/suffix
  sprintf(buf, "%s", word); // puts null terminator after inserting word

  buf[wordlen] = '0';
  buf[wordlen+1] = '\0';
  for (int i = 0; i < 10; i++) {
    //printf("(ones) buf = %s\n", buf);
    char *crypted = crypting(buf); // crypt
    int same = compare(password, crypted, buf); // compare
    if (same == 1) {
      return 1; 
    }
    buf[wordlen]++;
  }
  //free(buf);
  return 0;
}

// checks password to dictionary word with suffixes 10-99
int tensCheck (char *password, char *word) {
  int wordlen = strlen(word); // store dict word length
  int buflen = (3 + wordlen * sizeof(char)); // buffer length = 'tens' place + word length
  char *buf = malloc(buflen); // buffer to hold the dictionary word and the prefix/suffix
  sprintf(buf, "%s", word); // puts null terminator after inserting word

  buf[wordlen] = '0';
  buf[wordlen + 1] = '0'; 
  buf[wordlen+2] = '\0';

  for (int i = 9; i <= 100; i++) {
      //printf("(tens) buf = %s\n", buf);
      char *crypted = crypting(buf); // crypt
      int same = compare(password, crypted, buf); // compare
      if (same == 1){
        return 1; 
      }
      
      buf[wordlen+1]++;
      if (i % 10 == 0) {
        buf[wordlen]++; // tens 
        buf[wordlen+1]= '0' ; // ones 
      }
  }
    //free(buf);
  return 0;
}

// checks password to dictionary word with suffixes 100-999
int hundredsCheck (char *password, char *word) {
  int wordlen = strlen(word); // store dict word length
  int buflen = (4 + wordlen * sizeof(char)); // buffer length = 'hundreds' place + word length
  char *buf = malloc(buflen); // buffer to hold the dictionary word and the prefix/suffix
  sprintf(buf, "%s", word); // puts null terminator after inserting word

  buf[wordlen] = '1';
  buf[wordlen+1] = '0'; 
  buf[wordlen+2] = '0'; 
  buf[wordlen+3] = '\0'; 

  for(int i = 0; i < 10; i++) {
    for(int j = 0; j < 10; j++) {
      for(int k = 0; k < 10; k++){ 
        //printf("(hundreds) buf = %s\n", buf);
        char *crypted = crypting(buf); // crypt
        //printf("crypted buf (hundreds): %s\n", crypted);
        int same = compare(password, crypted, buf); // compare
        //printf("SAME: %d", same);
        if (same == 1) { // compare returns 1 if found, not 0
          return 1; 
        }
        
        buf[wordlen+2]++; // ones place 

        // hacking - stops the loop early 
        if (buf[wordlen+2]==':' && buf[wordlen+1]=='9' && buf[wordlen]=='9') {
          return 0;
        }
      
      }
      buf[wordlen+2]= '0'; // ones
      buf[wordlen+1]++; // tens
    }
    buf[wordlen+2]= '0'; // ones
    buf[wordlen+1]='0';// tens
    buf[wordlen]++; // hundreads
  }
  //free(buf);
  return 0;
}

// checks password to dictionary word with suffixes 1000-9999
int thousandsCheck (char *password, char *word) {
  int wordlen = strlen(word); // store dict word length
  int buflen = (5 + wordlen * sizeof(char)); // buffer length = 'hundreds' place + word length
  char *buf = malloc(buflen); // buffer to hold the dictionary word and the prefix/suffix
  sprintf(buf, "%s", word); // puts null terminator after inserting word

  buf[wordlen] = '1';
  buf[wordlen+1] = '0'; 
  buf[wordlen+2] = '0'; 
  buf[wordlen+3] = '0'; 
  buf[wordlen+4] = '\0'; 

  for(int i = 0; i < 9; i++) {
    for(int j = 0; j < 10; j++) {
      for(int k = 0; k < 10; k++) { 
        for (int l = 0; l < 10; l++) {
          //printf("(thousands) buf = %s\n", buf);
          char *crypted = crypting(buf); // crypt
          int same = compare(password, crypted, buf); // compare
          if (same == 1){
            return 1; 
          }
          
          buf[wordlen+3]++; // ones place 

          // hacking - stops the loop early 
          if(buf[wordlen]==':' && buf[wordlen+1]=='0' && buf[wordlen+2]=='0' && buf[wordlen+3]=='0') {
            return 0;
          }
        }
        buf[wordlen+3] = '0'; // ones
        buf[wordlen+2]++; //tens
      }
      buf[wordlen+3]= '0'; // ones
      buf[wordlen+2]= '0';// tens
      buf[wordlen+1]++; // hundreads
    }
    buf[wordlen+3] = '0'; // ones
    buf[wordlen+2]= '0'; // tens
    buf[wordlen+1]= '0';// hundreds
    buf[wordlen]++; // thousands
  }
  //free(buf);
  return 0;
}

// checks dictionary word for each prefix & suffix size
int checkWord(char *password , char *word) {
  // printf("IN CHECKWORD");
  // printf("password %s word= %s", password, word);

  if (onesCheckPrefix(password, word) == 1)
    return 1;
  if (tensCheckPrefix(password, word) == 1)
    return 1;
  if (hundsCheckPrefix(password, word) == 1)
    return 1;
  if (thousCheckPrefix(password, word) == 1)
    return 1;
  if (onesCheck(password, word) == 1)
    return 1;
  if (tensCheck(password, word) == 1)
    return 1;
  if (hundredsCheck(password, word) == 1)
    return 1;
  if (thousandsCheck(password, word) == 1)
    return 1;
  
  return 0; // false
}

// int main () {
//   char *word = "aardvark";
//   //char *testing = "aardvark123";

//   // suffix checking
//   char *password = "$1$ab$koTc2TaVJm9d6HbDymlZO/"; // aardvark123
//   //char *password = "$1$ab$EN4HdMyFfvk9VYCCEIIJs0"; //abandon9365
//   //char *password = "$1$ab$nceComwr03PR64UjgZOTM/"; // abashed8
//   //char *password = "$1$ab$6mj6pLEZbyGKA9.J6InfW0"; // abase11

//   // prefix checking
//   //char *password = "$1$ab$hNU8w62rGJrVyGwCWeCXq/"; // 4abashed
//   //char *password = "$1$ab$AVH5ZeCRu0OFiaX7eM.2a0"; // 34abase
//   //char *password = "$1$ab$i05qx2WzHLm2O92sVdTwm0"; // 997abandon
//   //char *password = "$1$ab$2uYJbJ8oxDCDv9Haijjp9/"; // 8569aardvark

//   checkWord(password, word);

//   return 0;
// }
