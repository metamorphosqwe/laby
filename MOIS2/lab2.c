/*
 * Лабораторная работа №2
 * Нахождение эйлеровых и гамильтоновых циклов в неориентированном графе.
 *
 * Запуск:
 *     lab2 euler    graph.txt   - найти эйлеров цикл
 *     lab2 hamilton graph.txt   - найти гамильтонов цикл
 *
 * Формат входного файла:
 *     m            <- количество рёбер
 *     u1 v1        <- первое ребро (названия вершин: цифры или буквы)
 *     u2 v2
 *     ...
 *     um vm
 * Количество вершин n программа определяет сама по числу различных
 * названий вершин в файле. Вершины нумеруются внутри программы с 0
 * в порядке сортировки названий (a, b, c ... или 1, 2, 3 ...).
 */

#include <stdio.h>   /* printf, fopen, fscanf, fclose */
#include <string.h>  /* strcmp, strcpy, strlen */

#define MAX_V 50     /* максимальное число вершин */
#define MAX_E 200    /* максимальное число рёбер */

/* ---------------------- Глобальные данные ---------------------- */

/* Матрица смежности: adjacency[i][j] = количество рёбер между i и j.
 * Для простого графа там 0 или 1. */
int adjacency[MAX_V][MAX_V];

/* degree[i] - степень вершины i (число рёбер, выходящих из неё). */
int degree[MAX_V];

/* labels[i] - исходное название вершины i ("1", "a" и т.д.).
 * Нужно, чтобы выводить в ответе привычные названия, а не индексы. */
char labels[MAX_V][8];

/* Список рёбер: i-е ребро соединяет вершины edge_u[i] и edge_v[i]
 * (уже индексы с нуля, а не названия). */
int edge_u[MAX_E];
int edge_v[MAX_E];

/* visited[i] = 1, если вершину i уже посетил обход (проверка связности). */
int visited[MAX_V];

/* Результат алгоритма Хирхольцера: последовательность вершин эйлерова
 * цикла. Для m рёбер в ней m + 1 вершина (первая и последняя совпадают). */
int euler_cycle[MAX_E + 1];

/* Для поиска гамильтонова цикла:
 * hamilton_path[k] - вершина, стоящая на k-й позиции строящегося цикла;
 * hamilton_used[v] - 1, если вершина v уже включена в текущий путь. */
int hamilton_path[MAX_V];
int hamilton_used[MAX_V];

/* ---------------------- Чтение графа ---------------------- */

/* Сравнение названий вершин для сортировки.
 * Сначала сравниваем длину (чтобы "2" было меньше "10"),
 * при равной длине - обычное сравнение строк.
 * Возвращает 1, если a должно стоять раньше b. */
int label_less(const char *a, const char *b)
{
    int la = (int)strlen(a);
    int lb = (int)strlen(b);
    if (la != lb)
        return la < lb;
    return strcmp(a, b) < 0;
}

/* Читает граф из файла.
 * filename - имя файла;
 * n_out    - сюда записывается число вершин;
 * m_out    - сюда записывается число рёбер.
 * Возвращает 1 при успехе и 0 при любой ошибке (нет файла, неверный формат). */
int read_graph(const char *filename, int *n_out, int *m_out)
{
    FILE *f = fopen(filename, "r");
    if (!f)
        return 0; /* файл не открылся */

    /* Первое число в файле - количество рёбер */
    int m = 0;
    if (fscanf(f, "%d", &m) != 1 || m < 1 || m > MAX_E)
    {
        fclose(f);
        return 0;
    }

    /* Временно хранится названия концов каждого ребра в виде строк:
     * a[i] и b[i] - концы i-го ребра. */
    char a[MAX_E][8];
    char b[MAX_E][8];
    int n = 0; /* сколько различных вершин уже встретилось */

    for (int i = 0; i < m; i++)
    {
        /* %7s - читаем слово не длиннее 7 символов (8-й байт под '\0') */
        if (fscanf(f, "%7s %7s", a[i], b[i]) != 2)
        {
            fclose(f);
            return 0; /* рёбер в файле меньше, чем указано в первой строке */
        }

        /* Обрабатываем оба конца ребра одинаково */
        const char *pair[2] = {a[i], b[i]};
        for (int k = 0; k < 2; k++)
        {
            /* Ищем это название среди уже известных вершин */
            int found = 0;
            for (int j = 0; j < n; j++)
            {
                if (strcmp(labels[j], pair[k]) == 0)
                {
                    found = 1;
                    break;
                }
            }
            /* Если не нашли - это новая вершина, запоминаем её название */
            if (!found)
            {
                if (n >= MAX_V)
                {
                    fclose(f);
                    return 0; /* слишком много вершин */
                }
                strcpy(labels[n++], pair[k]);
            }
        }
    }
    fclose(f);

    /* Сортируем названия вершин (простая сортировка обменами),
     * чтобы индекс 0 всегда соответствовал "наименьшей" вершине (a или 1). */
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if (label_less(labels[j], labels[i]))
            {
                char t[8];
                strcpy(t, labels[i]);
                strcpy(labels[i], labels[j]);
                strcpy(labels[j], t);
            }
        }
    }

    /* Переводим названия концов рёбер в индексы вершин */
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (strcmp(labels[j], a[i]) == 0)
                edge_u[i] = j;
            if (strcmp(labels[j], b[i]) == 0)
                edge_v[i] = j;
        }
    }

    *n_out = n;
    *m_out = m;
    return 1;
}

/* ---------------------- Построение матрицы смежности ---------------------- */

/* Заполняет adjacency и degree по списку рёбер.
 * n - число вершин, m - число рёбер. */
void build_adjacency(int n, int m)
{
    /* Обнуляем матрицу и степени перед заполнением */
    for (int i = 0; i < n; i++)
    {
        degree[i] = 0;
        for (int j = 0; j < n; j++)
            adjacency[i][j] = 0;
    }

    for (int i = 0; i < m; i++)
    {
        int u = edge_u[i];
        int v = edge_v[i];

        /* Граф неориентированный, поэтому матрица симметрична:
         * ребро (u,v) записывается и в [u][v], и в [v][u]. */
        adjacency[u][v]++;
        if (u != v)
            adjacency[v][u]++;

        /* Каждое ребро увеличивает степень обоих концов
         * (петля увеличивает степень своей вершины на 2). */
        degree[u]++;
        degree[v]++;
    }
}

/* Печатает список рёбер, матрицу смежности и степени вершин. */
void print_graph(int n, int m)
{
    printf("Vertices: %d, Edges: %d\n", n, m);

    printf("Edges:");
    for (int i = 0; i < m; i++)
        printf(" (%s,%s)", labels[edge_u[i]], labels[edge_v[i]]);

    /* Шапка матрицы: названия столбцов */
    printf("\nAdjacency matrix:\n    ");
    for (int j = 0; j < n; j++)
        printf("%-3s ", labels[j]);
    printf("\n");

    /* Строки матрицы: название вершины и её строка смежности */
    for (int i = 0; i < n; i++)
    {
        printf("%-3s ", labels[i]);
        for (int j = 0; j < n; j++)
            printf("%-3d ", adjacency[i][j]);
        printf("\n");
    }

    printf("Degrees:");
    for (int i = 0; i < n; i++)
        printf(" %s=%d", labels[i], degree[i]);
    printf("\n");
}

/* ---------------------- Проверка связности ---------------------- */

/* Проверяет, что все вершины, имеющие рёбра, лежат в одной компоненте
 * связности. Используется обход в глубину (DFS) со стеком.
 * Возвращает 1, если граф связный, иначе 0. */
int is_connected(int n)
{
    int stack[MAX_V]; /* стек вершин, которые надо обработать */
    int top = 0;      /* top - сколько элементов сейчас в стеке */
    int start = -1;   /* вершина, с которой начинаем обход */

    for (int i = 0; i < n; i++)
        visited[i] = 0;

    /* Стартуем с первой вершины, у которой есть рёбра */
    for (int i = 0; i < n; i++)
    {
        if (degree[i] > 0)
        {
            start = i;
            break;
        }
    }
    if (start == -1)
        return 1; /* рёбер нет совсем - считаем граф связным */

    stack[top++] = start;
    visited[start] = 1;

    while (top > 0)
    {
        int v = stack[--top]; /* берём вершину с вершины стека */
        for (int to = 0; to < n; to++)
        {
            /* Если есть ребро v-to и to ещё не посещена - помечаем её
             * и кладём в стек */
            if (adjacency[v][to] && !visited[to])
            {
                visited[to] = 1;
                stack[top++] = to;
            }
        }
    }

    /* Если хоть одна вершина осталась непосещённой - граф несвязный */
    for (int i = 0; i < n; i++)
    {
        if (!visited[i])
            return 0;
    }
    return 1;
}

/* ---------------------- Эйлеров цикл ---------------------- */

/* Проверка критерия: эйлеров цикл существует тогда и только тогда, когда
 * граф связный и степень каждой вершины чётная.
 * Возвращает 1, если критерий выполнен, иначе 0 (с пояснением причины). */
int has_euler_cycle(int n)
{
    if (!is_connected(n))
    {
        printf("Graph is not connected\n");
        return 0;
    }
    for (int i = 0; i < n; i++)
    {
        if (degree[i] % 2 != 0)
        {
            printf("Vertex %s has odd degree (%d)\n", labels[i], degree[i]);
            return 0;
        }
    }
    return 1;
}

/* Построение эйлерова цикла итеративным алгоритмом Хирхольцера, O(E).
 * Идея: идём по неиспользованным рёбрам вперёд, пока не упрёмся в тупик
 * (вершину без свободных рёбер). Тупиковую вершину записываем в ответ и
 * возвращаемся назад по стеку, проверяя, нет ли там ещё свободных рёбер.
 * Ответ получается в обратном порядке, но для неориентированного графа
 * обратный цикл - тоже верный эйлеров цикл.
 * Возвращает число вершин в цикле (m + 1) или 0 при ошибке. */
int find_euler_cycle(int n, int m)
{
    int work[MAX_V][MAX_V]; /* копия матрицы, из которой удаляем рёбра */
    int stack[MAX_E + 2];   /* стек текущего пути */
    int top = 0;
    int len = 0;            /* сколько вершин уже записано в ответ */

    /* Копируем матрицу смежности, чтобы не портить исходную */
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            work[i][j] = adjacency[i][j];

    stack[top++] = 0; /* начинаем с вершины с индексом 0 */

    while (top > 0)
    {
        int v = stack[top - 1]; /* текущая вершина (без удаления из стека) */
        int next = -1;

        /* Ищем любое ещё не пройденное ребро из v */
        for (int to = 0; to < n; to++)
        {
            if (work[v][to] > 0)
            {
                next = to;
                break;
            }
        }

        if (next == -1)
        {
            /* Свободных рёбер из v нет: вершина попадает в ответ,
             * а мы возвращаемся на шаг назад */
            euler_cycle[len++] = v;
            top--;
        }
        else
        {
            /* Проходим ребро v-next: удаляем его (в обе стороны)
             * и переходим в next */
            work[v][next]--;
            work[next][v]--;
            stack[top++] = next;
        }
    }

    /* Если пройдены все m рёбер, в ответе будет ровно m + 1 вершина */
    return len == m + 1 ? len : 0;
}

/* ---------------------- Гамильтонов цикл ---------------------- */

/* Рекурсивный поиск с возвратом (backtracking).
 * pos - позиция в цикле, которую сейчас заполняем (0 уже занята стартом).
 * Возвращает 1, если удалось достроить цикл, иначе 0. */
int hamilton_dfs(int n, int pos)
{
    int last = hamilton_path[pos - 1]; /* последняя вершина текущего пути */

    /* Все n вершин уже в пути: остаётся проверить, что последняя вершина
     * соединена ребром с первой - тогда путь замыкается в цикл */
    if (pos == n)
        return adjacency[last][hamilton_path[0]] > 0;

    /* Пробуем каждую вершину как следующую */
    for (int to = 0; to < n; to++)
    {
        /* Подходит, если есть ребро last-to и to ещё не использована */
        if (adjacency[last][to] > 0 && !hamilton_used[to])
        {
            hamilton_used[to] = 1;      /* включаем вершину в путь */
            hamilton_path[pos] = to;
            if (hamilton_dfs(n, pos + 1))
                return 1;               /* цикл найден - выходим */
            hamilton_used[to] = 0;      /* возврат: убираем вершину и пробуем другую */
        }
    }
    return 0; /* ни одно продолжение не привело к циклу */
}

/* Запускает поиск гамильтонова цикла, предварительно отсекая заведомо
 * неподходящие графы. Возвращает 1, если цикл найден, иначе 0. */
int find_hamilton_cycle(int n)
{
    /* Цикл возможен только при трёх и более вершинах */
    if (n < 3)
        return 0;

    /* Ранняя оптимизация: в гамильтоновом цикле у каждой вершины
     * есть два соседа по циклу, значит степень должна быть >= 2 */
    for (int i = 0; i < n; i++)
    {
        if (degree[i] < 2)
        {
            printf("Vertex %s has degree < 2\n", labels[i]);
            return 0;
        }
    }

    /* Несвязный граф не может иметь гамильтонов цикл */
    if (!is_connected(n))
    {
        printf("Graph is not connected\n");
        return 0;
    }

    /* Достаточное условие Дирака: deg(v) >= n/2 для всех вершин.
     * Записано как 2*deg < n, чтобы не работать с дробями.
     * Это только информация: если условие не выполнено, цикл всё равно
     * может существовать, поэтому перебор запускается в любом случае. */
    int dirac = 1;
    for (int i = 0; i < n; i++)
    {
        if (2 * degree[i] < n)
            dirac = 0;
    }
    printf("Dirac condition (deg >= n/2): %s\n", dirac ? "satisfied" : "not satisfied");

    /* Начальные значения: путь начинается с вершины 0 (цикл можно
     * начинать с любой вершины, поэтому достаточно одного старта) */
    for (int i = 0; i < n; i++)
        hamilton_used[i] = 0;
    hamilton_path[0] = 0;
    hamilton_used[0] = 1;

    return hamilton_dfs(n, 1);
}

/* ---------------------- Главная функция ---------------------- */

int main(int argc, char *argv[])
{
    /* Нужны два аргумента: режим (euler/hamilton) и имя файла */
    if (argc < 3)
    {
        printf("Usage: %s <euler|hamilton> <file>\n", argv[0]);
        return 1;
    }

    int n = 0; /* число вершин */
    int m = 0; /* число рёбер */
    if (!read_graph(argv[2], &n, &m))
    {
        printf("Cannot read file: %s\n", argv[2]);
        return 1;
    }

    build_adjacency(n, m);
    printf("File: %s\n", argv[2]);
    print_graph(n, m);

    if (strcmp(argv[1], "euler") == 0)
    {
        /* Сначала проверка критерия, и только потом построение цикла */
        if (!has_euler_cycle(n))
        {
            printf("Euler cycle does not exist\n");
            return 0;
        }
        int len = find_euler_cycle(n, m);
        printf("Euler cycle:");
        for (int i = 0; i < len; i++)
            printf(" %s", labels[euler_cycle[i]]);
        printf("\n");
    }
    else
    {
        if (!find_hamilton_cycle(n))
        {
            printf("Hamilton cycle does not exist\n");
            return 0;
        }
        printf("Hamilton cycle:");
        for (int i = 0; i < n; i++)
            printf(" %s", labels[hamilton_path[i]]);
        /* Замыкаем цикл: в конце снова стартовая вершина */
        printf(" %s\n", labels[hamilton_path[0]]);
    }
    return 0;
}
