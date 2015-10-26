#include "mysh.h"

/*
 CITS2002 Project 2 2015
 Name(s):       student-name1 (, student-name2)
 Student number(s): student-number-1 (, student-number-2)
 Date:      date-of-submission
 */
// cc -std=c99 -Wall -Werror -pedantic execute.c globals.c mysh.c parser.c 
// -------------------------------------------------------------------

//  THIS FUNCTION SHOULD TRAVERSE THE COMMAND-TREE and EXECUTE THE COMMANDS
//  THAT IT HOLDS, RETURNING THE APPROPRIATE EXIT-STATUS.
//  READ print_cmdtree0() IN globals.c TO SEE HOW TO TRAVERSE THE COMMAND-TREE


//  THAT IT HOLDS, RETURNING THE APPROPRIATE EXIT-STATUS.
//  READ print_cmdtree0() IN globals.c TO SEE HOW TO TRAVERSE THE COMMAND-TREE
// -------------------change directory
int start_time_sec ;
int start_time_usec;
int stop_time_sec;
int stop_time_usec;
int  exitstatus =0;

int timeCommand(CMDTREE *t);
int exitCommand(CMDTREE *t);
int cdCommand(CMDTREE *t);
int specifiedInternalCommand(CMDTREE *t);    
int unspecifiedInternalCommand(CMDTREE *t);
int do_N_COMMAND(CMDTREE *t);
int do_N_SEMICOLON (CMDTREE *t);
int do_N_AND(CMDTREE *t);
int do_N_OR(CMDTREE *t);


int do_N_SEMICOLON(CMDTREE *t)
{
  if (t == NULL)
    {           // hmmmm, a that's problem
        exitstatus  = EXIT_FAILURE;
    }
  execute_cmdtree(t->left);
  execute_cmdtree(t->right);
  return exitstatus;

}

int nBackground(CMDTREE *t)
{
    int pid;
	int pid2;
    
    switch (pid =fork())
    {
		case -1 :
            perror("fork() failed");     // process creation failed
    		return exitstatus;
    
		case 0  :
    execute_cmdtree(t->left);
	    exit(EXIT_FAILURE);
    
	default :
       
            execute_cmdtree(t->right);
            break;
                exit(EXIT_FAILURE);
    }
    return exitstatus;
}

int execute_cmdtree (CMDTREE *t)
{
if (t == NULL)
    {           // hmmmm, a that's problem
        exitstatus  = EXIT_FAILURE;
    }
  switch (t->type)  //EXECUTE COMMAND BASED ON DIFFERENT TYPE 
{
  case N_AND :   // as in   cmd1 && cmd2
  break;

  case N_BACKGROUND: // as in   cmd1 &
        nBackground(t);
        break;

  case N_OR:    // as in   cmd1 || cmd2 
  break;

  case N_SEMICOLON:    // as in   cmd1 ;  cmd2 
  exitstatus = do_N_SEMICOLON(t);
  break;

  case N_PIPE:   // as in   cmd1 |  cmd2 
  break;

  case N_SUBSHELL:  // as in   ( cmds )
  break;

  case N_COMMAND:
  exitstatus = do_N_COMMAND(t);
  break;

  default :
  printf("invalid input\n");

}


    return exitstatus;
}


int exitCommand(CMDTREE *t)
{
   if (t->argc == 1)
                {
                    exit(exitstatus);
                }
                else  
                {
                    exitstatus = atoi (t->argv[1]);  // USER INPUT EXIT STATUS NUMBER
                    printf("%d",exitstatus);
                    exit(exitstatus);
                    
                }
                return exitstatus;
}

int cdCommand(CMDTREE *t)
{
   if(t->argc == 1)   // NO ARUGMENTS , USE HOME
   {

    if(chdir(HOME) == -1)
      {
    perror("change directory fails");
    exit(EXIT_FAILURE);
      }
   }
   else if( t->argc ==2 && strchr(t->argv[1],'/' )== NULL) //INPUT ARGUMENT DOESN'T HAVE  '/'
   {
    //printf("the CDPATH is %s\n",getenv(CDPATH));
    char *tokencopy;
   
    char *token = strtok(CDPATH,":");
    //printf("fisrt token is %s\n",token);
    

    while(token != NULL)
    {
       tokencopy = malloc(sizeof(char) *(strlen(t->argv[1]) + strlen(token)) +1);
       strcpy(tokencopy,token);
      //printf("first token copy is %s\n",tokencopy);
       strcat(tokencopy,"/");
      //printf("after strcat1 is %s\n",tokencopy);
       strcat(tokencopy,t->argv[1]);
       //printf("after strcat2 is %s\n",tokencopy);

    if(chdir(tokencopy)!=0)
      {
      printf("-mysh: cd: %s:%s\n",tokencopy,strerror(errno)); // ERROR MESSAGE IF CHDIR FAILS
      }
      token = strtok(NULL,":");
        
    }
    free(token);
    free(tokencopy);
   
   }
 
  else if(t->argc ==2 && strchr(t->argv[1],'/' )!= NULL)  //INPUT ARGUMENT HAS  '/'
        {
            char *useful_path= NULL;
            useful_path = malloc(sizeof(char) * strlen(t->argv[1]) + 3); // INITIALIZE A SPACE FOR THE PATH TO BE PUT IN
            if (useful_path == NULL)
            {
                printf("cd: malloc failed");
            }
            strcpy(useful_path,"./");  // COPY CURRENT DIRECTORY INTO THE SPACE
            strcat(useful_path,t->argv[1]); //APPEND INPUT DIRECTORY
            if (chdir(useful_path) != 0) // CHANGE DIRECTORY, IF NOT SUCCESSFUL, PRINT ERROR
            {
                printf("-mysh: cd: %s:%s\n",useful_path,strerror(errno));
                free(useful_path);
            }
            
        }
 return exitstatus;
}

int timeCommand(CMDTREE *t)
{
    if (t->argc >1)  //  if timing is required of a command
    {
        struct timeval  start_time;
        struct timeval  stop_time;
        long double start_time_sec ;
        long double start_time_usec;
        long double stop_time_sec;
        long double stop_time_usec;
        int pid;
        switch (pid = fork())
        {
            case -1 :
                perror("fork() failed");     // process creation failed
                exit(1);
                break;
                
            case 0:// a new child process is created
                gettimeofday(&start_time, NULL);        // gets the time that the time command is started
                start_time_sec = (long double)start_time.tv_sec;  // time in seconds
                start_time_usec=(long double)start_time.tv_usec;  // time in microseconds
                printf("program started at %Lf %Lf\n",
                       start_time_sec, start_time_usec);
                if ((strchr(t->argv[1],'/')) == NULL ) // if the command given does not have specified location
                {
                   exitstatus=unspecifiedInternalCommand(t);
                }
        
                else
                    exitstatus = specifiedInternalCommand(t); //  if the command given does have specified location
               
               
                exit(EXIT_FAILURE);
                
                
            default:                      // original parent process
                while(wait(&exitstatus) != pid); // waits for the child process to finish running;
                gettimeofday(&stop_time, NULL );
                stop_time_sec = (long double)stop_time.tv_sec;
                stop_time_usec = (long double)stop_time.tv_usec;
                printf("program stopped at  %Lf %LF usec\n",stop_time_sec,stop_time_usec);
                /*printf("program stopped at %Lf %Lf\n",
                 stop_time_sec, stop_time_usec );
                 fprintf(stderr,"program ran for %Lf",stop_time_sec - start_time_sec + ((stop_time_usec - start_time_usec)*0.000001));
                 */
                
                /*gettimeofday(&stop_time, NULL );
                 
                 
                 stop_time_sec = (int)stop_time.tv_sec;
                 stop_time_usec = (int)stop_time.tv_usec;
                 
                 printf("program stopped at %i.06%i\n",
                 stop_time_sec, stop_time_usec );
                 
                 printf("%d\n",stop_time_sec);
                 printf("%d\n",start_time_sec);
                 printf ("%d\n",stop_time_sec - start_time_sec );
                 printf("program ran for %i.06%i\n",stop_time_sec - start_time_sec , stop_time_usec - start_time_usec);
                 */
                break;
        }
         return exitstatus;
    }
    
  else
        exit(EXIT_FAILURE);
    
}

int specifiedInternalCommand(CMDTREE *t) //specified location of internal command such as ls
{
   int pid;
   switch (pid = fork())
          {
                                    case -1 :
                                        perror("fork() failed");     // process creation failed
                                        exit(EXIT_FAILURE);
                                        
                                        
                                    case 0:// a new child process is created
                                    execv(t->argv[t->argc-1], t->argv);
                                    exit(EXIT_FAILURE);

                                    default:                      // original parent process
                                        while(wait(&exitstatus) != pid); // waits for the child process to finish running;
                                        break;
            }
                                fflush(stdout);
                                 return exitstatus;    
}
       
int unspecifiedInternalCommand(CMDTREE *t) //unspecified location of internal command such as ls
{
       char *pathlist[10];
       int pid;
	char *temparray[t->argc-1];
                         switch (pid = fork()) 
                                 {
                                    case -1 :
                                        perror("fork() failed");     // process creation failed
                                        exit(1);
                                        break;
                                        
                                    case 0:// a new child process is created
                                         // printf("%s\n",getenv("PATH"));
                                        if(PATH == NULL)
                                        {
                                          perror("Path is null");
                                          exit(EXIT_FAILURE);
                                        }
                                        //char *temparray[t->argc-1];
                                        if (strcmp (t->argv[0],"time")== 0) // if the time command is required then argv needs to 									be manipulated
                                        {
                                            
                                            for (int i=0; i<t->argc;i++)
                                            {
                                                strcpy(temparray[i],t->argv[i+1]);
                                            }
                                        }
                                        char *token = strtok(PATH,":"); //SEPERATE THE PATH VARIABLE
                                        int n = 0;
                                        while(token !=NULL)
                                        {
                                          pathlist[n] = strdup(token);   
                                          token = strtok(NULL,":"); 
                                          strcat(pathlist[n],"/");   //APPEND '/' FOR THE PATH
                                           // printf ("argc -1 is %d\n",t->argc-1);
                                          //  printf("pathlist[n] is %s argv[] is %s\n",pathlist[n],t->argv[t->argc-1]);
                                          strcat(pathlist[n],t->argv[t->argc-1]); // APPEND INPUT ARGUMENT FOR THE PATH
                                         // printf("%s\n",pathlist[n]);
                                          if (strcmp (t->argv[0],"time")== 0)
                                              execv(pathlist[n],temparray); // temparray is used instead of argv when time is called before
                                          else
                                              execv(pathlist[n],t->argv); // EXECUTE THE SYSTEM CALL
                                          n++;
                                        } 
                                        exit(EXIT_FAILURE); 

                                    default:                      // original parent process
                                        while(wait(&exitstatus) != pid); // waits for the child process to finish running;
                                        break;
                                }
                                fflush(stdout);
                                return exitstatus;
}

int do_N_COMMAND(CMDTREE *t) // EXECUTION IF TYPE IS N_COMMAND
{
if (t == NULL)
    {           // hmmmm, a that's problem
        exitstatus  = EXIT_FAILURE;
    }
    
  // --------------------------------- EXIT COMMAND
    else if(strcmp (t->argv[0] , "exit")== 0) 
            {
                exitstatus = exitCommand(t);
            }

// ----------------------------------- change directory
          else  if(strcmp (t->argv[0],"cd")== 0) // if the command is cd then follow these conditions
   {
             exitstatus =cdCommand(t);
   }
             
// ---------------------------------ls type commands
            else   if ((strchr(t->argv[0],'/')) != NULL ) // INPUT ARGUMENT DOES NOT HAVE '/'
               {
                exitstatus = specifiedInternalCommand(t);
              }
               else if ((strchr(t->argv[0],'/')) == NULL && strcmp (t->argv[0],"cd")!= 0 && strcmp (t->argv[0],"time")!= 0) 
               {
                exitstatus =unspecifiedInternalCommand(t);
               }
               else if(strcmp (t->argv[0],"time")== 0) // time command
               {
                exitstatus = timeCommand(t);
               }
               
                return exitstatus;
}

