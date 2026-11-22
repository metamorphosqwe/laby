#include <stdio.h>

#define MAX_N 205
#define MAX_M 40005

static int adjacency[MAX_N][MAX_N];
static int edge_u[MAX_M], edge_v[MAX_M];
static int bfs_component[MAX_N], dfs_component[MAX_N];
static int queue[MAX_N];

static int read_graph(FILE *input, int *n, int *m) {
    if (fscanf(input, "%d%d", n, m) != 2 ||
        *n < 1 || *n >= MAX_N || *m < 0 || *m >= MAX_M)
        return 0;
    for (int i = 0; i < *m; ++i) {
        int u, v;
        if (fscanf(input, "%d%d", &u, &v) != 2 ||
            u < 1 || u > *n || v < 1 || v > *n || u == v)
            return 0;
        edge_u[i] = u;
        edge_v[i] = v;
        ++adjacency[u][v];
        ++adjacency[v][u];
    }
    return 1;
}

static int bfs_components(int n) {
    int count = 0;
    for (int start = 1; start <= n; ++start) {
        if (bfs_component[start]) continue;
        ++count;
        int head = 0, tail = 0;
        queue[tail++] = start;
        bfs_component[start] = count;
        while (head < tail) {
            int u = queue[head++];
            for (int v = 1; v <= n; ++v) {
                if (adjacency[u][v] && !bfs_component[v]) {
                    bfs_component[v] = count;
                    queue[tail++] = v;
                }
            }
        }
    }
    return count;
}

static void dfs_visit(int n, int u, int component) {
    dfs_component[u] = component;
    for (int v = 1; v <= n; ++v)
        if (adjacency[u][v] && !dfs_component[v])
            dfs_visit(n, v, component);
}

static int dfs_components(int n) {
    int count = 0;
    for (int v = 1; v <= n; ++v)
        if (!dfs_component[v]) dfs_visit(n, v, ++count);
    return count;
}

static void print_components(const int *component, int n, int count) {
    for (int id = 1; id <= count; ++id) {
        printf("C%d:", id);
        for (int v = 1; v <= n; ++v)
            if (component[v] == id) printf(" %d", v);
        putchar('\n');
    }
}

int main(int argc, char **argv) {
    FILE *input = stdin;
    int n, m;
    if (argc > 1) {
        input = fopen(argv[1], "r");
        if (!input) {
            perror(argv[1]);
            return 1;
        }
    }
    if (!read_graph(input, &n, &m)) {
        fprintf(stderr, "Invalid graph input\n");
        if (input != stdin) fclose(input);
        return 1;
    }
    if (input != stdin) fclose(input);

    printf("Vertices: %d Edges: %d\n", n, m);
    printf("Adjacency matrix:\n   ");
    for (int v = 1; v <= n; ++v) printf(" %d", v);
    putchar('\n');
    for (int u = 1; u <= n; ++u) {
        printf("%d:", u);
        for (int v = 1; v <= n; ++v) printf(" %d", adjacency[u][v]);
        putchar('\n');
    }

    printf("Incidence matrix:\n   ");
    for (int e = 1; e <= m; ++e) printf(" E%d", e);
    putchar('\n');
    for (int v = 1; v <= n; ++v) {
        printf("%d:", v);
        for (int e = 0; e < m; ++e)
            printf(" %d", edge_u[e] == v || edge_v[e] == v);
        putchar('\n');
    }

    int bfs_count = bfs_components(n);
    int dfs_count = dfs_components(n);
    printf("BFS components: %d\n", bfs_count);
    print_components(bfs_component, n, bfs_count);
    printf("DFS components: %d\n", dfs_count);
    print_components(dfs_component, n, dfs_count);
    return 0;
}
