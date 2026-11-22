#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_N 205
#define INF (LLONG_MAX / 4)

static long long graph[MAX_N][MAX_N];
static long long dist_dijkstra[MAX_N];
static long long dist_floyd[MAX_N][MAX_N];
static int parent_dijkstra[MAX_N];
static int next_floyd[MAX_N][MAX_N];

int main(int argc, char **argv) {
    FILE *input = stdin;
    int n, m, start = 1;
    if (argc > 1) {
        input = fopen(argv[1], "r");
        if (!input) {
            perror(argv[1]);
            return 1;
        }
    }

    if (fscanf(input, "%d%d", &n, &m) != 2 || n < 1 || n >= MAX_N || m < 0) {
        fprintf(stderr, "Invalid graph size\n");
        return 1;
    }
    for (int i = 1; i <= n; ++i)
        for (int j = 1; j <= n; ++j)
            graph[i][j] = i == j ? 0 : INF;

    for (int i = 0; i < m; ++i) {
        int u, v;
        long long w;
        if (fscanf(input, "%d%d%lld", &u, &v, &w) != 3 ||
            u < 1 || u > n || v < 1 || v > n || w < 0) {
            fprintf(stderr, "Invalid edge\n");
            return 1;
        }
        if (w < graph[u][v]) graph[u][v] = graph[v][u] = w;
    }
    if (fscanf(input, "%d", &start) != 1) start = 1;
    if (start < 1 || start > n) {
        fprintf(stderr, "Invalid source vertex\n");
        return 1;
    }
    if (input != stdin) fclose(input);

    int used[MAX_N] = {0};
    for (int i = 1; i <= n; ++i) {
        dist_dijkstra[i] = INF;
        parent_dijkstra[i] = -1;
    }
    dist_dijkstra[start] = 0;
    for (int step = 0; step < n; ++step) {
        int u = -1;
        for (int v = 1; v <= n; ++v)
            if (!used[v] && (u == -1 || dist_dijkstra[v] < dist_dijkstra[u]))
                u = v;
        if (u == -1 || dist_dijkstra[u] == INF) break;
        used[u] = 1;
        for (int v = 1; v <= n; ++v) {
            if (graph[u][v] != INF && dist_dijkstra[u] + graph[u][v] < dist_dijkstra[v]) {
                dist_dijkstra[v] = dist_dijkstra[u] + graph[u][v];
                parent_dijkstra[v] = u;
            }
        }
    }

    for (int i = 1; i <= n; ++i)
        for (int j = 1; j <= n; ++j) {
            dist_floyd[i][j] = graph[i][j];
            next_floyd[i][j] = graph[i][j] == INF ? -1 : j;
        }
    for (int k = 1; k <= n; ++k)
        for (int i = 1; i <= n; ++i)
            for (int j = 1; j <= n; ++j)
                if (dist_floyd[i][k] != INF && dist_floyd[k][j] != INF &&
                    dist_floyd[i][k] + dist_floyd[k][j] < dist_floyd[i][j]) {
                    dist_floyd[i][j] = dist_floyd[i][k] + dist_floyd[k][j];
                    next_floyd[i][j] = next_floyd[i][k];
                }

    printf("Dijkstra:");
    for (int i = 1; i <= n; ++i)
        if (dist_dijkstra[i] == INF) printf(" INF");
        else printf(" %lld", dist_dijkstra[i]);
    printf("\nFloyd-Warshall:");
    for (int i = 1; i <= n; ++i)
        if (dist_floyd[start][i] == INF) printf(" INF");
        else printf(" %lld", dist_floyd[start][i]);
    printf("\nDijkstra paths:");
    for (int destination = 1; destination <= n; ++destination) {
        printf(" %d=", destination);
        if (dist_dijkstra[destination] == INF) {
            printf("-");
            continue;
        }
        int path[MAX_N], length = 0, current = destination;
        while (current != start && current != -1 && length < n) {
            path[length++] = current;
            current = parent_dijkstra[current];
        }
        if (current != start) {
            printf("-");
            continue;
        }
        printf("%d", start);
        while (length > 0) printf("->%d", path[--length]);
    }
    printf("\nFloyd-Warshall paths:");
    for (int destination = 1; destination <= n; ++destination) {
        printf(" %d=", destination);
        if (next_floyd[start][destination] == -1) {
            printf("-");
            continue;
        }
        int current = start, steps = 0;
        printf("%d", current);
        while (current != destination && steps++ < n) {
            current = next_floyd[current][destination];
            if (current == -1) break;
            printf("->%d", current);
        }
        if (current == -1) printf("-");
    }
    putchar('\n');
    return 0;
}
