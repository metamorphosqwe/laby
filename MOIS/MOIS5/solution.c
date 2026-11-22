#include <stdio.h>
#include <stdlib.h>

#define MAX_N 205
#define MAX_M 40005

static int directed[MAX_N][MAX_N], reverse_graph[MAX_N][MAX_N];
static int visited[MAX_N], finish_order[MAX_N], order_size;
static int component[MAX_N], component_count;
static int edge_u[MAX_M], edge_v[MAX_M], edge_count;
static int head[MAX_N], adjacency_to[MAX_M * 2], adjacency_edge[MAX_M * 2];
static int adjacency_next[MAX_M * 2], adjacency_count;
static int tin[MAX_N], low[MAX_N], timer_value, parent_edge[MAX_N];
static int edge_stack[MAX_M], stack_size;
static int articulation[MAX_N], bridge[MAX_M];
static unsigned char block_vertices[MAX_M][MAX_N];
static int block_count;

static void dfs_order(int v, int n) {
    visited[v] = 1;
    for (int to = 1; to <= n; ++to)
        if (directed[v][to] && !visited[to]) dfs_order(to, n);
    finish_order[order_size++] = v;
}

static void dfs_component(int v, int n, int id) {
    component[v] = id;
    for (int to = 1; to <= n; ++to)
        if (reverse_graph[v][to] && !component[to]) dfs_component(to, n, id);
}

static void add_adjacency(int u, int v, int edge_id) {
    int a = adjacency_count++;
    adjacency_to[a] = v;
    adjacency_edge[a] = edge_id;
    adjacency_next[a] = head[u];
    head[u] = a;
}

static void save_block_until(int stop_edge) {
    if (block_count >= MAX_M) return;
    while (stack_size > 0) {
        int id = edge_stack[--stack_size];
        block_vertices[block_count][edge_u[id]] = 1;
        block_vertices[block_count][edge_v[id]] = 1;
        if (id == stop_edge) break;
    }
    ++block_count;
}

static void dfs_blocks(int v) {
    tin[v] = low[v] = ++timer_value;
    int children = 0;
    for (int a = head[v]; a != -1; a = adjacency_next[a]) {
        int to = adjacency_to[a], id = adjacency_edge[a];
        if (id == parent_edge[v]) continue;
        if (!tin[to]) {
            ++children;
            parent_edge[to] = id;
            edge_stack[stack_size++] = id;
            dfs_blocks(to);
            if (low[to] < low[v]) low[v] = low[to];
            if (low[to] > tin[v]) bridge[id] = 1;
            if (low[to] >= tin[v]) {
                if (parent_edge[v] != -1 || children > 1) articulation[v] = 1;
                save_block_until(id);
            }
        } else if (tin[to] < tin[v]) {
            edge_stack[stack_size++] = id;
            if (tin[to] < low[v]) low[v] = tin[to];
        }
    }
}

static int compare_ints(const void *a, const void *b) {
    return *(const int *)a - *(const int *)b;
}

int main(int argc, char **argv) {
    FILE *input = stdin;
    int n1, m1, n2, m2;
    if (argc > 1) {
        input = fopen(argv[1], "r");
        if (!input) { perror(argv[1]); return 1; }
    }
    if (fscanf(input, "%d%d", &n1, &m1) != 2 ||
        n1 < 1 || n1 >= MAX_N || m1 < 0 || m1 >= MAX_M) {
        fprintf(stderr, "Invalid directed graph size\n");
        return 1;
    }
    for (int i = 0; i < m1; ++i) {
        int u, v;
        if (fscanf(input, "%d%d", &u, &v) != 2 ||
            u < 1 || u > n1 || v < 1 || v > n1) {
            fprintf(stderr, "Invalid directed edge\n");
            return 1;
        }
        directed[u][v] = reverse_graph[v][u] = 1;
    }
    if (fscanf(input, "%d%d", &n2, &m2) != 2 ||
        n2 < 1 || n2 >= MAX_N || m2 < 0 || m2 >= MAX_M) {
        fprintf(stderr, "Invalid undirected graph size\n");
        return 1;
    }
    for (int i = 1; i <= n2; ++i) head[i] = -1;
    edge_count = m2;
    for (int i = 0; i < m2; ++i) {
        int u, v;
        if (fscanf(input, "%d%d", &u, &v) != 2 ||
            u < 1 || u > n2 || v < 1 || v > n2 || u == v) {
            fprintf(stderr, "Invalid undirected edge\n");
            return 1;
        }
        edge_u[i] = u;
        edge_v[i] = v;
        add_adjacency(u, v, i);
        add_adjacency(v, u, i);
    }
    if (input != stdin) fclose(input);

    for (int v = 1; v <= n1; ++v)
        if (!visited[v]) dfs_order(v, n1);
    for (int i = order_size - 1; i >= 0; --i)
        if (!component[finish_order[i]])
            dfs_component(finish_order[i], n1, ++component_count);
    printf("SCC: %d\n", component_count);
    for (int id = 1; id <= component_count; ++id) {
        printf("C%d:", id);
        for (int v = 1; v <= n1; ++v)
            if (component[v] == id) printf(" %d", v);
        putchar('\n');
    }

    for (int v = 1; v <= n2; ++v) {
        if (!tin[v]) {
            if (head[v] == -1) {
                tin[v] = low[v] = ++timer_value;
                block_vertices[block_count++][v] = 1;
            } else {
                parent_edge[v] = -1;
                dfs_blocks(v);
            }
        }
    }
    printf("Articulation:");
    int any = 0;
    for (int v = 1; v <= n2; ++v)
        if (articulation[v]) { printf(" %d", v); any = 1; }
    if (!any) printf(" none");
    printf("\nBridges:");
    any = 0;
    for (int i = 0; i < edge_count; ++i)
        if (bridge[i]) { printf(" %d-%d", edge_u[i], edge_v[i]); any = 1; }
    if (!any) printf(" none");
    printf("\nBlocks: %d\n", block_count);
    for (int b = 0; b < block_count; ++b) {
        int vertices[MAX_N], count = 0;
        for (int v = 1; v <= n2; ++v)
            if (block_vertices[b][v]) vertices[count++] = v;
        qsort(vertices, (size_t)count, sizeof(int), compare_ints);
        printf("B%d:", b + 1);
        for (int i = 0; i < count; ++i) printf(" %d", vertices[i]);
        putchar('\n');
    }
    return 0;
}
