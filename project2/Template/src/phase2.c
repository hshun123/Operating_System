#include "phase2.h"

#include <stdio.h>

#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <zconf.h>
 #include <sys/types.h>
 #include <errno.h>
// You are free to use your own logic. The following points are just for getting started
/* 	Map Function
	1)	Each mapper selects a Mapper_i.txt to work with
	2)	Creates a list of letter, wordcount from the text files found in the Mapper_i.txt
	3)	Send the list to Reducer via pipes with proper closing of ends
*/

//char pathlines[1000][1000];
int count[26];
int ok = 0; //ok is used to count the number of lines read in each process_file
char letters[26] = {'a','b','c','d','e','f','g',
'h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};

char upper_letters[26] = {'A','B','C','D','E','F','G',
'H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};

//This funciton trims the newLine \n
void trimNewline(char * str)
{
    int index, i;

    index = -1;

    i = 0;
    while(str[i] != '\0')
    {
        if(str[i] != '\n')
        {
            index= i;
        }

        i++;
    }
    str[index + 1] = '\0';
}

//Parse the file and store each line in to path
//From project 1
char** process_file(char *fname)
{
	FILE* fp = fopen(fname, "r");

	char line[1000];
	int i = 0;
  char **pathlines;

  //dynamically allocating multi array
    pathlines = malloc (1000 * sizeof(char*));
    for(int j = 0; j < 1000; j++){
       pathlines[j] = malloc(1000 * sizeof(char));
    }
	if (!fp) {
		printf("Failed to open the file: **************%s*********** \n", fname);
    printf("errono: %d\n", errno);
    printf("Error: %s\n", strerror(errno));
		return NULL;
	}

	//Read the contents and store in lines
	while (fgets(line, 1000, fp)){
		strncpy(pathlines[i++], line, strlen(line) + 1);
    }
    //eric
    ok = i;
	fclose(fp);

	return pathlines;
}

//This function count the word per letter
void countletters(char* txtpath){

  //initializing the count array each time it is called
  //to make sure value overwritten
  for(int k = 0; k < 26; k++){
      count[k] = 0;
  }

    char**check = process_file(txtpath);

    int num_of_words = ok;

    for(int i = 0; i < num_of_words; i++){
        //trim the line with \n
        trimNewline(check[i]);
        //eric
        //check the first letter in the word
        for(int j = 0; j < 26; j++){
            if((check[i][0] == letters[j]) || (check[i][0] == upper_letters[j])){
                count[j]++;
            }
        }
    }
    
    free(check);

}

//This function calculate the sum of each element in arrays

int* sumcounts(int* nat, int* ad){

    int* sum = (int*)malloc(26* sizeof(int));

    for(int i = 0; i < 26; i++){
        sum[i] = (nat[i]) + (ad[i]);
    }

    return sum;
}

//This is the Map function which returns the list for each alphbet count
//each index in totalcount is corresponding to a-z alphabet
int* Map (char* filepath) {


    int*totalcount = (int*)malloc(26* sizeof(int));
    for (int i = 0; i < 26; i++)
      totalcount[i] = 0;

    char** paths = process_file(filepath);
    //num_of_txt is the total number of txt files inside each Mapper_i.txt
    int num_of_txt = ok;

    for(int i = 0; i < num_of_txt; i++){
      //trim newline \n
      trimNewline(paths[i]);

      countletters(paths[i]);
      totalcount = sumcounts(totalcount, count);
    }

    free(paths);

    for (int i = 0; i < 26; i++)
      //printf("From file %s: %d\n", filepath, totalcount[i]);
    return totalcount;

}
