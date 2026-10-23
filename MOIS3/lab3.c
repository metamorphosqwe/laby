/*
 * Лабораторная работа №3
 * Нахождение кратчайшего пути в графе.
 *
 * Программа вычисляет кратчайшие пути от вершины 1 до всех остальных
 * вершин двумя алгоритмами:
 *   1) алгоритмом Дейкстры (веса рёбер должны быть неотрицательными);
 *   2) алгоритмом Флойда-Уоршолла (допускает отрицательные веса,
 *      обнаруживает циклы отрицательного веса).
 * В конце результаты двух алгоритмов сравниваются.
 *
 * Запуск:
 *     lab3 graph.txt
 *
 * Формат входного файла:
 *     n m          <- количество вершин и количество рёбер
 *     u1 v1 w1     <- ребро между вершинами u1 и v1 с весом w1
 *     ...
 *     um vm wm
 * Вершины нумеруются числами от 1 до n. Граф неориентированный.
 */

#include <stdio.h> /* printf, fopen, fscanf, fclose */

#define MAX_V 20          /* максимальное число вершин */
#define MAX_E 100         /* максимальное число рёбер */
#define INF 1000000000    /* "бесконечность": пути не существует */

/* ---------------------- Глобальные данные ---------------------- */

/* Матрица весов: weight[i][j] - вес ребра между вершинами i и j,
 * либо INF, если ребра нет. Индексы с нуля (вершина 1 -> индекс 0). */
int weight[MAX_V][MAX_V];

/* Признак того, что во входном файле встретилось ребро с отрицательным весом.
 * Для алгоритма Дейкстры это недопустимо. */
int has_negative = 0;

/* Результаты алгоритма Дейкстры:
 *   dijk_dist[v] - длина кратчайшего пути от стартовой вершины до v;
 *   dijk_prev[v] - вершина, предшествующая v на этом пути (для восстановления
 *                  самого пути); -1, если предшественника нет. */
int dijk_dist[MAX_V];
int dijk_prev[MAX_V];

/* Результаты алгоритма Флойда-Уоршолла:
 *   fw_dist[i][j] - кратчайшее расстояние между i и j;
 *   fw_next[i][j] - первая вершина после i на кратчайшем пути в j
 *                   (для восстановления пути); -1, если пути нет. */
int fw_dist[MAX_V][MAX_V];
int fw_next[MAX_V][MAX_V];

/* ---------------------- Чтение графа ---------------------- */

/* Читает граф из файла и заполняет матрицу весов.
 * filename - имя файла; n_out, m_out - сюда записываются n и m.
 * Возвращает 1 при успехе и 0 при ошибке (нет файла, неверный формат). */
int read_graph(const char *filename, int *n_out, int *m_out)
{
    FILE *f = fopen(filename, "r");
    if (!f)
        return 0; /* файл не открылся */

    int n = 0;
    int m = 0;
    if (fscanf(f, "%d %d", &n, &m) != 2 || n < 1 || n > MAX_V || m < 0 || m > MAX_E)
    {
        fclose(f);
        return 0;
    }

    /* Сначала считаем, что рёбер нет: везде INF, на диагонали 0
     * (расстояние от вершины до самой себя равно нулю) */
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
            weight[i][j] = (i == j) ? 0 : INF;
    }

    for (int i = 0; i < m; i++)
    {
        int u = 0;
        int v = 0;
        int w = 0;
        if (fscanf(f, "%d %d %d", &u, &v, &w) != 3 || u < 1 || u > n || v < 1 || v > n)
        {
            fclose(f);
            return 0; /* строка с ребром неверная */
        }
        u--; /* переходим к индексам с нуля */
        v--;

        /* Граф неориентированный: вес записывается симметрично.
         * Если между вершинами несколько рёбер, оставляем наименьшее. */
        if (w < weight[u][v])
        {
            weight[u][v] = w;
            weight[v][u] = w;
        }
        if (w < 0)
            has_negative = 1;
    }
    fclose(f);

    *n_out = n;
    *m_out = m;
    return 1;
}

/* Печатает матрицу весов (INF выводится как "-"). */
void print_weights(int n)
{
    printf("Weight matrix:\n     ");
    for (int j = 0; j < n; j++)
        printf("%4d", j + 1);
    printf("\n");
    for (int i = 0; i < n; i++)
    {
        printf("%4d ", i + 1);
        for (int j = 0; j < n; j++)
        {
            if (weight[i][j] == INF)
                printf("%4s", "-");
            else
                printf("%4d", weight[i][j]);
        }
        printf("\n");
    }
}

/* ---------------------- Алгоритм Дейкстры ---------------------- */

/* Печатает одну строку вспомогательной таблицы алгоритма Дейкстры.
 * Для вершин с окончательно найденным расстоянием ставится "*",
 * для недостижимых пока вершин выводится "inf". */
void print_dijkstra_row(int step, int n, const int fixed[])
{
    printf("%4d |", step);
    for (int v = 0; v < n; v++)
    {
        char cell[16];
        if (dijk_dist[v] == INF)
            snprintf(cell, sizeof(cell), "inf");
        else if (fixed[v])
            snprintf(cell, sizeof(cell), "%d*", dijk_dist[v]);
        else
            snprintf(cell, sizeof(cell), "%d", dijk_dist[v]);
        printf("%6s", cell);
    }
    printf("\n");
}

/* Алгоритм Дейкстры (простая реализация за O(n^2) с массивами).
 * start - стартовая вершина (индекс с нуля).
 * Идея: на каждом шаге выбираем из ещё не обработанных вершин ту, до которой
 * найденное расстояние минимально; оно уже окончательное (так как нет
 * отрицательных весов). Затем пытаемся улучшить расстояния до её соседей. */
void dijkstra(int n, int start)
{
    int fixed[MAX_V]; /* fixed[v] = 1: расстояние до v уже окончательное */

    /* Начальные значения: до всех вершин пути пока нет */
    for (int v = 0; v < n; v++)
    {
        dijk_dist[v] = INF;
        dijk_prev[v] = -1;
        fixed[v] = 0;
    }
    dijk_dist[start] = 0; /* до самой себя расстояние 0 */

    printf("Dijkstra table (* - final distance):\n");
    printf("Step |");
    for (int v = 0; v < n; v++)
        printf("%6d", v + 1);
    printf("\n");

    /* Нужно n шагов: на каждом фиксируем ровно одну вершину */
    for (int step = 1; step <= n; step++)
    {
        /* 1) Ищем необработанную вершину с минимальным расстоянием */
        int best = -1;
        for (int v = 0; v < n; v++)
        {
            if (!fixed[v] && dijk_dist[v] < INF && (best == -1 || dijk_dist[v] < dijk_dist[best]))
                best = v;
        }
        if (best == -1)
            break; /* остальные вершины недостижимы */

        /* 2) Фиксируем её расстояние как окончательное */
        fixed[best] = 1;

        /* 3) Релаксация: проверяем, не короче ли путь через best */
        for (int to = 0; to < n; to++)
        {
            if (weight[best][to] != INF && !fixed[to] && dijk_dist[best] + weight[best][to] < dijk_dist[to])
            {
                dijk_dist[to] = dijk_dist[best] + weight[best][to];
                dijk_prev[to] = best; /* запоминаем, откуда пришли */
            }
        }

        print_dijkstra_row(step, n, fixed);
    }
}

/* Печатает путь до вершины v, используя массив предшественников.
 * Рекурсия идёт от v назад к старту, а печать - при возврате, поэтому
 * путь выводится в правильном порядке (от старта к v). */
void print_dijkstra_path(int v)
{
    if (dijk_prev[v] != -1)
    {
        print_dijkstra_path(dijk_prev[v]);
        printf(" -> ");
    }
    printf("%d", v + 1);
}

/* ---------------------- Алгоритм Флойда-Уоршолла ---------------------- */

/* Алгоритм Флойда-Уоршолла: находит кратчайшие пути между всеми парами
 * вершин за O(n^3). Идея: по очереди разрешаем использовать вершину k
 * как промежуточную и обновляем расстояния по формуле
 *     dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j]).
 * Возвращает 1, если найден цикл отрицательного веса (тогда результат
 * недостоверен), иначе 0. */
int floyd_warshall(int n)
{
    /* Начальные значения: расстояния равны весам рёбер.
     * next[i][j] = j, если между i и j есть прямое ребро. */
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            fw_dist[i][j] = weight[i][j];
            fw_next[i][j] = (weight[i][j] != INF && i != j) ? j : -1;
        }
    }

    /* Три вложенных цикла: k - промежуточная вершина, i - начало, j - конец */
    for (int k = 0; k < n; k++)
    {
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                /* Проверки на INF нужны, чтобы не складывать "бесконечности" */
                if (fw_dist[i][k] != INF && fw_dist[k][j] != INF && fw_dist[i][k] + fw_dist[k][j] < fw_dist[i][j])
                {
                    fw_dist[i][j] = fw_dist[i][k] + fw_dist[k][j];
                    fw_next[i][j] = fw_next[i][k]; /* первый шаг пути i -> k */
                }
            }
        }
    }

    /* Если расстояние от вершины до самой себя стало отрицательным,
     * в графе есть цикл отрицательного веса */
    for (int i = 0; i < n; i++)
    {
        if (fw_dist[i][i] < 0)
            return 1;
    }
    return 0;
}

/* Печатает итоговую матрицу расстояний Флойда-Уоршолла. */
void print_floyd_matrix(int n)
{
    printf("Floyd-Warshall distance matrix:\n     ");
    for (int j = 0; j < n; j++)
        printf("%4d", j + 1);
    printf("\n");
    for (int i = 0; i < n; i++)
    {
        printf("%4d ", i + 1);
        for (int j = 0; j < n; j++)
        {
            if (fw_dist[i][j] == INF)
                printf("%4s", "inf");
            else
                printf("%4d", fw_dist[i][j]);
        }
        printf("\n");
    }
}

/* Печатает путь от вершины from до вершины to по матрице fw_next:
 * из текущей вершины идём в fw_next[текущая][to], пока не дойдём до to. */
void print_floyd_path(int from, int to)
{
    int cur = from;
    printf("%d", cur + 1);
    while (cur != to)
    {
        cur = fw_next[cur][to];
        printf(" -> %d", cur + 1);
    }
}

/* ---------------------- Главная функция ---------------------- */

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <file>\n", argv[0]);
        return 1;
    }

    int n = 0;
    int m = 0;
    if (!read_graph(argv[1], &n, &m))
    {
        printf("Cannot read file: %s\n", argv[1]);
        return 1;
    }

    printf("File: %s\n", argv[1]);
    printf("Vertices: %d, Edges: %d\n", n, m);
    print_weights(n);

    int start = 0; /* стартовая вершина x1 (индекс 0) */
    int dijkstra_ok = 0;

    /* ----- Алгоритм Дейкстры ----- */
    printf("\n=== Dijkstra from vertex 1 ===\n");
    if (has_negative)
    {
        /* Дейкстра даёт неверный результат при отрицательных весах */
        printf("Graph has negative edge weights: Dijkstra is not applicable\n");
    }
    else
    {
        dijkstra(n, start);
        dijkstra_ok = 1;
        printf("Shortest paths from vertex 1:\n");
        for (int v = 0; v < n; v++)
        {
            printf("1 -> %d: ", v + 1);
            if (dijk_dist[v] == INF)
                printf("no path\n");
            else
            {
                printf("dist = %d, path: ", dijk_dist[v]);
                print_dijkstra_path(v);
                printf("\n");
            }
        }
    }

    /* ----- Алгоритм Флойда-Уоршолла ----- */
    printf("\n=== Floyd-Warshall ===\n");
    if (floyd_warshall(n))
    {
        printf("Negative cycle detected: shortest paths are not defined\n");
        return 0;
    }
    print_floyd_matrix(n);
    printf("Shortest paths from vertex 1:\n");
    for (int v = 0; v < n; v++)
    {
        printf("1 -> %d: ", v + 1);
        if (fw_dist[start][v] == INF)
            printf("no path\n");
        else
        {
            printf("dist = %d, path: ", fw_dist[start][v]);
            print_floyd_path(start, v);
            printf("\n");
        }
    }

    /* ----- Сравнение результатов ----- */
    if (dijkstra_ok)
    {
        int same = 1;
        for (int v = 0; v < n; v++)
        {
            if (dijk_dist[v] != fw_dist[start][v])
                same = 0;
        }
        printf("\nDijkstra and Floyd-Warshall results %s\n", same ? "match" : "DIFFER");
    }
    return 0;
}
