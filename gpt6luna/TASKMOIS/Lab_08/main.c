#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_N 8
#define MAX_CELLS (MAX_N * MAX_N)

static int n;
static int adjacency[MAX_CELLS][MAX_CELLS];
static int degree[MAX_CELLS];
static int visited[MAX_CELLS];
static int path[MAX_CELLS];
static int best_path[MAX_CELLS];
static int path_length;
static int best_length;
static int complete;

static void add_move(int from_row, int from_column, int row_delta,
                     int column_delta) {
    int row = from_row + row_delta;
    int column = from_column + column_delta;
    if (row >= 0 && row < n && column >= 0 && column < n) {
        int from = from_row * n + from_column;
        int to = row * n + column;
        adjacency[from][to] = 1;
        adjacency[to][from] = 1;
    }
}

static void build_graph(void) {
    static const int moves[8][2] = {
        {3, 0}, {-3, 0}, {0, 3}, {0, -3},
        {2, 2}, {2, -2}, {-2, 2}, {-2, -2}
    };
    int row, column, move, vertex, next;
    for (row = 0; row < n; ++row)
        for (column = 0; column < n; ++column)
            for (move = 0; move < 8; ++move)
                add_move(row, column, moves[move][0], moves[move][1]);
    for (vertex = 0; vertex < n * n; ++vertex)
        for (next = 0; next < n * n; ++next)
            degree[vertex] += adjacency[vertex][next];
}

/* Считаем, сколько ещё клеток можно достичь.
   Если это не улучшит лучший путь, дальше эту попытку не проверяем. */
static int reachable_upper_bound(int current) {
    int seen[MAX_CELLS] = {0}, queue[MAX_CELLS];
    int head = 0, tail = 0, count = 0, vertex, next;
    queue[tail++] = current;
    seen[current] = 1;
    while (head < tail) {
        vertex = queue[head++];
        ++count;
        for (next = 0; next < n * n; ++next)
            if (adjacency[vertex][next] && !visited[next] && !seen[next]) {
                seen[next] = 1;
                queue[tail++] = next;
            }
    }
    return path_length + count - 1;
}

static int onward_degree(int vertex) {
    int next, count = 0;
    for (next = 0; next < n * n; ++next)
        if (adjacency[vertex][next] && !visited[next]) ++count;
    return count;
}

static void search_from(int current) {
    int options[MAX_CELLS], option_count = 0, next, i, j;
    if (path_length > best_length) {
        best_length = path_length;
        memcpy(best_path, path, (size_t)path_length * sizeof(path[0]));
    }
    if (path_length == n * n) {
        complete = 1;
        return;
    }
    if (reachable_upper_bound(current) <= best_length) return;
    for (next = 0; next < n * n; ++next)
        if (adjacency[current][next] && !visited[next])
            options[option_count++] = next;
    /* Сначала пробуем клетку, из которой меньше продолжений.
       Это помогает быстрее найти длинный путь. */
    for (i = 0; i < option_count; ++i)
        for (j = i + 1; j < option_count; ++j)
            if (onward_degree(options[j]) < onward_degree(options[i])) {
                int temp = options[i];
                options[i] = options[j];
                options[j] = temp;
            }
    for (i = 0; i < option_count && !complete; ++i) {
        next = options[i];
        visited[next] = 1;
        path[path_length++] = next;
        search_from(next);
        --path_length;
        visited[next] = 0;
    }
}

int main(int argc, char **argv) {
    FILE *input;
    int start, row, column, i;
    int board[MAX_CELLS] = {0};
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <файл.txt>\n", argv[0]);
        return 1;
    }
    input = fopen(argv[1], "r");
    if (input == NULL) {
        perror("Не удалось открыть файл");
        return 1;
    }
    if (fscanf(input, "%d", &n) != 1 || n < 1 || n > MAX_N) {
        fprintf(stderr, "Ошибка: размер доски должен быть целым числом 1..%d.\n",
                MAX_N);
        fclose(input);
        return 1;
    }
    fclose(input);
    build_graph();
    for (start = 0; start < n * n && !complete; ++start) {
        if (degree[start] == 0 && best_length > 0) continue;
        visited[start] = 1;
        path[0] = start;
        path_length = 1;
        search_from(start);
        visited[start] = 0;
    }
    for (i = 0; i < best_length; ++i)
        board[best_path[i]] = i + 1;
    printf("Заполнено клеток: %d из %d\n", best_length, n * n);
    for (row = 0; row < n; ++row) {
        for (column = 0; column < n; ++column) {
            int value = board[row * n + column];
            if (value == 0) printf(" .");
            else printf(" %d", value);
        }
        putchar('\n');
    }
    /* Проверяем, что каждый шаг сделан разрешённым ходом. */
    for (i = 1; i < best_length; ++i) {
        int previous = best_path[i - 1], current = best_path[i];
        int dr = abs(previous / n - current / n);
        int dc = abs(previous % n - current % n);
        if (!((dr == 3 && dc == 0) || (dr == 0 && dc == 3) ||
              (dr == 2 && dc == 2))) {
            fprintf(stderr, "Внутренняя ошибка: недопустимый ход.\n");
            return 1;
        }
    }
    return 0;
}
