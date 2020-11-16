#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <signal.h>

const unsigned int MAX = 1024; // max length for the input line

FILE *f; // log file

int input(char *line,char *args[]) { // takes input and check if the input is exit ,clear or empty line

    // to print the word directory
    char cwd[MAX];
    // have & in the last char or not
    int wait = 0;

    getcwd(cwd, sizeof(cwd));

    printf("%s>> ",cwd);

    // to take the input string with spaces
    fgets(line,MAX,stdin);

    line[strlen(line)-1] = '\0';

    if (!strcmp(line,"exit")) { // to break the loop

        return -1;

    }else if (strlen(line) == 0) { // if the user enter new line
        return 2;
    } else if (strlen(line) >= 2 && line[strlen(line)-1] == '&' && line[strlen(line)-2] == ' ') { // if the command finished with &
        line[strlen(line)-2] = '\0';
        wait = 1;
    }

    args[0] = strtok(line," ");

    if (!strcmp(args[0],"cd")) { // check if the word is change directory
        // take the string after the first space to the null char
        char *path = strtok(NULL,"\0");
        if (path == NULL) { // if no director entered we change to the root directory
            path = "/";
        }
        int error = chdir(path);
        if (error == -1) {
            printf("bash: cd: %s: No such file or directory\n",path);
        }
        return 2;
    }
    // split around the space
    for (int i = 1; args[i-1] != NULL ; ++i) {
        args[i] = strtok(NULL," ");
    }

    if (wait) {
        return 1;
    }

    return 0;
}
 // to write in the log file when the process is terminated
void handler() {

    int id = waitpid (-1, NULL, WNOHANG);
    if (id != -1) {
        fprintf(f, "Child process was terminated with id = %d\n", id); // write in the log file
        fflush(f);
    }

}

int main() {

    char line[MAX],*args[MAX/2 + 1];

    f = fopen ("data.log", "w");

    while (1) {

        int in = input(&line,&args);

        if (in == -1) {
            break;
        } else if (in > 1) {
            continue;
        }

        signal(SIGCHLD, handler); // when the command is finished

        pid_t pid = fork();

        if (pid == -1) {

            printf("Failed forking child..\n");
            return 1;

        } if (pid == 0) {

            int error = execvp(args[0], args);
            if (error == -1) {
                printf("%s: command not found\n",line); // if there is error
            }
            exit(0);

        } else {
            if (!in) {

                waitpid (pid, NULL, 0); // wait the current child process

            }
        }

    }
    fclose(f);
    return 0;
}
