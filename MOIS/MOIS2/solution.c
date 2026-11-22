#include <stdio.h>
#include <stdlib.h>

#define MAX_N 205
#define MAX_M 40005

typedef struct {
    int n;
    int m;
    int adjacency[MAX_N][MAX_N];
    int degree[MAX_N];
} Graph;

static int euler_stack[MAX_M + 1];
static int euler_cycle[MAX_M + 1];
static int hamilton_path[MAX_N];
static int hamilton_used[MAX_N];

static int read_graph(FILE *input, Graph *graph) {
    if (fscanf(input, "%d%d", &graph->n, &graph->m) != 2 ||
        graph->n < 1 || graph->n >= MAX_N || graph->m < 0 ||
        graph->m >= MAX_M) {
        return 0;
    }

    for (int i = 0; i < graph->m; ++i) {
        int u, v;
        if (fscanf(input, "%d%d", &u, &v) != 2 ||
            u < 1 || u > graph->n || v < 1 || v > graph->n || u == v) {
            return 0;
        }
        ++graph->adjacency[u][v];
        ++graph->adjacency[v][u];
        ++graph->degree[u];
        ++graph->degree[v];
    }
    return 1;
}

static int is_connected(const Graph *graph) {
    int visited[MAX_N] = {0};
    int queue[MAX_N], head = 0, tail = 0;
    queue[tail++] = 1;
    visited[1] = 1;

    while (head < tail) {
        int u = queue[head++];
        for (int v = 1; v <= graph->n; ++v) {
            if (graph->adjacency[u][v] && !visited[v]) {
                visited[v] = 1;
                queue[tail++] = v;
            }
        }
    }
    for (int v = 1; v <= graph->n; ++v)
        if (!visited[v]) return 0;
    return 1;
}

static int find_euler_cycle(const Graph *graph, int *cycle_length) {
    if (graph->m == 0 || !is_connected(graph)) return 0;
    for (int v = 1; v <= graph->n; ++v)
        if (graph->degree[v] % 2 != 0) return 0;

    int work[MAX_N][MAX_N];
    for (int i = 1; i <= graph->n; ++i)
        for (int j = 1; j <= graph->n; ++j)
            work[i][j] = graph->adjacency[i][j];

    int top = 0, length = 0;
    euler_stack[top++] = 1;
    while (top > 0) {
        int u = euler_stack[top - 1], next = 0;
        for (int v = 1; v <= graph->n; ++v)
            if (work[u][v] > 0) { next = v; break; }

        if (next == 0) {
            euler_cycle[length++] = u;
            --top;
        } else {
            --work[u][next];
            --work[next][u];
            euler_stack[top++] = next;
        }
    }
    if (length != graph->m + 1) return 0;

    *cycle_length = length;
    return 1;
}

static int hamilton_dfs(const Graph *graph, int position) {
    int last = hamilton_path[position - 1];
    if (position == graph->n)
        return graph->adjacency[last][hamilton_path[0]] > 0;

    for (int next = 1; next <= graph->n; ++next) {
        if (graph->adjacency[last][next] > 0 && !hamilton_used[next]) {
            hamilton_used[next] = 1;
            hamilton_path[position] = next;
            if (hamilton_dfs(graph, position + 1)) return 1;
            hamilton_used[next] = 0;
        }
    }
    return 0;
}

static int find_hamilton_cycle(const Graph *graph, int *dirac_satisfied) {
    *dirac_satisfied = graph->n >= 3;
    if (graph->n < 3 || !is_connected(graph)) return 0;

    int low_degree = 0;
    for (int v = 1; v <= graph->n; ++v) {
        if (2 * graph->degree[v] < graph->n) *dirac_satisfied = 0;
        if (graph->degree[v] < 2) low_degree = 1;
    }
    if (low_degree) return 0;

    for (int v = 1; v <= graph->n; ++v) hamilton_used[v] = 0;
    hamilton_path[0] = 1;
    hamilton_used[1] = 1;
    return hamilton_dfs(graph, 1);
}

static void print_graph(const Graph *graph, const char *name) {
    printf("%s: n=%d m=%d\nDegrees:", name, graph->n, graph->m);
    for (int v = 1; v <= graph->n; ++v)
        printf(" %d=%d", v, graph->degree[v]);
    putchar('\n');
}

int main(int argc, char **argv) {
    FILE *input = stdin;
    if (argc > 1) {
        input = fopen(argv[1], "r");
        if (!input) {
            perror(argv[1]);
            return 1;
        }
    }

    Graph first = {0}, second = {0};
    if (!read_graph(input, &first) || !read_graph(input, &second)) {
        fprintf(stderr, "Invalid input: expected two graphs as n m followed by edges\n");
        if (input != stdin) fclose(input);
        return 1;
    }
    if (input != stdin) fclose(input);

    print_graph(&first, "G1");
    int euler_length = 0;
    if (find_euler_cycle(&first, &euler_length)) {
        printf("Euler cycle:");
        for (int i = euler_length - 1; i >= 0; --i)
            printf(" %d", euler_cycle[i]);
        putchar('\n');
    } else {
        printf("Euler cycle: does not exist\n");
    }

    print_graph(&second, "G2");
    int dirac_satisfied = 0;
    int has_hamilton_cycle = find_hamilton_cycle(&second, &dirac_satisfied);
    printf("Dirac condition: %s\n", dirac_satisfied ? "satisfied" : "not satisfied");
    if (has_hamilton_cycle) {
        printf("Hamilton cycle:");
        for (int i = 0; i < second.n; ++i) printf(" %d", hamilton_path[i]);
        printf(" %d\n", hamilton_path[0]);
    } else {
        printf("Hamilton cycle: does not exist\n");
    }
    return 0;
}
