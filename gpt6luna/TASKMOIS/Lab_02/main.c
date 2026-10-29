#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_VERTICES 40
#define MAX_EDGES 800

typedef struct {
    int from;
    int to;
} Edge;

typedef struct {
    int vertex_count;
    int edge_count;
    Edge edges[MAX_EDGES];
    int degree[MAX_VERTICES];
    int adjacency[MAX_VERTICES][MAX_VERTICES];
} Graph;

/* Сначала читаем количество вершин и рёбер, затем сами рёбра.
   Если числа неверные, сообщаем об ошибке. */
static int read_graph(FILE *input, Graph *graph) {
    int i;
    memset(graph, 0, sizeof(*graph));
    if (fscanf(input, "%d%d", &graph->vertex_count, &graph->edge_count) != 2 ||
        graph->vertex_count < 1 || graph->vertex_count > MAX_VERTICES ||
        graph->edge_count < 0 || graph->edge_count > MAX_EDGES) {
        return 0;
    }
    for (i = 0; i < graph->edge_count; ++i) {
        int u, v;
        if (fscanf(input, "%d%d", &u, &v) != 2 || u < 1 ||
            u > graph->vertex_count || v < 1 || v > graph->vertex_count ||
            u == v || graph->adjacency[u - 1][v - 1]) {
            return 0;
        }
        graph->edges[i].from = u - 1;
        graph->edges[i].to = v - 1;
        graph->adjacency[u - 1][v - 1] = 1;
        graph->adjacency[v - 1][u - 1] = 1;
        ++graph->degree[u - 1];
        ++graph->degree[v - 1];
    }
    return 1;
}

/* Эйлеров цикл возможен, если степени чётные и все рёбра связаны
   между собой через вершины. */
static int euler_cycle(const Graph *graph, int *cycle) {
    int stack[MAX_EDGES + 1], used[MAX_EDGES] = {0};
    int next_edge[MAX_VERTICES] = {0};
    int adjacency_edge[MAX_VERTICES][MAX_VERTICES];
    int adjacency_to[MAX_VERTICES][MAX_VERTICES];
    int adjacency_count[MAX_VERTICES] = {0};
    int seen[MAX_VERTICES] = {0}, queue[MAX_VERTICES];
    int start = -1, head = 0, tail = 0, top = 0, count = 0, i, v;

    for (i = 0; i < graph->vertex_count; ++i) {
        if (graph->degree[i] % 2 != 0) return 0;
        if (graph->degree[i] > 0 && start == -1) start = i;
    }
    if (start == -1) {
        cycle[0] = 0;
        return graph->edge_count == 0;
    }

    for (i = 0; i < graph->edge_count; ++i) {
        int a = graph->edges[i].from, b = graph->edges[i].to;
        int pa = adjacency_count[a]++, pb = adjacency_count[b]++;
        adjacency_to[a][pa] = b;
        adjacency_edge[a][pa] = i;
        adjacency_to[b][pb] = a;
        adjacency_edge[b][pb] = i;
    }
    seen[start] = 1;
    queue[tail++] = start;
    while (head < tail) {
        v = queue[head++];
        for (i = 0; i < adjacency_count[v]; ++i) {
            int next = adjacency_to[v][i];
            if (!seen[next]) {
                seen[next] = 1;
                queue[tail++] = next;
            }
        }
    }
    for (i = 0; i < graph->vertex_count; ++i) {
        if (graph->degree[i] > 0 && !seen[i]) return 0;
    }

    /* Идём по неиспользованным рёбрам. Когда продолжения нет,
       добавляем вершину в готовый маршрут. */
    stack[top++] = start;
    while (top > 0) {
        int current = stack[top - 1];
        while (next_edge[current] < adjacency_count[current] &&
               used[adjacency_edge[current][next_edge[current]]]) {
            ++next_edge[current];
        }
        if (next_edge[current] == adjacency_count[current]) {
            cycle[count++] = current;
            --top;
        } else {
            int position = next_edge[current]++;
            int edge_id = adjacency_edge[current][position];
            if (!used[edge_id]) {
                used[edge_id] = 1;
                stack[top++] = adjacency_to[current][position];
            }
        }
    }
    if (count != graph->edge_count + 1) return 0;
    for (i = 0; i < count / 2; ++i) {
        int temp = cycle[i];
        cycle[i] = cycle[count - 1 - i];
        cycle[count - 1 - i] = temp;
    }
    return count;
}

static int hamilton_dfs(const Graph *graph, int current, int start,
                        int *path, int depth, int *visited) {
    int next;
    if (depth == graph->vertex_count) {
        if (graph->adjacency[current][start]) {
            path[depth] = start;
            return 1;
        }
        return 0;
    }
    for (next = 0; next < graph->vertex_count; ++next) {
        if (graph->adjacency[current][next] && !visited[next]) {
            visited[next] = 1;
            path[depth] = next;
            if (hamilton_dfs(graph, next, start, path, depth + 1, visited))
                return 1;
            visited[next] = 0;
        }
    }
    return 0;
}

static int hamilton_cycle(const Graph *graph, int *path) {
    int visited[MAX_VERTICES] = {0};
    int start, reached[MAX_VERTICES] = {0}, queue[MAX_VERTICES];
    int head = 0, tail = 0, vertex;
    if (graph->vertex_count < 3) return 0;
    /* Если граф не связан или у вершины меньше двух соседей,
       пройти все вершины циклом невозможно. */
    for (vertex = 0; vertex < graph->vertex_count; ++vertex)
        if (graph->degree[vertex] < 2) return 0;
    reached[0] = 1;
    queue[tail++] = 0;
    while (head < tail) {
        int current = queue[head++];
        for (vertex = 0; vertex < graph->vertex_count; ++vertex)
            if (graph->adjacency[current][vertex] && !reached[vertex]) {
                reached[vertex] = 1;
                queue[tail++] = vertex;
            }
    }
    for (start = 0; start < graph->vertex_count; ++start)
        if (!reached[start]) return 0;
    visited[0] = 1;
    path[0] = 0;
    return hamilton_dfs(graph, 0, 0, path, 1, visited);
}

static void print_cycle(const char *title, const int *path, int length) {
    int i;
    printf("%s", title);
    for (i = 0; i < length; ++i)
        printf("%s%d", i == 0 ? "" : "-", path[i] + 1);
    putchar('\n');
}

int main(int argc, char **argv) {
    FILE *input;
    Graph euler_graph, hamilton_graph;
    int euler_path[MAX_EDGES + 1], hamilton_path[MAX_VERTICES + 1];
    int result;
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <файл.txt>\n", argv[0]);
        return 1;
    }
    input = fopen(argv[1], "r");
    if (input == NULL) {
        perror("Не удалось открыть файл");
        return 1;
    }
    if (!read_graph(input, &euler_graph) || !read_graph(input, &hamilton_graph)) {
        fprintf(stderr, "Ошибка формата: ожидаются два блока n m и список рёбер.\n");
        fclose(input);
        return 1;
    }
    fclose(input);

    result = euler_cycle(&euler_graph, euler_path);
    if (result > 0) print_cycle("Эйлеров цикл G1: ", euler_path, result);
    else puts("Эйлеров цикл G1 отсутствует.");

    if (hamilton_cycle(&hamilton_graph, hamilton_path))
        print_cycle("Гамильтонов цикл G2: ", hamilton_path,
                    hamilton_graph.vertex_count + 1);
    else puts("Гамильтонов цикл G2 отсутствует.");
    return 0;
}
