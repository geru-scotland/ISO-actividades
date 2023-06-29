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

// Patrón: cat /etc/passwd | grep acaf00 | wc -l

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define error(a) {perror(a); exit(1);};
#define MAX_CHANNELS 2
#define PIPE_RD 0
#define PIPE_WR 1

#define PASSWD "/etc/passwd"
#define PATTERN "acaf00"
#define OPTIONS "-l"


/**
 * Función para mejorar la legibilidad del código, a la vez
 * que aplicamos tratamiento de errores.
 *
 * \param Resultado a evaluar. Detendrá la ejecución del program en caso de que el
 * argumento sea negativo.
 */
void eval(int res);

/**
 * Utilizamos dos tuberías para el siguiente comando:
 *
 * cat /etc/passwd | grep acaf00 | wc -l
 *
 * Aunque podría hacerse reduciendo comandos:
 *
 * grep acaf00 /etc/passwd | wc -l
 */
int main(int argc, char* argv[])
{
    int pipe1[MAX_CHANNELS], pipe2[MAX_CHANNELS];

    if(pipe(pipe1) == -1)
        error("pipe1");

    if(pipe(pipe2) == -1)
        error("pipe2");

    /**
     * cat
     */
    switch(fork()){
        case -1:
        error("fork1");
        case 0:
            eval(close(pipe2[PIPE_RD]));
            eval(close(pipe2[PIPE_WR]));

            eval(close(pipe1[PIPE_RD]));

            eval(close(STDOUT_FILENO));
            eval(dup(pipe1[PIPE_WR]));
            eval(close(pipe1[PIPE_WR]));

            execlp("cat", "cat", PASSWD, NULL);
            break;
        default:
            eval(close(pipe1[PIPE_WR]));
            break;
    }

    /**
    * grep
    */
    switch(fork()){
        case -1:
        error("fork2");
        case 0:
            eval(close(pipe2[PIPE_RD]));

            eval(close(STDIN_FILENO));

            eval(dup(pipe1[PIPE_RD]));

            eval(close(pipe1[PIPE_RD]));
            eval(close(STDOUT_FILENO));

            eval(dup(pipe2[PIPE_WR]));

            eval(close(pipe2[PIPE_WR]));

            execlp("grep", "grep", PATTERN, NULL);
            break;
        default:
            eval(close(pipe1[PIPE_RD]));
            eval(close(pipe2[PIPE_WR]));
            break;
    }

    printf("Numero de cuentas: \n");
    /**
     * wc
     */
    switch(fork()){
        case -1:
        error("fork3");
        case 0:
            eval(close(STDIN_FILENO));

            eval(dup(pipe2[PIPE_RD]));

            eval(close(pipe2[PIPE_RD]));

            execlp("wc", "wc", OPTIONS, NULL);
            break;
        default:
            close(pipe2[PIPE_WR]);
            break;
    }

    // No es necesario que sigan un orden, con que terminen los 3, vale.
    // Los pipes escriben concurrentemente (por eso tenemos 2 en lugar de 1)
    while(wait(NULL) != -1);

    printf("\n");
    exit(0);
}

void eval(int res){
    if(res == -1)
        error("close or dup error");
}