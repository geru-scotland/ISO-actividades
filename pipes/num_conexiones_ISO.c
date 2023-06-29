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

int main() {   // how many users are connected of acaf00?
   int pid;

   unlink("my_fifo1");
   unlink("my_fifo2");

   if (mkfifo("my_fifo1", 0644) == -1)
       error("mkfifo1");

#ifdef DEBUG
   printf("parent: fifo1 created\n");
#endif

   if (mkfifo("my_fifo2", 0644) == -1)
       error("mkfifo2");

#ifdef DEBUG
   printf("parent: fifo2 created\n");
#endif

   switch (pid = fork()) {
      case -1:
          error("fork");
      case 0: /* 1st child process: who */
#ifdef DEBUG
         printf("1st child process created\n");
#endif
         if (close(1) == -1)
             error("close");
         if (open("my_fifo1", O_WRONLY) != 1)
             error("open");

         execlp("who", "who", NULL);
         error("execlp");
       default:
           break;
   }

#ifdef DEBUG
   printf("parent: who(%d) process launched\n", pid);
#endif

   switch (pid = fork()) {
      case -1:
          error("fork");
      case 0: /* 2nd child process: grep acaf00 */
#ifdef DEBUG
          printf("2nd child process created\n");
#endif
          if (close(0) == -1)
              error("close0");
          if (close(1) == -1)
              error("close1");
          if (open("my_fifo1", O_RDONLY) != 0)
              error("open1");
          if (open("my_fifo2", O_WRONLY) != 1)
              error("open2");

          execlp("grep", "grep", "acaf00", NULL);
          error("execlp");
       default:
           break;
   }

    printf("Numero de conexiones: \n");
   switch (pid = fork()) {
      case -1:
          error("fork");
      case 0: /* 3nd child process: wc -l */
#ifdef DEBUG
          printf("3nd child process created\n");
#endif
          if (close(0) == -1)
              error("close");
          if (open("my_fifo2", O_RDONLY) != 0)
              error("open");

          execlp("wc", "wc", "-l", NULL);
          error("execlp");
       default:
           break;
   }

#ifdef DEBUG
   printf("parent: wc(%d) process launched\n", pid);
#endif
   while ((pid = wait(NULL)) != -1){
#ifdef DEBUG
       printf("parent: %d process finished\n", pid);
#endif
   }

   unlink("my_fifo1");
#ifdef DEBUG
   printf("parent: fifo1 removed\n");
#endif
   unlink("my_fifo2");

#ifdef DEBUG
   printf("parent: fifo2 removed\n");
#endif
   exit(0);
}
