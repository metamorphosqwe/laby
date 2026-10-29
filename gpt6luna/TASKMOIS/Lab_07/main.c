#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_N 20
#define MAX_MODULUS 63
#define MAX_SUBGROUPS 64

static int permutation[MAX_N];
static int visited[MAX_N];

/* Храним элементы группы в битах числа. Поэтому размер группы
   ограничен 63 элементами. */
static uint64_t generated_subgroup(int element, int modulus) {
    uint64_t mask = 0;
    int current = 0;
    do {
        mask |= UINT64_C(1) << current;
        current = (current + element) % modulus;
    } while (current != 0);
    return mask;
}

static int order_of(int element, int modulus) {
    int current = 0, order;
    for (order = 1; order <= modulus; ++order) {
        current = (current + element) % modulus;
        if (current == 0) return order;
    }
    return 0;
}

static int gcd_int(int a, int b) {
    while (b != 0) {
        int remainder = a % b;
        a = b;
        b = remainder;
    }
    return a;
}

static int is_permutation(int n) {
    int i;
    for (i = 0; i < n; ++i) {
        if (permutation[i] < 1 || permutation[i] > n ||
            visited[permutation[i] - 1])
            return 0;
        visited[permutation[i] - 1] = 1;
    }
    return 1;
}

static void print_mask(uint64_t mask, int modulus) {
    int value, first = 1;
    putchar('{');
    for (value = 0; value < modulus; ++value)
        if (mask & (UINT64_C(1) << value)) {
            printf("%s%d", first ? "" : ", ", value);
            first = 0;
        }
    putchar('}');
}

static void analyze_permutation(int n) {
    int start, inversions = 0, transpositions = 0, permutation_order = 1;
    printf("Циклы подстановки: ");
    for (start = 0; start < n; ++start) {
        if (!visited[start]) {
            int current = start, length = 0;
            do {
                visited[current] = 1;
                current = permutation[current] - 1;
                ++length;
            } while (!visited[current]);
            transpositions += length - 1;
            permutation_order =
                permutation_order / gcd_int(permutation_order, length) * length;
            if (length > 1) {
                int item = start;
                putchar('(');
                do {
                    printf("%d", item + 1);
                    item = permutation[item] - 1;
                    if (item != start) putchar(' ');
                } while (item != start);
                putchar(')');
            }
        }
    }
    if (transpositions == 0) printf("единичная подстановка");
    for (start = 0; start < n; ++start) {
        int j;
        for (j = start + 1; j < n; ++j)
            if (permutation[start] > permutation[j]) ++inversions;
    }
    printf("\nПорядок подстановки: %d\nТранспозиций: %d; инверсий: %d; "
           "чётность: %s\n",
           permutation_order, transpositions, inversions,
           inversions % 2 ? "нечётная" : "чётная");
}

static void print_cayley(uint64_t elements, int modulus) {
    int a, b;
    printf("    ");
    for (b = 0; b < modulus; ++b)
        if (elements & (UINT64_C(1) << b)) printf("%d ", b);
    putchar('\n');
    for (a = 0; a < modulus; ++a) {
        if (!(elements & (UINT64_C(1) << a))) continue;
        printf("%d | ", a);
        for (b = 0; b < modulus; ++b)
            if (elements & (UINT64_C(1) << b))
                printf("%d ", (a + b) % modulus);
        putchar('\n');
    }
}

static void print_cosets(uint64_t subgroup, int modulus,
                         uint64_t *cosets, int *count) {
    uint64_t covered = 0;
    int representative, member;
    *count = 0;
    for (representative = 0; representative < modulus; ++representative) {
        uint64_t coset = 0;
        if (covered & (UINT64_C(1) << representative)) continue;
        for (member = 0; member < modulus; ++member)
            if (subgroup & (UINT64_C(1) << member))
                coset |= UINT64_C(1) << ((representative + member) % modulus);
        cosets[(*count)++] = coset;
        covered |= coset;
    }
}

static int read_inputs(FILE *input, int *n, int *modulus) {
    int i;
    if (fscanf(input, "%d", n) != 1 || *n < 1 || *n > MAX_N) return 0;
    for (i = 0; i < *n; ++i)
        if (fscanf(input, "%d", &permutation[i]) != 1) return 0;
    for (i = 0; i < MAX_N; ++i) visited[i] = 0;
    if (!is_permutation(*n)) return 0;
    memset(visited, 0, sizeof(visited));
    if (fscanf(input, "%d", modulus) != 1 ||
        *modulus < 1 || *modulus > MAX_MODULUS)
        return 0;
    return 1;
}

int main(int argc, char **argv) {
    FILE *input;
    int n, modulus, i, j, subgroup_count = 0;
    uint64_t subgroups[MAX_SUBGROUPS];
    uint64_t all_elements, chosen;
    uint64_t cosets[MAX_MODULUS];
    int coset_count;
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <файл.txt>\n", argv[0]);
        return 1;
    }
    input = fopen(argv[1], "r");
    if (input == NULL) {
        perror("Не удалось открыть файл");
        return 1;
    }
    if (!read_inputs(input, &n, &modulus)) {
        fprintf(stderr, "Ошибка: ожидаются корректная перестановка и модуль 1..63.\n");
        fclose(input);
        return 1;
    }
    fclose(input);

    analyze_permutation(n);
    all_elements = (UINT64_C(1) << modulus) - 1;
    printf("\nГруппа Z_%d по сложению: нейтральный элемент 0.\n", modulus);
    printf("Элемент: обратный, порядок\n");
    for (i = 0; i < modulus; ++i)
        printf("%d: %d, %d\n", i, (modulus - i) % modulus,
               order_of(i, modulus));
    for (i = 0; i < modulus; ++i) {
        uint64_t candidate = generated_subgroup(i, modulus);
        int duplicate = 0;
        for (j = 0; j < subgroup_count; ++j)
            if (subgroups[j] == candidate) duplicate = 1;
        if (!duplicate) subgroups[subgroup_count++] = candidate;
    }
    printf("Группа циклическая: %s.\n", order_of(1 % modulus, modulus) == modulus
                                                ? "да" : "да (тривиальная)");
    printf("Таблица Кэли Z_%d:\n", modulus);
    print_cayley(all_elements, modulus);
    puts("Подгруппы, их генераторы и смежные классы:");
    for (i = 0; i < subgroup_count; ++i) {
        uint64_t subgroup = subgroups[i];
        int first = 1;
        printf("H=");
        print_mask(subgroup, modulus);
        printf("; генераторы:");
        for (j = 0; j < modulus; ++j)
            if (generated_subgroup(j, modulus) == subgroup) {
                printf("%s%d", first ? " " : ", ", j);
                first = 0;
            }
        print_cosets(subgroup, modulus, cosets, &coset_count);
        printf("\n  Классы:");
        for (j = 0; j < coset_count; ++j) {
            putchar(' ');
            print_mask(cosets[j], modulus);
        }
        putchar('\n');
    }
    chosen = generated_subgroup(modulus % 2 == 0 ? modulus / 2 : 0, modulus);
    print_cosets(chosen, modulus, cosets, &coset_count);
    printf("Таблица Кэли подгруппы ");
    print_mask(chosen, modulus);
    putchar('\n');
    print_cayley(chosen, modulus);
    printf("Фактор-группа по этой подгруппе (%d классов):\n", coset_count);
    for (i = 0; i < coset_count; ++i) {
        int representative_i = 0;
        while (!(cosets[i] & (UINT64_C(1) << representative_i)))
            ++representative_i;
        for (j = 0; j < coset_count; ++j) {
            int representative_j = 0, sum, k;
            while (!(cosets[j] & (UINT64_C(1) << representative_j)))
                ++representative_j;
            sum = (representative_i + representative_j) % modulus;
            for (k = 0; k < coset_count; ++k)
                if (cosets[k] & (UINT64_C(1) << sum)) {
                    printf("%d ", k);
                    break;
                }
        }
        putchar('\n');
    }
    /* Здесь показаны два примера соответствия между группами.
       Каждое такое соответствие сохраняет операцию группы. */
    puts("Гомоморфизм Z_8 -> U(5): f(k)=2^k mod 5 (ядро {0,4}, образ U(5)).");
    puts("Гомоморфизм Z_4 -> {1,3,9,27} mod 80: f(k)=3^k (изоморфизм).");
    return 0;
}
