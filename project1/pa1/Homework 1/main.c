/*test machine: csel-kh4250-37
* date: 10/09/19
* name: Eric Hwang, [Mihoko Kawada]
* x500: hwang241, [kawad003]
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "graph.h"
#include "dfs_stack.h"
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

//Global variables
char **myargv;
target tg[MAX_TARGETS];
int numtokens;
int tgcount = 0;
int linecount = 0;
char delim[] = ": ";
const char ch = ':';
char space [] = "\t ";
int test;
char **myrecipe;
int current = 0;
char comma [] = ",";

//Parse the input makefile to determine targets, dependencies, and recipes
int process_file(char *fname)
{
	FILE* fp = fopen(fname, "r");
	char line[LINE_SIZE];
	int i = 0;

	if (!fp) {
		printf("Failed to open the file: %s \n", fname);
		return -1;
	}

	//Read the contents and store in lines
	while (fgets(line, LINE_SIZE, fp))
		strncpy(lines[i++], line, strlen(line));

	fclose(fp);

	return 0;
}

//parsing each line with this function and return the number of tokens for each line
int makeargv(const char *s, const char *delimiters, char ***argvp) {
   int error;
   int i;
   int numtokens;
   const char *snew;
   char *t;

   if ((s == NULL) || (delimiters == NULL) || (argvp == NULL)) {
      errno = EINVAL;
			printf("Fail to parse because of NULL");
      return -1;
   }
   *argvp = NULL;
   snew = s + strspn(s, delimiters);         /* snew is real start of string */
   if ((t = malloc(strlen(snew) + 1)) == NULL)
      return -1;
   strcpy(t, snew);
   numtokens = 0;
   if (strtok(t, delimiters) != NULL)     /* count the number of tokens in s */
      for (numtokens = 1; strtok(NULL, delimiters) != NULL; numtokens++) ;

                             /* create argument array for ptrs to the tokens */
   if ((*argvp = malloc((numtokens + 1)*sizeof(char *))) == NULL) {
      error = errno;
      free(t);
      errno = error;
      return -1;
   }
                        /* insert pointers to tokens into the argument array */
   if (numtokens == 0)
      free(t);
   else {
      strcpy(t, snew);
      **argvp = strtok(t, delimiters);
      for (i = 1; i < numtokens; i++)
          *((*argvp) + i) = strtok(NULL, delimiters);
    }
    *((*argvp) + numtokens) = NULL;             /* put in final NULL pointer */
    return numtokens;
}

//FInd the index of dependency store in  the target array
//Return -1 if a dependency is not in the target array
int Char_to_Target (char* depName, target* t, int tgcount){
			target temp;
			for(int i = 0 ; i <= tgcount; i ++){
				if (!strcmp(depName, t[i].name)){
					return i;
				}
			}
			return -1;
}

//return the index of fist dependency which has not been visited for this target
int UnvisitedIndex (target g, target* t, int tgcount){

	for (int i = 0; i < g.dep_count; i ++){
		if (Char_to_Target(g.depend[i], t, tgcount) != -1){
			if (t[Char_to_Target(g.depend[i], t, tgcount)].visited != 1){
				return Char_to_Target(g.depend[i], t, tgcount);
			}
		}
	}

	return -1;
}

//DFS search for -r
void DFS_dep(target* g, int tgcount){
	//target* w;
	int v = 0;
	top = NULL;
	push(v);
	g[v].visited = 1;

	while(!isEmpty()){

		int check = UnvisitedIndex(g[v], g, tgcount);

		if (check != -1){

			push(v);
			g[check].visited = 1;

			v = check;

		}
		else{
			for (int i = 0; i < g[v].recipe_count; i ++){
				printf("%s\n",g[v].recipe[i]);
			}
			if(UnvisitedIndex(g[v], g, tgcount) == -1){
      	v = pop();
			}
		}
	}
}

//DFS search for fork in order
//This function returns the array of integer which is the order of target to execute
int* DFS_dep_fork(target* g, int tgcount, int v){

	top = NULL;
	push(v);
	g[v].visited = 1;


  int* recipe = (int*)malloc (tgcount);

	while(!isEmpty()){
		//check if there is any recipes not visited
		int check = UnvisitedIndex(g[v], g, tgcount);

		if (check != -1){

			push(v);
			g[check].visited = 1;
			v = check;
		}
		else{
            recipe[current] = v;
            current++;
			if(UnvisitedIndex(g[v], g, tgcount) == -1){
                v = pop();
			}
		}
	}
	return recipe;
}

//This funciton trims the newLine \n in the tokens
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

//Store target information by throughing each lines
// extra credit cases are handled
void store_target_Extracredit () {

    for (int i = 0; i < MAX_LINES; i ++ ){
						if (! ((numtokens = makeargv(lines[i], delim, &myargv)) == -1)){
							//check if there is ":" in this line to figure out targets
							if (strchr(lines[i], ch) != NULL){
								tgcount++;
								if (linecount == 0 ){
									tgcount--;
								}

								//trim \n
                trimNewline(myargv[0]);
								tg[tgcount].name = myargv[0];

								if(!strcmp("\n", myargv[1])){
                	tg[tgcount].dep_count = 0;
								}else{
                  tg[tgcount].dep_count = numtokens - 1;
                }
								//store dependencies
								for (int j = 0; (j < numtokens - 1) && (strcmp(myargv[1], "\n")); j ++){

              		trimNewline(myargv[j + 1]);
                	tg[tgcount].depend[j] = myargv[j + 1];

								}
								//If there is a recipe
							}else if (!(numtokens == 1 && !strcmp("\n", myargv[0])) && (numtokens != 0)){

								trimNewline(lines[i]);

								//separate by comma
								char* ptr = strtok(lines[i], comma);
								//Multiple recipes parallel and sequential
								// if the recipe is separated by comma, it handles it using strtok
								if(ptr != NULL){
              		while( ptr != NULL){

                  	tg[tgcount].recipe_count++;
                    tg[tgcount].recipe[tg[tgcount].recipe_count - 1] = ptr;
                    ptr = strtok(NULL, comma);
                  }
                }else{
              		tg[tgcount].recipe_count++;
                  tg[tgcount].recipe[tg[tgcount].recipe_count - 1] = lines[i];
                  linecount++;
                }
							}else{
								linecount++;

							}
					}
			}
}

//Validate the input arguments, bullet proof the program
int main(int argc, char *argv[])
{

	int repcount = 0; // count the total amount of recipes in the file
	//$./mymake Makefile
	//Similarly account for -r flag
	if (argc == 2 && strncmp(argv[1], "-p", 2) && strncmp(argv[1], "-r", 2)) {
		//check if we can open the file or not
		if(!(process_file(argv[1]))) {

			store_target_Extracredit();

		//start fork and excute
		//This loop count the total number of recipes
		for(int i = 0; i <= tgcount; i++){
			repcount += tg[i].recipe_count;
		}

		//Assign the returned array of ordered target index into rep_list
		int* rep_list = DFS_dep_fork(tg, tgcount, 0);

		//This loop go through targets in order based on def_on_fork
		//Then excute each recipe in corresponding order
	 	for(int i = 0; i <= tgcount; i++){
			for(int d = 0 ; d < tg[*(rep_list + i)].recipe_count; d++){
					printf("Recipe Name: %s\n", tg[*(rep_list + i)].recipe[d]);
					pid_t pid = fork();
					//child process
				 	if(pid == 0){

						//parsing the recipe to get filename and arguments for execvp
				 		if (! ((test = makeargv(tg[*(rep_list + i)].recipe[d], space, &myrecipe)) == -1)){
							execvp(myrecipe[0], myrecipe);
							exit(0);
						}else{
							printf("Counld not parse the line\n");
					 	}
					}
					//parent process wait for child to finish
					else if (pid > 0){
						wait(NULL);
				 	}
				}
			}
		}
	}

	if (argc == 3) {
		//$./mymake Makefile target
		if (strncmp(argv[1], "-r", 2) && strncmp(argv[1], "-p", 2) && !process_file(argv[1])) {
			//TODO
			store_target_Extracredit();
			//index store the [target] index in target array
			int index;
      index = Char_to_Target(argv[2], tg, tgcount);

			//rep_list store the array which stores the index of targets in excution order
      int* rep_list = DFS_dep_fork(tg, tgcount, index);

			//start from set [target] print and excute corresponding recipes
      for(int i = 0; i < current; i++){
		   	for(int d = 0 ; d < tg[*(rep_list + i)].recipe_count; d++){
        	printf("Recipe Name: %s\n", tg[*(rep_list + i)].recipe[d]);
          	pid_t pid = fork();
						//child process
            if(pid == 0){

						//parsing the recipe to get filename and arguments for execvp
						if (! ((test = makeargv(tg[*(rep_list + i)].recipe[d], space, &myrecipe)) == -1)){
							execvp(myrecipe[0], myrecipe);
							exit(0);
						}else{
							printf("Counld not parse the line\n");
				 		}
					}
					//parent process
					else if (pid > 0){
						wait(NULL);
				 	}
				}
			}


		} else if (!strncmp(argv[1], "-p", 2)) {
			//$./mymake -p Makefile
			if (!process_file(argv[2])) {

				//store information for targets
				store_target_Extracredit();

				//print out information of targets
				for(int t = 0;t <= tgcount; t++ ){
					//targets
					printf("\ntarget '%s' has %d dependencies and %d recipe\n", tg[t].name, tg[t].dep_count, tg[t].recipe_count);
					//dependencies
					for(int d = 0; d < tg[t].dep_count; d++ ){
						printf("Dependency %d is %s\n", d, tg[t].depend[d]);
					}
					//recipes
					for(int r = 0; r < tg[t].recipe_count; r++){
						printf("Recipe %d is %s\n", r, tg[t].recipe[r]);
					}
				}
			}
		}
		//$./mymake -r Makefile
		else if (!strncmp(argv[1], "-r", 2)){
			//Same in here. Store information for each target
			if (!process_file(argv[2])) {

				//This for loop is storing information for each target
				store_target_Extracredit();

				//print out recipes in order using DFS_dep function
				DFS_dep(tg, tgcount);
			}
		}else{
			printf("Not a way to excute the project \n");
		}
	}

	exit(EXIT_SUCCESS);
}
