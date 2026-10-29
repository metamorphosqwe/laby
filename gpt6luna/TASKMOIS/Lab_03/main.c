#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_VERTICES 200
#define INF (LLONG_MAX / 4)

static long long weights[MAX_VERTICES][MAX_VERTICES];
static long long dijkstra_distance[MAX_VERTICES];
static int predecessor[MAX_VERTICES];
static int next_step[MAX_VERTICES][MAX_VERTICES];

/* В файле: число вершин, число рёбер, начальная вершина,
   затем рёбра и их веса. Вес не может быть отрицательным. */
static int read_graph(FILE *input, int *n, int *m, int *source) {
    int i, row, column;
    if (fscanf(input, "%d%d%d", n, m, source) != 3 ||
        *n < 1 || *n > MAX_VERTICES || *m < 0 ||
        *m > (*n * (*n - 1)) / 2 || *source < 1 || *source > *n)
        return 0;
    for (row = 0; row < *n; ++row)
        for (column = 0; column < *n; ++column)
            weights[row][column] = row == column ? 0 : INF;
    for (i = 0; i < *m; ++i) {
        int u, v;
        long long weight;
        if (fscanf(input, "%d%d%lld", &u, &v, &weight) != 3 ||
            u < 1 || u > *n || v < 1 || v > *n || u == v ||
            weight < 0 || weight >= INF)
            return 0;
        --u;
        --v;
        if (weight < weights[u][v]) {
            weights[u][v] = weight;
            weights[v][u] = weight;
        }
    }
    return 1;
}

/* Каждый раз выбираем ближайшую ещё не посещённую вершину.
   Запоминаем, откуда пришли, чтобы потом показать путь. */
static void run_dijkstra(int n, int source) {
    int visited[MAX_VERTICES] = {0};
    int i, step;
    for (i = 0; i < n; ++i) {
        dijkstra_distance[i] = INF;
        predecessor[i] = -1;
    }
    dijkstra_distance[source] = 0;
    for (step = 0; step < n; ++step) {
        int best = -1;
        for (i = 0; i < n; ++i)
            if (!visited[i] && (best == -1 ||
                                dijkstra_distance[i] < dijkstra_distance[best]))
                best = i;
        if (best == -1 || dijkstra_distance[best] == INF) break;
        visited[best] = 1;
        for (i = 0; i < n; ++i) {
            if (!visited[i] && weights[best][i] != INF &&
                dijkstra_distance[best] <= INF - weights[best][i] &&
                dijkstra_distance[best] + weights[best][i] <
                    dijkstra_distance[i]) {
                dijkstra_distance[i] =
                    dijkstra_distance[best] + weights[best][i];
                predecessor[i] = best;
            }
        }
    }
}

/* next_step помогает восстановить маршрут. Недостижимые расстояния
   не складываем, чтобы не получить неверный ответ. */
static void run_floyd(int n) {
    int i, j, k;
    for (i = 0; i < n; ++i)
        for (j = 0; j < n; ++j)
            next_step[i][j] = weights[i][j] == INF ? -1 : j;
    for (k = 0; k < n; ++k)
        for (i = 0; i < n; ++i)
            for (j = 0; j < n; ++j)
                if (weights[i][k] != INF && weights[k][j] != INF &&
                    weights[i][k] <= INF - weights[k][j] &&
                    weights[i][k] + weights[k][j] < weights[i][j]) {
                    weights[i][j] = weights[i][k] + weights[k][j];
                    next_step[i][j] = next_step[i][k];
                }
}

static void print_single_paths(int n, int source) {
    int target;
    puts("Дейкстра:");
    for (target = 0; target < n; ++target) {
        int path[MAX_VERTICES], length = 0, current = target, i;
        if (dijkstra_distance[target] == INF) {
            printf("%d: недостижима\n", target + 1);
            continue;
        }
        while (current != -1 && length < n) {
            path[length++] = current;
            if (current == source) break;
            current = predecessor[current];
        }
        if (length == 0 || path[length - 1] != source) {
            fprintf(stderr, "Ошибка восстановления пути Дейкстры.\n");
            continue;
        }
        printf("%d: расстояние %lld, путь ", target + 1,
               dijkstra_distance[target]);
        for (i = length - 1; i >= 0; --i)
            printf("%s%d", i == length - 1 ? "" : "->", path[i] + 1);
        putchar('\n');
    }
}

int main(int argc, char **argv) {
    FILE *input;
    int n, m, source, target;
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <файл.txt>\n", argv[0]);
        return 1;
    }
    input = fopen(argv[1], "r");
    if (input == NULL) {
        perror("Не удалось открыть файл");
        return 1;
    }
    if (!read_graph(input, &n, &m, &source)) {
        fprintf(stderr, "Ошибка входных данных графа.\n");
        fclose(input);
        return 1;
    }
    fclose(input);
    --source;

    run_dijkstra(n, source);
    run_floyd(n);
    print_single_paths(n, source);
    puts("Флойд—Уоршелл:");
    for (target = 0; target < n; ++target) {
        int current = source, steps = 0;
        if (weights[source][target] == INF) {
            printf("%d: недостижима\n", target + 1);
            continue;
        }
        printf("%d: расстояние %lld, путь %d", target + 1,
               weights[source][target], source + 1);
        while (current != target && steps++ < n) {
            current = next_step[current][target];
            if (current < 0) break;
            printf("->%d", current + 1);
        }
        if (current != target) {
            fprintf(stderr, "\nОшибка восстановления пути Флойда—Уоршелла.\n");
            return 1;
        }
        putchar('\n');
        if (dijkstra_distance[target] != weights[source][target]) {
            fprintf(stderr, "Ошибка: расстояния алгоритмов не совпадают.\n");
            return 1;
        }
    }
    (void)m;
    return 0;
}
