#include <stdio.h>
#include <stdlib.h>

#define MAX_VERTICES 200
#define MAX_EDGES 20000

typedef struct {
    int from;
    int to;
    int weight;
} Edge;

static Edge edges[MAX_EDGES];
static int parent[MAX_VERTICES];
static int rank_value[MAX_VERTICES];

static int compare_edges(const void *left, const void *right) {
    const Edge *a = (const Edge *)left;
    const Edge *b = (const Edge *)right;
    if (a->weight < b->weight) return -1;
    if (a->weight > b->weight) return 1;
    if (a->from < b->from) return -1;
    if (a->from > b->from) return 1;
    return a->to - b->to;
}

/* Помогает быстро понять, соединены ли уже две вершины.
   Так Крускал не добавляет ребро, которое создаст круг. */
static int find_root(int vertex) {
    if (parent[vertex] != vertex)
        parent[vertex] = find_root(parent[vertex]);
    return parent[vertex];
}

static int unite(int a, int b) {
    a = find_root(a);
    b = find_root(b);
    if (a == b) return 0;
    if (rank_value[a] < rank_value[b]) {
        int temp = a;
        a = b;
        b = temp;
    }
    parent[b] = a;
    if (rank_value[a] == rank_value[b]) ++rank_value[a];
    return 1;
}

static int read_graph(FILE *input, int *n, int *m) {
    int i;
    if (fscanf(input, "%d%d", n, m) != 2 || *n < 1 ||
        *n > MAX_VERTICES || *m < 0 || *m > MAX_EDGES)
        return 0;
    for (i = 0; i < *m; ++i) {
        int u, v, weight;
        if (fscanf(input, "%d%d%d", &u, &v, &weight) != 3 ||
            u < 1 || u > *n || v < 1 || v > *n || u == v || weight < 0)
            return 0;
        edges[i].from = u - 1;
        edges[i].to = v - 1;
        edges[i].weight = weight;
    }
    return 1;
}

static int kruskal(int n, int m, Edge *tree, long long *total) {
    int i, count = 0;
    *total = 0;
    qsort(edges, (size_t)m, sizeof(edges[0]), compare_edges);
    for (i = 0; i < n; ++i) {
        parent[i] = i;
        rank_value[i] = 0;
    }
    for (i = 0; i < m && count < n - 1; ++i) {
        if (unite(edges[i].from, edges[i].to)) {
            tree[count++] = edges[i];
            *total += edges[i].weight;
        }
    }
    return count;
}

/* Прим начинает с первой вершины. Каждый раз он добавляет
   самое лёгкое ребро к новой вершине. */
static int prim(int n, int m, Edge *tree, long long *total) {
    int visited[MAX_VERTICES] = {0};
    int count = 0, step;
    *total = 0;
    visited[0] = 1;
    for (step = 1; step < n; ++step) {
        int best = -1, i;
        for (i = 0; i < m; ++i) {
            int crosses = visited[edges[i].from] != visited[edges[i].to];
            if (crosses && (best == -1 || edges[i].weight < edges[best].weight))
                best = i;
        }
        if (best == -1) return count;
        tree[count++] = edges[best];
        *total += edges[best].weight;
        visited[edges[best].from] = 1;
        visited[edges[best].to] = 1;
    }
    return count;
}

static void print_tree(const char *name, const Edge *tree, int count,
                       long long total) {
    int i;
    printf("%s:\n", name);
    for (i = 0; i < count; ++i)
        printf("%d-%d вес=%d\n", tree[i].from + 1, tree[i].to + 1,
               tree[i].weight);
    printf("Рёбер: %d; суммарный вес: %lld\n", count, total);
}

int main(int argc, char **argv) {
    FILE *input;
    int n, m, count_k, count_p;
    Edge kruskal_tree[MAX_VERTICES], prim_tree[MAX_VERTICES];
    long long total_k, total_p;
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <файл.txt>\n", argv[0]);
        return 1;
    }
    input = fopen(argv[1], "r");
    if (input == NULL) {
        perror("Не удалось открыть файл");
        return 1;
    }
    if (!read_graph(input, &n, &m)) {
        fprintf(stderr, "Ошибка входных данных графа.\n");
        fclose(input);
        return 1;
    }
    fclose(input);

    /* Крускал меняет порядок рёбер. Сохраняем список,
       чтобы затем Прим получил исходные данные. */
    {
        Edge original[MAX_EDGES];
        int i;
        for (i = 0; i < m; ++i) original[i] = edges[i];
        count_k = kruskal(n, m, kruskal_tree, &total_k);
        for (i = 0; i < m; ++i) edges[i] = original[i];
    }
    count_p = prim(n, m, prim_tree, &total_p);
    if (count_k != n - 1 || count_p != n - 1) {
        puts("Граф несвязен: остовное дерево не существует.");
        return 0;
    }
    print_tree("Крускал", kruskal_tree, count_k, total_k);
    print_tree("Прим", prim_tree, count_p, total_p);
    if (total_k != total_p) {
        fprintf(stderr, "Ошибка: алгоритмы получили разный вес.\n");
        return 1;
    }
    return 0;
}
