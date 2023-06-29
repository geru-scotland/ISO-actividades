/*
 * This file is part of the Sistemas Operativos project.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * @authors - Geru-Scotland (https://github.com/geru-scotland)
 *          - Farolei (https://github.com/farolei)
 *          - UnaiAD22 (https://github.com/UnaiAD22)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define error(a) {perror(a); exit(1);};
#define BUFSIZE 512
#define MAXARG 11

#define EXIT_PROCESS 0

#define SHELL2 "my_sh2> "
#define SHELL3 "my_sh3> "

void get_parameters(char *buf, int n, char *argk[], int ma);

int main(int argc, char *argv[])
{
    int n;
    pid_t cpid, gcpid;
    char buf[BUFSIZE];
    char* arg[MAXARG];
    char command_type;
    char* sh = SHELL2;
    int sh3_mode = 0;

    if(argc > 1 && strcmp(argv[1],"-m --sh3") == 0){
        sh = SHELL3;
        sh3_mode = 1;
    }

    for (n = 0; n < BUFSIZE; n++)
        buf[n] = '\0';

    /* read */
    write(STDOUT_FILENO, sh, strlen(sh));
    while ((n = read(0, buf, BUFSIZE)) > 0)
    {
        buf[n] = '\n';
        n++;

        get_parameters(buf, n, arg, MAXARG);

        if (arg[0] == NULL)
        {
            write(STDOUT_FILENO, sh, strlen(sh));
            continue;
        }

        command_type = arg[0][0];
        if (command_type != 'R' && command_type != 'S')
        {
            write(STDOUT_FILENO, "Comando no valido (usa 'R' o 'S')\n", 36);
            write(STDOUT_FILENO, sh, strlen(sh));
            continue;
        }

        switch (cpid = fork())
        {
            case -1:
                error("fork");
            case 0: /* child */
                if(sh3_mode){
                    switch(gcpid = fork()){
                        case -1:
                            error("fork");
                        case 0:
                            execvp(arg[1], &arg[1]);
                            error("exec");
                        default:
                            while (wait(NULL) != -1);

                            printf("[Hijo] He terminado y mi proceso es: %i \n", getpid());
                            exit(EXIT_PROCESS);
                    }
                }else{
                    execvp(arg[1], &arg[1]);
                    error("exec");
                }
            default: /* parent */
                if (command_type == 'R')
                    while(cpid != wait(NULL)); // Es para que cuando termine el último R ejecutado, se detecte

                for (n = 0; n < BUFSIZE; n++)
                    buf[n] = '\0';

                write(STDOUT_FILENO, sh, strlen(sh));
                break;
        }
    }
    printf("\n");
}

void get_parameters(char *buf, int n, char *argk[], int m)
{
    int i, j;

    for (i = 0, j = 0; (i < n) && (j < m); j++)
    {
        /* advance blanks */
        while (((buf[i] == ' ') || (buf[i] == '\n')) && (i < n)) i++;
        if (i == n) break;
        argk[j] = &buf[i];
        /* find blank */
        while ((buf[i] != ' ') && (buf[i] != '\n')) i++;
        buf[i++] = '\0';
    }
    argk[j] = NULL;
}
