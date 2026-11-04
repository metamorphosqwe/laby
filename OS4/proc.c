/* ============================================================
 * Лабораторная работа №4 «GCC. Процессы», вариант 7.
 *
 * Генеалогическое дерево (столбец fork): 0 1 1 1 3 3 5 5
 *   каждая цифра - номер процесса-родителя для процесса с
 *   номером 1, 2, ..., 8 (0 - родитель вне программы):
 *     №1 - корень (запускается из shell)
 *     №2, №3, №4 - порождены процессом №1
 *     №5, №6     - порождены процессом №3
 *     №7, №8     - порождены процессом №5
 * Процесс №7 (столбец exec) заменяет себя командой ps -u <user>.
 *
 * Сборка: gcc -Wall -Wextra -o proc proc.c
 * ============================================================ */

#include <stdio.h>      /* printf, perror, fflush */
#include <stdlib.h>     /* exit */
#include <unistd.h>     /* fork, getpid, getppid, execlp, usleep */
#include <sys/types.h>  /* pid_t */
#include <sys/wait.h>   /* waitpid */
#include <pwd.h>        /* getpwuid: имя текущего пользователя */

#define N 8             /* всего процессов в дереве */
#define EXEC_NUM 7      /* номер процесса, вызывающего exec */

/* parent[i] - номер родителя процесса №i; индекс 0 не
 * используется, у корня (№1) родителя в дереве нет. */
static const int parent[N + 1] = {0, 0, 1, 1, 1, 3, 3, 5, 5};

/* Сообщение о себе. getpid() и getppid() вызываются КАЖДЫЙ раз
 * заново: PID и PPID берутся у ядра перед каждым выводом. */
static void say_start(int num)
{
    printf("Процесс №%d: ID = %d, ID родителя = %d\n",
           num, getpid(), getppid());
    fflush(stdout);
}

/* Завершение процесса №num. Процесс EXEC_NUM вместо завершения
 * заменяет свой пользовательский контекст программой ps. */
static void finish(int num)
{
    if (num == EXEC_NUM) {
        struct passwd *pw = getpwuid(getuid());
        const char *user = pw ? pw->pw_name : "root";

        printf("Процесс с ID %d и ID родителя %d "
               "завершает работу (запускает ps -u %s)\n",
               getpid(), getppid(), user);
        fflush(stdout);
        /* exec не создаёт нового процесса: PID и PPID сохраняются.
         * Возврат из execlp происходит только при ошибке. */
        execlp("ps", "ps", "-u", user, (char *)NULL);
        perror("execlp");
        exit(1);
    }
    printf("Процесс с ID %d и ID родителя %d "
           "завершает работу\n", getpid(), getppid());
    fflush(stdout);
    exit(0);
}

/* Работа процесса №num: сообщить о себе, породить всех своих
 * детей согласно таблице parent[], завершиться. */
static void run(int num)
{
    int i;
    pid_t pid;

    say_start(num);

    for (i = num + 1; i <= N; i++) {
        if (parent[i] != num)
            continue;               /* i - не ребёнок процесса num */

        /* Сбрасываем буфер stdout ПЕРЕД fork: иначе при выводе
         * в файл/канал ребёнок унаследовал бы копию буфера и
         * сообщения продублировались бы. */
        fflush(stdout);
        pid = fork();

        if (pid == -1) {            /* ошибка fork */
            perror("fork");
            exit(1);
        } else if (pid == 0) {      /* ребёнок */
            /* Небольшая задержка, чтобы родитель успел вывести
             * сообщение "породил" раньше первого сообщения ребёнка
             * (тот же приём, что sleep() в примере из задания). */
            usleep(100000);
            run(i);                 /* из run ребёнок не возвращается */
        } else {                    /* родитель */
            printf("Процесс с ID %d породил процесс с ID %d "
                   "(процесс №%d)\n", getpid(), pid, i);
            fflush(stdout);
            /* Ждём завершения ребёнка: нет процессов-зомби, нет
             * "осиротевших" процессов (PPID не меняется на 1),
             * порядок вывода всегда одинаковый. */
            waitpid(pid, NULL, 0);
        }
    }

    finish(num);
}

int main(void)
{
    run(1);                         /* процесс №1 - корень дерева */
    return 0;
}
