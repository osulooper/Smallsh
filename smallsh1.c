#define _GNU_SOURCE 
#define _POSIX_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <ctype.h>
char *prompt(char *buffer);
char * str_gsub(char *restrict *restrict haystack, char const *restrict needle, char const *restrict sub);

int main(void){
  char *background_pid = "";//$!
  char *child_exit_status = "0";//$?
  pid_t child_status = 0;
  int exit_status = 0;
  int result = 0;
  int status = 0;
  // check for any un-waited-for background processes in the same process group ID as smallsh
  pid_t child_pid = 0; 
  int pid_1 = 0;
  while (1==1){
  while ((pid_1 = waitpid(0,&status, WNOHANG | WUNTRACED)) > 0){
    if (WIFEXITED(status)){
    fprintf(stderr,"child process %jd done, Exit Status %d. \n", (intmax_t) pid_1, status);
    }
    if (WIFSIGNALED(status)){
    fprintf(stderr, "child process %jd done. Signalled %d. \n",(intmax_t) pid_1,WTERMSIG(status));
    }
    if(WIFSTOPPED(status)){//Child process STOPPED kill(2)
    kill(pid_1,SIGCONT);
    fprintf(stderr,"child process %jd stopped. Continuing.\n",(intmax_t) pid_1);
    }
  }
  char *buffer = NULL;
  buffer = prompt(buffer);
  char *ifs = getenv("IFS"); 
  if (getenv("IFS") == NULL){
    ifs = " \t\n";
    }

  char *token;
  char *tokenCopies[513];
  int counter = 0;
  token = strtok(buffer,ifs);
  while (token != NULL){
    tokenCopies[counter] = strdup(token);
    token = strtok(NULL,ifs);
    counter += 1;
  }
  tokenCopies[counter] = NULL;

  if (tokenCopies[0] == NULL){
    continue;
  }

  free(token);
  token = NULL;
  free(buffer);
  buffer = NULL;
  char *path = getenv("HOME");
  if (tokenCopies[0] && strcmp(tokenCopies[0],"cd")==0){
    if (tokenCopies[2]!= NULL){
      perror("too many arguments");
    }
    else if (tokenCopies[1]){
      //path = tokenCopies[1];
      int result = chdir(tokenCopies[1]);
      if (result == -1){
        perror ("failed chdir()");
      }}
    else{
        chdir(path);
    }
    /*else{
      path = getenv("HOME");
      chdir(path);
    }*/
    free(*tokenCopies);
    memset(&tokenCopies[0],0,sizeof(tokenCopies));
    continue;
  }

  if (tokenCopies[0] && strcmp(tokenCopies[0],"exit")==0){
    if (tokenCopies[2]){
      perror("too many arguments");
    }
    fprintf(stderr,"\nexit\n");
    if (tokenCopies[1] && isdigit(*tokenCopies[1])!=0){
      exit_status = atoi(tokenCopies[1]); 
      exit(exit_status);
      kill(0,SIGINT);
    }
    else{
      exit(child_status);
      kill(0,SIGINT);
    }
  }

  
  if (child_exit_status == NULL){
    child_exit_status = "0";
  }
  if (background_pid == NULL){
      background_pid = "";
    }
  for (int i = 0; i < counter; i++){
    char *pid = malloc(sizeof(int));
    sprintf(pid, "%d", getpid());
    str_gsub(&tokenCopies[i], "$$",pid);
    str_gsub(&tokenCopies[i], "$?",child_exit_status);
    str_gsub(&tokenCopies[i], "$!",background_pid);
    free(pid);
    if (strncmp("~/",tokenCopies[i] ,2)==0){
        str_gsub(&tokenCopies[i], "~",getenv("HOME"));
    }
    //printf("token After expansion: %s \n",tokenCopies[i]);
  }
  for (int i = 0; i < counter; i++){
    if (strcmp(tokenCopies[i],"#")==0){
      free(tokenCopies[i]);
      tokenCopies[i] = NULL;
      counter = i;
      break;
    }
  }
  
  int backgroundFlag = 0;
  if (tokenCopies[counter-1] && strcmp(tokenCopies[counter-1], "&") ==0){
    backgroundFlag = 1;
    free(tokenCopies[counter -1]);
    tokenCopies[counter -1]=NULL;
    counter -=3;
  }
  else{
    counter -=2;
  }

  
  char *input_file = NULL;
  char *output_file = NULL;
  while (counter > 0){
  if (tokenCopies[counter] && strcmp(tokenCopies[counter],"<")==0){
    input_file = tokenCopies[counter + 1];
    free(tokenCopies[counter]);
    tokenCopies[counter] = NULL;
    //printf("inputfile: %s\n", input_file);
  }
  if (tokenCopies[counter] && strcmp(tokenCopies[counter],">")==0){
    output_file = tokenCopies[counter + 1];
    free(tokenCopies[counter]);
    tokenCopies[counter] = NULL;
  }
  counter -=2;
  }


  child_pid = fork();
  switch(child_pid){
    case -1:
      perror("fork() failed");
      exit(1);
      break;
    case 0:   

      //int result = 0;
      if (input_file != NULL){
        int sourceFD = open(input_file, O_RDONLY);
        if (sourceFD == -1) { 
		      perror("source open()"); 
	        }
        result = dup2(sourceFD,0);
	      if (result == -1) { 
		      perror("source dup2()"); 
		      
	        }
      }
      if (output_file != NULL){
        int targetFD = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (targetFD == -1) { 
		    perror("target open()"); 
		    exit(1); 
	      }
         result = dup2(targetFD , 1);
        if (result == -1){
          perror("target dup2");
          exit(2);
          }
      }
      execvp(tokenCopies[0],tokenCopies);
      break;
    default:
      if (backgroundFlag != 1){
      waitpid(child_pid,&child_status, 0);
      //NOT RUNNING IN BACKGROUND/RUNNING IN FOREGROUND
      }
      else{ 
        background_pid = malloc(sizeof(int));
        sprintf(background_pid,"%d", child_pid);
      }
      backgroundFlag = 0;
      break;
      }


  child_exit_status = malloc(sizeof(int));
  sprintf(child_exit_status,"%d",WEXITSTATUS(child_status));
  free(*tokenCopies);
  memset(&tokenCopies[0],0,sizeof(tokenCopies));
  }
  free(background_pid);
  free(child_exit_status);
  child_exit_status = NULL;
  background_pid = NULL;
}

char* prompt(char *buffer){
   
    size_t len = 0;
    char *ps1 = "";
    if (getenv("PS1") != NULL){
    ps1 = getenv("PS1");
    }
    fprintf(stderr,"%s",ps1);
    getline(&buffer,&len,stdin);
    if (feof(stdin) != 0){
      fprintf(stderr,"\nexit\n");
      clearerr(stdin);
      if(kill(0,SIGINT) == -1){
        perror("kill()");
      }
    }

    return buffer;
}



char * str_gsub(char *restrict *restrict haystack, char const *restrict needle, char const *restrict sub)
{
	char *str = *haystack;
	size_t haystack_len = strlen(str);
	size_t const needle_len = strlen(needle),
	             sub_len = strlen(sub);
  int counter = 0;
	
	for (; (str = strstr(str, needle));) {
		ptrdiff_t off = str - *haystack;
		if(sub_len > needle_len){
			str = realloc(*haystack, sizeof **haystack * (haystack_len + sub_len - needle_len + 1));
			if (!str) goto exit;
			*haystack = str;
			str = *haystack + off;
      counter += 1;
	}

/*
	haystack = "this is my haystack!!!"
	needle = "stacl"
	sub = "heap"
*/
	memmove(str + sub_len, str + needle_len, haystack_len + 1 - off - needle_len);
	memcpy(str, sub, sub_len);
	haystack_len = haystack_len + sub_len - needle_len;
	str += sub_len;
   if (strcmp("~/",needle)==0 && counter > 0)
    {
      break;
    }
	
	}
	str = *haystack;
	if (sub_len < needle_len) {
		str = realloc(*haystack, sizeof **haystack * (haystack_len + 1));
		if (!str)goto exit;
		*haystack = str;
	}

exit:
	return str;
}

	


