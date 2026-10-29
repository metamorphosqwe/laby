#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_V 100
#define MAX_SIGNATURES 10000

typedef struct {
    int n;
    int children[MAX_V][2];
    int child_count[MAX_V];
    int parent[MAX_V];
} Tree;

typedef struct {
    int left;
    int right;
} Signature;

static Signature signatures[MAX_SIGNATURES];
static int signature_count;

/* В файле для дерева записаны количество вершин и пары «родитель — ребёнок».
   Корень имеет номер 1. */
static int read_tree(FILE *input, Tree *tree) {
    int i, edge_count;
    memset(tree, 0, sizeof(*tree));
    if (fscanf(input, "%d", &tree->n) != 1 ||
        tree->n < 1 || tree->n > MAX_V)
        return 0;
    edge_count = tree->n - 1;
    for (i = 0; i < tree->n; ++i) tree->parent[i] = -1;
    for (i = 0; i < edge_count; ++i) {
        int parent, child;
        if (fscanf(input, "%d%d", &parent, &child) != 2 ||
            parent < 1 || parent > tree->n || child < 1 ||
            child > tree->n || parent == child)
            return 0;
        --parent;
        --child;
        if (tree->parent[child] != -1 || tree->child_count[parent] == 2)
            return 0;
        tree->parent[child] = parent;
        tree->children[parent][tree->child_count[parent]++] = child;
    }
    if (tree->parent[0] != -1) return 0;
    for (i = 1; i < tree->n; ++i)
        if (tree->parent[i] == -1) return 0;
    return 1;
}

/* Проверяем, что от корня можно дойти до каждой вершины.
   Это помогает отсеять неверно заданные деревья. */
static void mark_reachable(const Tree *tree, int vertex, int *seen) {
    int i;
    seen[vertex] = 1;
    for (i = 0; i < tree->child_count[vertex]; ++i) {
        int child = tree->children[vertex][i];
        if (!seen[child]) mark_reachable(tree, child, seen);
    }
}

static int is_valid_tree(const Tree *tree) {
    int seen[MAX_V] = {0}, i;
    mark_reachable(tree, 0, seen);
    for (i = 0; i < tree->n; ++i)
        if (!seen[i]) return 0;
    return 1;
}

/* Создаём краткое описание формы поддерева.
   Порядок двух детей не важен, поэтому сортируем их описания. */
static int intern_signature(int left, int right) {
    int i;
    if (left > right) {
        int temp = left;
        left = right;
        right = temp;
    }
    for (i = 0; i < signature_count; ++i)
        if (signatures[i].left == left && signatures[i].right == right)
            return i + 1;
    if (signature_count == MAX_SIGNATURES) return -1;
    signatures[signature_count].left = left;
    signatures[signature_count].right = right;
    return ++signature_count;
}

static int canonical_id(const Tree *tree, int vertex, int omitted_vertex) {
    int child_ids[2], count = 0, i, temp;
    if (vertex == omitted_vertex) return 0;
    for (i = 0; i < tree->child_count[vertex]; ++i) {
        int child = tree->children[vertex][i];
        int id;
        if (child == omitted_vertex) continue;
        id = canonical_id(tree, child, omitted_vertex);
        if (id < 0) return -1;
        child_ids[count++] = id;
    }
    if (count == 2 && child_ids[0] > child_ids[1]) {
        temp = child_ids[0];
        child_ids[0] = child_ids[1];
        child_ids[1] = temp;
    }
    return intern_signature(count > 0 ? child_ids[0] : 0,
                            count > 1 ? child_ids[1] : 0);
}

/* Считаем высоту веток. Если ветки одного узла отличаются
   по высоте больше чем на единицу, дерево несбалансировано. */
static int tree_height(const Tree *tree, int vertex, int *balanced) {
    int heights[2] = {-1, -1};
    int i, maximum = -1;
    for (i = 0; i < tree->child_count[vertex]; ++i) {
        heights[i] = tree_height(tree, tree->children[vertex][i], balanced);
        if (heights[i] > maximum) maximum = heights[i];
    }
    if (abs(heights[0] - heights[1]) > 1) *balanced = 0;
    return maximum + 1;
}

static void print_preorder(const Tree *tree, int vertex) {
    int i;
    printf("%d ", vertex + 1);
    for (i = 0; i < tree->child_count[vertex]; ++i)
        print_preorder(tree, tree->children[vertex][i]);
}

static void print_postorder(const Tree *tree, int vertex) {
    int i;
    for (i = 0; i < tree->child_count[vertex]; ++i)
        print_postorder(tree, tree->children[vertex][i]);
    printf("%d ", vertex + 1);
}

static void print_inorder(const Tree *tree, int vertex) {
    if (tree->child_count[vertex] > 0)
        print_inorder(tree, tree->children[vertex][0]);
    printf("%d ", vertex + 1);
    if (tree->child_count[vertex] > 1)
        print_inorder(tree, tree->children[vertex][1]);
}

static void describe_tree(const char *name, const Tree *tree) {
    int balanced = 1;
    int height = tree_height(tree, 0, &balanced);
    printf("%s: высота=%d ребр., AVL-сбалансировано=%s\n",
           name, height, balanced ? "да" : "нет");
    printf("Pre-order: ");
    print_preorder(tree, 0);
    printf("\nIn-order: ");
    print_inorder(tree, 0);
    printf("\nPost-order: ");
    print_postorder(tree, 0);
    putchar('\n');
}

int main(int argc, char **argv) {
    FILE *input;
    Tree first, second;
    int first_id, second_id, v, found = 0;
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <файл.txt>\n", argv[0]);
        return 1;
    }
    input = fopen(argv[1], "r");
    if (input == NULL) {
        perror("Не удалось открыть файл");
        return 1;
    }
    if (!read_tree(input, &first) || !read_tree(input, &second) ||
        !is_valid_tree(&first) || !is_valid_tree(&second)) {
        fprintf(stderr, "Ошибка: файл должен содержать два корневых бинарных дерева.\n");
        fclose(input);
        return 1;
    }
    fclose(input);

    signature_count = 0;
    first_id = canonical_id(&first, 0, -1);
    second_id = canonical_id(&second, 0, -1);
    describe_tree("T1", &first);
    describe_tree("T2", &second);
    printf("Изоморфны до изменения: %s\n",
           first_id == second_id ? "да" : "нет");

    /* По очереди пробуем удалить каждый лист и проверяем,
       станут ли после этого деревья одинаковой формы. */
    if (first_id != second_id) {
        for (v = 1; v < first.n && !found; ++v) {
            if (first.child_count[v] == 0) {
                int candidate = canonical_id(&first, 0, v);
                if (candidate == second_id) {
                    printf("Для изоморфности достаточно удалить лист %d из T1.\n",
                           v + 1);
                    found = 1;
                }
            }
        }
        for (v = 1; v < second.n && !found; ++v) {
            if (second.child_count[v] == 0) {
                int candidate = canonical_id(&second, 0, v);
                if (candidate == first_id) {
                    printf("Для изоморфности достаточно удалить лист %d из T2.\n",
                           v + 1);
                    found = 1;
                }
            }
        }
        if (!found)
            puts("Удаление одного листа недостаточно для изоморфности.");
    }
    return 0;
}
