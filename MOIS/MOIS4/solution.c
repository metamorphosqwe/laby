#include <stdio.h>
#include <stdlib.h>

#define MAX_N 205
#define MAX_M 40005

typedef struct { int u, v, w, id; } Edge;
static Edge edges[MAX_M], sorted[MAX_M];
static int parent[MAX_N], rank_set[MAX_N];

static int compare_edges(const void *a, const void *b) {
    const Edge *x = (const Edge *)a, *y = (const Edge *)b;
    if (x->w != y->w) return x->w < y->w ? -1 : 1;
    if (x->u != y->u) return x->u < y->u ? -1 : 1;
    if (x->v != y->v) return x->v < y->v ? -1 : 1;
    return x->id - y->id;
}

static int find_set(int v) {
    if (parent[v] != v) parent[v] = find_set(parent[v]);
    return parent[v];
}

static int unite(int a, int b) {
    a = find_set(a);
    b = find_set(b);
    if (a == b) return 0;
    if (rank_set[a] < rank_set[b]) { int t = a; a = b; b = t; }
    parent[b] = a;
    if (rank_set[a] == rank_set[b]) ++rank_set[a];
    return 1;
}

int main(int argc, char **argv) {
    FILE *input = stdin;
    int n, m;
    if (argc > 1) {
        input = fopen(argv[1], "r");
        if (!input) { perror(argv[1]); return 1; }
    }
    if (fscanf(input, "%d%d", &n, &m) != 2 ||
        n < 1 || n >= MAX_N || m < 0 || m >= MAX_M) {
        fprintf(stderr, "Invalid graph size\n");
        return 1;
    }
    for (int i = 0; i < m; ++i) {
        if (fscanf(input, "%d%d%d", &edges[i].u, &edges[i].v, &edges[i].w) != 3 ||
            edges[i].u < 1 || edges[i].u > n || edges[i].v < 1 ||
            edges[i].v > n || edges[i].u == edges[i].v || edges[i].w < 0) {
            fprintf(stderr, "Invalid edge\n");
            return 1;
        }
        edges[i].id = i;
        sorted[i] = edges[i];
    }
    if (input != stdin) fclose(input);
    qsort(sorted, (size_t)m, sizeof(Edge), compare_edges);

    for (int i = 1; i <= n; ++i) parent[i] = i;
    long long kruskal_total = 0;
    int kruskal_count = 0;
    Edge kruskal_tree[MAX_N];
    for (int i = 0; i < m && kruskal_count < n - 1; ++i)
        if (unite(sorted[i].u, sorted[i].v)) {
            kruskal_tree[kruskal_count++] = sorted[i];
            kruskal_total += sorted[i].w;
        }

    int included[MAX_M] = {0}, visited[MAX_N] = {0};
    Edge prim_tree[MAX_N];
    int prim_count = 0;
    long long prim_total = 0;
    visited[1] = 1;
    while (prim_count < n - 1) {
        int best = -1;
        for (int i = 0; i < m; ++i) {
            int crosses = visited[edges[i].u] != visited[edges[i].v];
            if (!included[i] && crosses &&
                (best == -1 || compare_edges(&edges[i], &edges[best]) < 0))
                best = i;
        }
        if (best == -1) break;
        included[best] = 1;
        visited[edges[best].u] = visited[edges[best].v] = 1;
        prim_tree[prim_count++] = edges[best];
        prim_total += edges[best].w;
    }

    if (prim_count != n - 1 || kruskal_count != n - 1) {
        printf("DISCONNECTED\n");
        return 0;
    }
    printf("Prim: %lld\n", prim_total);
    for (int i = 0; i < prim_count; ++i)
        printf("%d %d %d\n", prim_tree[i].u, prim_tree[i].v, prim_tree[i].w);
    printf("Kruskal: %lld\n", kruskal_total);
    for (int i = 0; i < kruskal_count; ++i)
        printf("%d %d %d\n", kruskal_tree[i].u, kruskal_tree[i].v, kruskal_tree[i].w);
    return 0;
}
