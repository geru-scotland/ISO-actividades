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

#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define error(a) {perror(a); exit(1);};

#define FIFO_1 "fifo_1"
#define FIFO_2 "fifo_2"

#define PATTERN "acaf00"
#define OPTIONS "-l"

int main(int argc, char* argv[]) {

    unlink(FIFO_1);
    unlink(FIFO_2);

    if(mkfifo(FIFO_1, 0644) == -1)
        error("Fifo 1 creation");

    if(mkfifo(FIFO_2, 0644) == -1)
        error("Fifo 2 creation");

    switch(fork()){
        case -1:
            error("fork");
        case 0:
            close(STDOUT_FILENO);
            if(open(FIFO_1, O_WRONLY) == -1)
                error("open");

            execlp("who", "who", NULL);
            break;
        default:
            break;
    }

    switch(fork()){
        case -1:
        error("fork");
        case 0:
            close(STDIN_FILENO);
            if(open(FIFO_1, O_RDONLY) == -1)
                error("open");

            close(STDOUT_FILENO);
            if(open(FIFO_2, O_WRONLY) == -1)
                error("open");

            execlp("grep", "grep", PATTERN, NULL);
            break;
        default:
            break;
    }

    switch(fork()){
        case -1:
        error("fork");
        case 0:
            close(STDIN_FILENO);
            if(open(FIFO_2, O_RDONLY) == -1)
                error("open");
            execlp("wc", "wc", OPTIONS, NULL);
            break;
        default:
            break;
    }

    while(wait(NULL) != -1);

    unlink(FIFO_1);
    unlink(FIFO_2);

    exit(0);
}
