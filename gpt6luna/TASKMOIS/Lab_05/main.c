#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_V 100
#define MAX_E 500
#define MAX_ARCS (MAX_E * 2)

typedef struct {
    int from;
    int to;
} Edge;

typedef struct {
    int n;
    int m;
    Edge edge[MAX_E];
} Graph;

static int directed[MAX_V][MAX_V];
static int reversed[MAX_V][MAX_V];
static int visited[MAX_V];
static int finish_order[MAX_V];
static int finish_count;
static int component[MAX_V];
static int component_count;

static int head[MAX_V];
static int arc_to[MAX_ARCS];
static int arc_edge[MAX_ARCS];
static int arc_next[MAX_ARCS];
static int arc_count;
static int entered[MAX_V];
static int low[MAX_V];
static int articulation[MAX_V];
static int bridge[MAX_E];
static int edge_stack[MAX_E];
static int stack_size;
static int timer_value;

static int read_graph(FILE *input, Graph *graph, int is_directed) {
    int i;
    if (fscanf(input, "%d%d", &graph->n, &graph->m) != 2 ||
        graph->n < 1 || graph->n > MAX_V ||
        graph->m < 0 || graph->m > MAX_E)
        return 0;
    for (i = 0; i < graph->m; ++i) {
        int u, v;
        if (fscanf(input, "%d%d", &u, &v) != 2 ||
            u < 1 || u > graph->n || v < 1 || v > graph->n || u == v)
            return 0;
        graph->edge[i].from = u - 1;
        graph->edge[i].to = v - 1;
        if (is_directed) {
            directed[u - 1][v - 1] = 1;
            reversed[v - 1][u - 1] = 1;
        }
    }
    return 1;
}

/* Первый обход запоминает вершины в порядке завершения. */
static void dfs_order(int vertex, int n) {
    int next;
    visited[vertex] = 1;
    for (next = 0; next < n; ++next)
        if (directed[vertex][next] && !visited[next]) dfs_order(next, n);
    finish_order[finish_count++] = vertex;
}

/* Обходим граф наоборот: каждый такой обход находит одну группу вершин,
   которые достижимы друг из друга. */
static void dfs_component(int vertex, int n, int id) {
    int next;
    visited[vertex] = 1;
    component[vertex] = id;
    for (next = 0; next < n; ++next)
        if (reversed[vertex][next] && !visited[next])
            dfs_component(next, n, id);
}

static void find_scc(int n) {
    int vertex;
    memset(visited, 0, sizeof(visited));
    finish_count = 0;
    for (vertex = 0; vertex < n; ++vertex)
        if (!visited[vertex]) dfs_order(vertex, n);
    memset(visited, 0, sizeof(visited));
    component_count = 0;
    while (finish_count > 0) {
        vertex = finish_order[--finish_count];
        if (!visited[vertex])
            dfs_component(vertex, n, component_count++);
    }
}

static void add_arc(int from, int to, int edge_id) {
    arc_to[arc_count] = to;
    arc_edge[arc_count] = edge_id;
    arc_next[arc_count] = head[from];
    head[from] = arc_count++;
}

static void bcc_dfs(int vertex, int parent_edge, const Graph *graph) {
    int arc, child_count = 0;
    entered[vertex] = low[vertex] = ++timer_value;
    for (arc = head[vertex]; arc != -1; arc = arc_next[arc]) {
        int next = arc_to[arc];
        int edge_id = arc_edge[arc];
        if (edge_id == parent_edge) continue;
        if (entered[next] == 0) {
            ++child_count;
            edge_stack[stack_size++] = edge_id;
            bcc_dfs(next, edge_id, graph);
            if (low[next] < low[vertex]) low[vertex] = low[next];
            if (low[next] > entered[vertex]) bridge[edge_id] = 1;
            if (low[next] >= entered[vertex]) {
                if (parent_edge != -1 || child_count > 1)
                    articulation[vertex] = 1;
                printf("{");
                {
                    int id, first = 1;
                    do {
                        id = edge_stack[--stack_size];
                        printf("%s%d-%d", first ? "" : ", ",
                               graph->edge[id].from + 1,
                               graph->edge[id].to + 1);
                        first = 0;
                    } while (id != edge_id && stack_size > 0);
                }
                puts("}");
            }
        } else if (entered[next] < entered[vertex]) {
            edge_stack[stack_size++] = edge_id;
            if (entered[next] < low[vertex]) low[vertex] = entered[next];
        }
    }
    if (parent_edge == -1 && child_count == 0)
        printf("{вершина %d}\n", vertex + 1);
}

static void find_biconnected(const Graph *graph) {
    int i, vertex, found_articulation = 0;
    for (vertex = 0; vertex < graph->n; ++vertex) head[vertex] = -1;
    arc_count = 0;
    for (i = 0; i < graph->m; ++i) {
        add_arc(graph->edge[i].from, graph->edge[i].to, i);
        add_arc(graph->edge[i].to, graph->edge[i].from, i);
    }
    memset(entered, 0, sizeof(entered));
    memset(low, 0, sizeof(low));
    memset(articulation, 0, sizeof(articulation));
    memset(bridge, 0, sizeof(bridge));
    timer_value = stack_size = 0;
    puts("Компоненты двусвязности G2:");
    for (vertex = 0; vertex < graph->n; ++vertex)
        if (!entered[vertex]) bcc_dfs(vertex, -1, graph);

    printf("Точки сочленения:");
    for (vertex = 0; vertex < graph->n; ++vertex) {
        if (articulation[vertex]) {
            printf(" %d", vertex + 1);
            found_articulation = 1;
        }
    }
    if (!found_articulation) printf(" нет");
    putchar('\n');
    printf("Мосты:");
    for (i = 0; i < graph->m; ++i)
        if (bridge[i])
            printf(" %d-%d", graph->edge[i].from + 1,
                   graph->edge[i].to + 1);
    puts("");
}

int main(int argc, char **argv) {
    FILE *input;
    Graph g1, g2;
    int vertex, id;
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <файл.txt>\n", argv[0]);
        return 1;
    }
    input = fopen(argv[1], "r");
    if (input == NULL) {
        perror("Не удалось открыть файл");
        return 1;
    }
    if (!read_graph(input, &g1, 1) || !read_graph(input, &g2, 0)) {
        fprintf(stderr, "Ошибка формата: ожидаются два блока n m и рёбра.\n");
        fclose(input);
        return 1;
    }
    fclose(input);

    find_scc(g1.n);
    puts("Компоненты сильной связности G1:");
    for (id = 0; id < component_count; ++id) {
        printf("{");
        {
            int first = 1;
            for (vertex = 0; vertex < g1.n; ++vertex)
                if (component[vertex] == id) {
                    printf("%s%d", first ? "" : ", ", vertex + 1);
                    first = 0;
                }
        }
        puts("}");
    }
    find_biconnected(&g2);
    return 0;
}
