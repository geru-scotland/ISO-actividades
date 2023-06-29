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
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>
#include <signal.h>

#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define MAX_RAND 10
#define FIRST_PARAM 1
#define EXIT_ARG_ERROR 1

#define CMD_EXEC "./clock"
#define CMD_SLEEP "sleep"
#define ARGS_TEST "--test"

int main(int argc, char *argv[]) {

    if (argc < 5) {
        printf("Uso: %s nb_tests min_time max_time program [parameters]\n", argv[0]);
        exit(EXIT_ARG_ERROR);
    }

    int i;
    pid_t child_pid, clock_pid, current_pid;
    struct timespec start_time, end_time;

    double time_taken;

    int nb_tests = atoi(argv[1]);
    int min_time = atoi(argv[2]);

    char *max_time = argv[3];
    char *program = argv[4];
    char **parameters = &argv[4];

    int too_fast = 0;
    int too_slow = 0;
    int within_range = 0;
    double total_time = 0;
    int random_number = -1;


    if(nb_tests <= 0){
        printf("Valor incorrecto para el numero de tests.\n");
        exit(EXIT_ARG_ERROR);
    }

    if(min_time <= 0){
        printf("Valor incorrecto para el tiempo mínimo.\n");
        exit(EXIT_ARG_ERROR);
    }

    if(atoi(max_time) <= 0 || atoi(max_time) <= min_time){
        printf("Valor incorrecto para el tiempo máximo.\n");
        exit(EXIT_ARG_ERROR);
    }

    int is_sleep = strcmp(program, CMD_SLEEP) == 0;
    int is_test = 0;

    if (is_sleep && argc > 5 && strcmp(parameters[FIRST_PARAM], ARGS_TEST) == 0) {
        is_test = 1;
        srand(time(NULL));
    }

    for (i = 0; i < nb_tests; i++) {

        if (is_test) {
            random_number = rand() % MAX_RAND + 1;
            sprintf(parameters[FIRST_PARAM], "%i", random_number);
        }

        clock_pid = fork();

        if(clock_pid == 0)
            execlp(CMD_EXEC, CMD_EXEC, max_time, NULL);

        child_pid = fork();

        if(child_pid == 0)
            execvp(program, parameters);

        clock_gettime(CLOCK_MONOTONIC, &start_time);
        current_pid = wait(NULL);
        clock_gettime(CLOCK_MONOTONIC, &end_time);

        time_taken = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1e9;

        if (current_pid == child_pid) {   /* child finishes first; stop clock */
            kill(clock_pid, SIGKILL);
            wait(NULL);

            if (time_taken < min_time)
                too_fast++;
            else
                within_range++;

        }
        else {   /* clock finishes first; force child to finish */
            kill(child_pid, SIGKILL);
            wait(NULL);
            too_slow++;
        }

        total_time += time_taken;
    }

    printf("\n-----------------------\n");
    printf("      RESULTADOS     \n");
    printf("-----------------------\n");

    if (too_fast > 0)
        printf(ANSI_COLOR_YELLOW " Demasiado rápido:   %d\n" ANSI_COLOR_RESET, too_fast);
    else
        printf(" Muy rápido:   %d\n", too_fast);

    if (too_slow > 0)
        printf(ANSI_COLOR_RED " Demasiado lento:    %d\n" ANSI_COLOR_RESET, too_slow);
    else
        printf(" Demasiado lento:    %d\n", too_slow);

    if (within_range > 0)
        printf(ANSI_COLOR_GREEN " Dentro del rango:   %d\n" ANSI_COLOR_RESET, within_range);
    else
        printf(" Dentro del rango:   %d\n", within_range);

    printf("-----------------------\n");
    printf(" Tiempo total de ejecución: %f segundos\n", total_time);
    printf("-----------------------\n\n");
    printf("\"May the force be with you.\"\n\n");
    return 0;
}
