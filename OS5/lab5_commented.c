/* Лабораторная работа №5, вариант 4.
 * Программа читает со стандартного ввода строки формата
 * /etc/passwd (login:password:UID:GID:GECOS:home:shell).
 *  - чётный UID  -> stdout (fd = 1);
 *  - нечётный UID -> stderr (fd = 2);
 *  - файл uid_names.txt: "UID login";
 *  - файл gid_names.txt: "GID login".
 * Весь ввод-вывод выполнен системными вызовами read, write,
 * open, close (printf/scanf не используются для данных). */
#include <fcntl.h>     /* open, O_WRONLY, O_CREAT, O_TRUNC */
#include <signal.h>    /* signal, SIGPIPE */
#include <stdio.h>     /* snprintf: только форматирование */
#include <stdlib.h>    /* malloc, realloc, free, strtol */
#include <string.h>    /* strchr, strlen, memcpy */
#include <sys/stat.h>  /* umask */
#include <sys/types.h> /* ssize_t, size_t */
#include <unistd.h>    /* read, write, close */

/* write может записать МЕНЬШЕ байт, чем просили (в pipe, при
 * сигнале). Поэтому пишем в цикле, пока не запишем всё.
 * Вопрос преподавателя: "почему не одним write?" - потому что
 * write возвращает число реально записанных байт. */
static int write_all(int fd, const char *buf, size_t n)
{
    while (n > 0)
    {
        ssize_t w = write(fd, buf, n);
        if (w < 0)
            return -1;  /* ошибка (например, EPIPE) */
        buf += w;       /* сдвигаем указатель */
        n -= (size_t)w; /* осталось записать */
    }
    return 0;
}

/* Разбор строки: находим начала полей после двоеточий и
 * читаем UID (поле 3) и GID (поле 4) через strtol, чтобы
 * отсечь некорректные строки. Возвращает 0 при успехе. */
static int parse_line(char *line, char **login, long *uid,
                      long *gid)
{
    char *f[7]; /* указатели на начала полей */
    int n = 0;
    char *p = line;
    char *end;

    f[n++] = p; /* первое поле - login */
    while (*p != '\0' && n < 7)
    {
        if (*p == ':')
            f[n++] = p + 1; /* после ':' начинается поле */
        p++;
    }
    if (n < 4) /* нужно минимум 4 поля */
        return -1;
    *login = f[0];
    *uid = strtol(f[2], &end, 10);
    if (end == f[2] || *end != ':') /* не число */
        return -1;
    *gid = strtol(f[3], &end, 10);
    if (end == f[3] || (*end != ':' && *end != '\0'))
        return -1;
    f[1][-1] = '\0'; /* обрываем login на первом ':' */
    return 0;
}

/* Читает весь stdin (fd = 0) в динамический буфер. read()
 * возвращает 0 в конце файла, -1 при ошибке. Буфер растёт
 * вдвое, поэтому размер входа не ограничен. */
static char *read_stdin(size_t *len)
{
    size_t cap = 4096, used = 0;
    char *buf = malloc(cap + 1); /* +1 под завершающий '\0' */

    if (buf == NULL)
        return NULL;
    for (;;)
    {
        ssize_t r;
        if (used == cap)
        { /* буфер заполнен - растим */
            char *t = realloc(buf, cap * 2 + 1);
            if (t == NULL)
            {
                free(buf);
                return NULL;
            }
            buf = t;
            cap *= 2;
        }
        r = read(0, buf + used, cap - used);
        if (r < 0)
        {
            free(buf);
            return NULL;
        }
        if (r == 0) /* конец ввода */
            break;
        used += (size_t)r;
    }
    buf[used] = '\0';
    *len = used;
    return buf;
}

int main(void)
{
    size_t len = 0;
    char *data, *line, *next;
    int fd_uid, fd_gid;

    /* Обнуляем umask, чтобы права файлов были ровно 0644
     * (реальные права = mode & ~umask). */
    (void)umask(0);
    /* Если читатель конвейера (head) закрылся раньше, write
     * в stdout вызвал бы SIGPIPE и убил бы программу до
     * создания файлов. Игнорируем сигнал: write вернёт -1. */
#ifdef SIGPIPE
    signal(SIGPIPE, SIG_IGN);
#endif

    data = read_stdin(&len);
    if (data == NULL)
    {
        write_all(2, "Can't read stdin\n", 17);
        return 1;
    }
    /* O_TRUNC: при повторном запуске файл перезаписывается,
     * а не дописывается. O_CREAT требует третий аргумент mode. */
    fd_uid = open("uid_names.txt",
                  O_WRONLY | O_CREAT | O_TRUNC, 0644);
    fd_gid = open("gid_names.txt",
                  O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_uid < 0 || fd_gid < 0)
    {
        write_all(2, "Can't open file\n", 16);
        free(data);
        return 1;
    }
    /* Идём по строкам буфера; конец строки - '\n'. */
    for (line = data; *line != '\0'; line = next)
    {
        char *nl = strchr(line, '\n');
        char *login, orig[1024], rec[1100];
        long uid, gid;
        size_t ll;

        if (nl != NULL)
        {
            *nl = '\0'; /* превращаем в C-строку */
            next = nl + 1;
        }
        else
        { /* последняя без '\n' */
            next = line + strlen(line);
        }
        ll = strlen(line);
        if (ll == 0 || ll >= sizeof(orig) - 1)
            continue; /* пустые/слишком длинные */
        /* parse_line портит строку (ставит '\0'), поэтому
         * сначала сохраняем исходный текст для вывода. */
        memcpy(orig, line, ll);
        orig[ll] = '\n';
        if (parse_line(line, &login, &uid, &gid) < 0)
            continue; /* некорректную пропускаем */
        /* чётный UID -> fd 1 (stdout), нечётный -> fd 2 */
        write_all(uid % 2 == 0 ? 1 : 2, orig, ll + 1);
        ll = (size_t)snprintf(rec, sizeof(rec), "%ld %s\n",
                              uid, login);
        write_all(fd_uid, rec, ll); /* "UID login" */
        ll = (size_t)snprintf(rec, sizeof(rec), "%ld %s\n",
                              gid, login);
        write_all(fd_gid, rec, ll); /* "GID login" */
    }
    if (close(fd_uid) < 0 || close(fd_gid) < 0)
        write_all(2, "Can't close file\n", 17);
    free(data);
    return 0;
}
