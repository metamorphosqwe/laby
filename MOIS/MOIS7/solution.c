#include <stdio.h>
#include <stdlib.h>

#define MAX_G 16

static int group[MAX_G], group_size, modulus, is_addition;
static int quotient_count[MAX_G];
static int quotient_cosets[MAX_G][MAX_G][MAX_G];

static int operation(int a, int b) {
    return is_addition ? (a + b) % modulus : (a * b) % modulus;
}

static int index_of(int value) {
    for (int i = 0; i < group_size; ++i)
        if (group[i] == value) return i;
    return -1;
}

static int element_order(int value, int identity) {
    int current = identity;
    for (int order = 1; order <= group_size; ++order) {
        current = operation(current, value);
        if (current == identity) return order;
    }
    return 0;
}

static void print_set(const int *set, int count) {
    printf("{");
    for (int i = 0; i < count; ++i)
        printf("%s%d", i ? ", " : "", set[i]);
    printf("}");
}

static void solve_permutation(FILE *input) {
    int n, p[101], visited[101] = {0}, cycle_lengths[101], cycle_count = 0;
    long long inversions = 0, order = 1;
    if (fscanf(input, "%d", &n) != 1 || n < 1 || n > 100) {
        fprintf(stderr, "Invalid permutation size\n");
        exit(1);
    }
    int seen[101] = {0};
    for (int i = 1; i <= n; ++i) {
        if (fscanf(input, "%d", &p[i]) != 1 || p[i] < 1 || p[i] > n || seen[p[i]]) {
            fprintf(stderr, "Invalid permutation\n");
            exit(1);
        }
        seen[p[i]] = 1;
        for (int j = 1; j < i; ++j)
            if (p[j] > p[i]) ++inversions;
    }
    printf("Cycles:");
    for (int i = 1; i <= n; ++i) {
        if (visited[i]) continue;
        int v = i, length = 0;
        printf(" (");
        do {
            printf("%s%d", length ? " " : "", v);
            visited[v] = 1;
            ++length;
            v = p[v];
        } while (v != i);
        printf(")");
        cycle_lengths[cycle_count++] = length;
    }
    for (int i = 0; i < cycle_count; ++i) {
        int a = (int)order, b = cycle_lengths[i];
        while (b) { int r = a % b; a = b; b = r; }
        order = order / a * cycle_lengths[i];
    }
    printf("\nOrder: %lld\nParity: %s\nInversions: %lld\n",
           order, inversions % 2 ? "odd" : "even", inversions);
}

static int subgroup_sets[MAX_G][MAX_G], subgroup_sizes[MAX_G], subgroup_count;

static void add_subgroup(int generator, int identity) {
    int set[MAX_G], count = 0, current = identity;
    do {
        int present = 0;
        for (int i = 0; i < count; ++i) if (set[i] == current) present = 1;
        if (present) break;
        set[count++] = current;
        current = operation(current, generator);
    } while (count < MAX_G);
    for (int i = 0; i < subgroup_count; ++i) {
        if (subgroup_sizes[i] != count) continue;
        int same = 1;
        for (int j = 0; j < count; ++j) {
            int present = 0;
            for (int k = 0; k < count; ++k)
                if (set[j] == subgroup_sets[i][k]) present = 1;
            if (!present) same = 0;
        }
        if (same) return;
    }
    for (int i = 0; i < count; ++i) subgroup_sets[subgroup_count][i] = set[i];
    subgroup_sizes[subgroup_count++] = count;
}

static void solve_group(FILE *input) {
    char operation_name[8];
    int count, identity = -1, cyclic = 0;
    if (fscanf(input, "%7s%d%d", operation_name, &modulus, &count) != 3 ||
        modulus < 1 || count < 1 || count >= MAX_G) {
        fprintf(stderr, "Invalid group description\n");
        exit(1);
    }
    is_addition = operation_name[0] == 'A' || operation_name[0] == 'a';
    if (!is_addition && operation_name[0] != 'M' && operation_name[0] != 'm') {
        fprintf(stderr, "Operation must be ADD or MUL\n");
        exit(1);
    }
    group_size = count;
    for (int i = 0; i < count; ++i)
        if (fscanf(input, "%d", &group[i]) != 1) {
            fprintf(stderr, "Missing group element\n");
            exit(1);
        }
    for (int i = 0; i < count; ++i)
        for (int j = 0; j < count; ++j)
            if (index_of(operation(group[i], group[j])) < 0) {
                fprintf(stderr, "Input set is not closed under the operation\n");
                exit(1);
            }
    for (int i = 0; i < count; ++i) {
        int valid = 1;
        for (int j = 0; j < count; ++j)
            if (operation(group[i], group[j]) != group[j] ||
                operation(group[j], group[i]) != group[j]) valid = 0;
        if (valid) { identity = group[i]; break; }
    }
    if (identity < 0) {
        fprintf(stderr, "Input set is not closed under the operation\n");
        exit(1);
    }

    printf("Identity: %d\nOrders:", identity);
    for (int i = 0; i < count; ++i) {
        int order = element_order(group[i], identity);
        printf(" %d:%d", group[i], order);
        if (order == count) cyclic = 1;
    }
    printf("\nInverses:");
    for (int i = 0; i < count; ++i) {
        int inverse = -1;
        for (int j = 0; j < count; ++j)
            if (operation(group[i], group[j]) == identity) {
                inverse = group[j];
                break;
            }
        if (inverse < 0) {
            fprintf(stderr, "Input set contains a non-invertible element\n");
            exit(1);
        }
        printf(" %d:%d", group[i], inverse);
    }
    printf("\nCyclic: %s\nCayley table:\n   ", cyclic ? "YES" : "NO");
    for (int j = 0; j < count; ++j) printf(" %d", group[j]);
    putchar('\n');
    for (int i = 0; i < count; ++i) {
        printf("%d:", group[i]);
        for (int j = 0; j < count; ++j) printf(" %d", operation(group[i], group[j]));
        putchar('\n');
    }
    printf("Subgroups:\n");
    subgroup_count = 0;
    for (int i = 0; i < count; ++i) add_subgroup(group[i], identity);
    for (int i = 0; i < subgroup_count; ++i) {
        int generated_by[MAX_G], generators = 0;
        for (int g = 0; g < count; ++g) {
            int generated[MAX_G], size = 0, x = identity;
            do {
                generated[size++] = x;
                x = operation(x, group[g]);
            } while (x != identity && size < MAX_G);
            if (size != subgroup_sizes[i]) continue;
            int same = 1;
            for (int j = 0; j < size; ++j) {
                int found = 0;
                for (int k = 0; k < size; ++k)
                    if (generated[j] == subgroup_sets[i][k]) found = 1;
                if (!found) same = 0;
            }
            if (same) generated_by[generators++] = group[g];
        }
        printf("H%d=", i + 1);
        print_set(subgroup_sets[i], subgroup_sizes[i]);
        printf(" generated by ");
        print_set(generated_by, generators);
        printf("\nCosets:");
        int used[MAX_G] = {0};
        quotient_count[i] = 0;
        for (int g = 0; g < count; ++g) {
            int coset[MAX_G], coset_count = 0;
            for (int j = 0; j < subgroup_sizes[i]; ++j)
                coset[coset_count++] = operation(subgroup_sets[i][j], group[g]);
            int all_used = 1;
            for (int j = 0; j < coset_count; ++j) {
                int index = index_of(coset[j]);
                if (index >= 0 && !used[index]) all_used = 0;
            }
            if (all_used) continue;
            for (int j = 0; j < coset_count; ++j) {
                int index = index_of(coset[j]);
                if (index >= 0) used[index] = 1;
                quotient_cosets[i][quotient_count[i]][j] = coset[j];
            }
            printf(" ");
            print_set(coset, coset_count);
            ++quotient_count[i];
        }
        putchar('\n');
    }

    printf("Cayley table of H1:\n   ");
    for (int j = 0; j < subgroup_sizes[0]; ++j)
        printf(" %d", subgroup_sets[0][j]);
    putchar('\n');
    for (int i = 0; i < subgroup_sizes[0]; ++i) {
        printf("%d:", subgroup_sets[0][i]);
        for (int j = 0; j < subgroup_sizes[0]; ++j)
            printf(" %d", operation(subgroup_sets[0][i], subgroup_sets[0][j]));
        putchar('\n');
    }

    printf("Factor groups:\n");
    for (int h = 0; h < subgroup_count; ++h) {
        int q_count = quotient_count[h];
        printf("G/H%d cosets:", h + 1);
        for (int q = 0; q < q_count; ++q) {
            printf(" C%d=", q + 1);
            print_set(quotient_cosets[h][q], subgroup_sizes[h]);
        }
        printf("\nG/H%d Cayley table:\n   ", h + 1);
        for (int q = 0; q < q_count; ++q) printf(" C%d", q + 1);
        putchar('\n');
        for (int a = 0; a < q_count; ++a) {
            printf("C%d:", a + 1);
            for (int b = 0; b < q_count; ++b) {
                int product = operation(quotient_cosets[h][a][0],
                                        quotient_cosets[h][b][0]);
                int result = -1;
                for (int q = 0; q < q_count; ++q)
                    for (int j = 0; j < subgroup_sizes[h]; ++j)
                        if (quotient_cosets[h][q][j] == product) result = q;
                printf(" C%d", result + 1);
            }
            putchar('\n');
        }
    }
}

static void solve_homomorphism(FILE *input) {
    int variant;
    if (fscanf(input, "%d", &variant) != 1 || (variant != 1 && variant != 2)) {
        fprintf(stderr, "Homomorphism variant must be 1 or 2\n");
        exit(1);
    }
    int domain = variant == 1 ? 8 : 4;
    int codomain[4] = {1, 2, 3, 4};
    int base = 2, target_modulus = 5;
    if (variant == 2) {
        codomain[0] = 1; codomain[1] = 3; codomain[2] = 9; codomain[3] = 27;
        base = 3;
        target_modulus = 80;
    }
    int image[8], kernel[8], kernel_count = 0, image_values[4], image_count = 0;
    int exponent_modulus = 4;
    for (int x = 0; x < domain; ++x) {
        int exponent = x % exponent_modulus, value = 1;
        for (int i = 0; i < exponent; ++i) value = value * base % target_modulus;
        image[x] = value;
        if (value == 1) kernel[kernel_count++] = x;
        int found = 0;
        for (int j = 0; j < image_count; ++j)
            if (image_values[j] == value) found = 1;
        if (!found) image_values[image_count++] = value;
    }
    printf("Mapping Z%d -> {%d, %d, %d, %d} (multiplication mod %d):\n",
           domain, codomain[0], codomain[1], codomain[2], codomain[3], target_modulus);
    for (int x = 0; x < domain; ++x) printf("%d -> %d\n", x, image[x]);
    printf("Kernel: ");
    print_set(kernel, kernel_count);
    printf("\nImage: ");
    print_set(image_values, image_count);
    printf("\nInjective: %s\nSurjective: %s\nIsomorphism: %s\n",
           kernel_count == 1 ? "YES" : "NO",
           image_count == 4 ? "YES" : "NO",
           kernel_count == 1 && image_count == 4 && domain == 4 ? "YES" : "NO");
}

int main(int argc, char **argv) {
    FILE *input = stdin;
    int mode;
    if (argc > 1) {
        input = fopen(argv[1], "r");
        if (!input) { perror(argv[1]); return 1; }
    }
    if (fscanf(input, "%d", &mode) != 1) {
        fprintf(stderr, "Missing mode\n");
        return 1;
    }
    if (mode == 1) solve_permutation(input);
    else if (mode == 2) solve_group(input);
    else if (mode == 3) solve_homomorphism(input);
    else { fprintf(stderr, "Mode must be 1, 2, or 3\n"); return 1; }
    if (input != stdin) fclose(input);
    return 0;
}
