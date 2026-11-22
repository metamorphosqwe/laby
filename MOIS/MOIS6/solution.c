#include <stdio.h>
#include <stdlib.h>

#define MAX_N 205

typedef struct {
    int child[2];
    int child_count;
} Node;

static Node first_tree[MAX_N], second_tree[MAX_N];
static int first_parent[MAX_N], second_parent[MAX_N];

static int add_child(Node *tree, int *parents, int parent, int child, int n) {
    if (parent < 1 || parent > n || child < 1 || child > n ||
        parent == child || tree[parent].child_count >= 2 || parents[child])
        return 0;
    tree[parent].child[tree[parent].child_count++] = child;
    parents[child] = parent;
    return 1;
}

static int isomorphic(int a, int b) {
    if (a == 0 || b == 0) return a == b;
    Node *x = &first_tree[a], *y = &second_tree[b];
    if (x->child_count != y->child_count) return 0;
    if (x->child_count == 0) return 1;
    if (x->child_count == 1) return isomorphic(x->child[0], y->child[0]);
    return (isomorphic(x->child[0], y->child[0]) &&
            isomorphic(x->child[1], y->child[1])) ||
           (isomorphic(x->child[0], y->child[1]) &&
            isomorphic(x->child[1], y->child[0]));
}

static int tree_info(Node *tree, int v, int *balanced) {
    int left = tree[v].child_count > 0 ? tree_info(tree, tree[v].child[0], balanced) : 0;
    int right = tree[v].child_count > 1 ? tree_info(tree, tree[v].child[1], balanced) : 0;
    int difference = left - right;
    if (difference < 0) difference = -difference;
    if (difference > 1) *balanced = 0;
    return (left > right ? left : right) + 1;
}

static void count_reachable(Node *tree, int v, int *count) {
    ++*count;
    for (int i = 0; i < tree[v].child_count; ++i)
        count_reachable(tree, tree[v].child[i], count);
}

static int read_tree(FILE *input, Node *tree, int *parents, int *n_out) {
    int n, m;
    if (fscanf(input, "%d%d", &n, &m) != 2 || n < 1 || n >= MAX_N ||
        m < 0 || m >= n) return 0;
    for (int i = 0; i < m; ++i) {
        int parent, child;
        if (fscanf(input, "%d%d", &parent, &child) != 2 ||
            !add_child(tree, parents, parent, child, n)) return 0;
    }
    if (parents[1] != 0) return 0;
    for (int v = 2; v <= n; ++v)
        if (parents[v] == 0) return 0;
    int reachable = 0;
    count_reachable(tree, 1, &reachable);
    if (reachable != n) return 0;
    *n_out = n;
    return 1;
}

int main(int argc, char **argv) {
    FILE *input = stdin;
    int n1, n2;
    if (argc > 1) {
        input = fopen(argv[1], "r");
        if (!input) { perror(argv[1]); return 1; }
    }
    if (!read_tree(input, first_tree, first_parent, &n1) ||
        !read_tree(input, second_tree, second_parent, &n2)) {
        fprintf(stderr, "Invalid rooted binary tree input\n");
        return 1;
    }
    if (input != stdin) fclose(input);
    int balanced1 = 1, balanced2 = 1;
    int height1 = tree_info(first_tree, 1, &balanced1);
    int height2 = tree_info(second_tree, 1, &balanced2);
    printf("Isomorphic: %s\n", isomorphic(1, 1) ? "YES" : "NO");
    printf("Tree1: height=%d balanced=%s\n", height1, balanced1 ? "YES" : "NO");
    printf("Tree2: height=%d balanced=%s\n", height2, balanced2 ? "YES" : "NO");
    return 0;
}
