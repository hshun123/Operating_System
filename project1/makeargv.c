#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#define LINE_SIZE 128
#define MAX_LINES 128
#define MAX_DEP 8
//Account for extra credit
#define MAX_RECIPES_PT 8
#define MAX_TARGETS 128
#define MAX_PARM 32

typedef struct target_block {
	char *name;
	char *depend[MAX_DEP];
	char *recipe[MAX_RECIPES_PT];
	unsigned char dep_count;
	unsigned char recipe_count;
	unsigned char visited;
} target;

int makeargv(const char *s, const char *delimiters, char ***argvp) {
   int error;
   int i;
   int numtokens;
   const char *snew;
   char *t;

   if ((s == NULL) || (delimiters == NULL) || (argvp == NULL)) {
      errno = EINVAL;
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

int main(int argc, char *argv[]) {
    char delim[] = ": ";
    int i;
    char*te = "   mygame: main.o player.o enemy.o renderer.o vecmath.o image.o mesh.o";
    char*te1 = "     mygame:               ";
    char **myargv;
    int numtokens;
    target tg[128];

    /*
    if (argc != 2) {
    fprintf(stderr, "Usage: %s string\n", argv[0]);
    return 1;
    }
    */
    if ((numtokens = makeargv(te1, delim, &myargv)) == -1) {
    fprintf(stderr, "Failed to construct an argument array for %s\n", te1);
    return 1;
    }
    printf("The argument array contains:\n");
    for (i = 0; i < numtokens; i++)
    printf("%d: %s\n", i, myargv[i]);

    printf("numtokens: %d\n", numtokens);
    //printf("%d\n", tg[0].recipe_count);
return 0;
}
