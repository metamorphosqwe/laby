#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BOARD 32

static long long queen_count;
static int queen_n, queen_col[16], queen_used_col[16], queen_d1[32], queen_d2[32];

static void count_queens(int row) {
    if (row == queen_n) { ++queen_count; return; }
    for (int col = 0; col < queen_n; ++col) {
        if (queen_used_col[col] || queen_d1[row - col + queen_n] ||
            queen_d2[row + col]) continue;
        queen_used_col[col] = queen_d1[row - col + queen_n] = queen_d2[row + col] = 1;
        queen_col[row] = col;
        count_queens(row + 1);
        queen_used_col[col] = queen_d1[row - col + queen_n] = queen_d2[row + col] = 0;
    }
}

static int knight_rows[8] = {-2, -2, -1, -1, 1, 1, 2, 2};
static int knight_cols[8] = {-1, 1, -2, 2, -2, 2, -1, 1};
static int tour[8][8], tour_n, tour_m, tour_start_r, tour_start_c;

static int knight_degree(int r, int c) {
    int degree = 0;
    for (int i = 0; i < 8; ++i) {
        int nr = r + knight_rows[i], nc = c + knight_cols[i];
        if (nr >= 0 && nr < tour_n && nc >= 0 && nc < tour_m && !tour[nr][nc])
            ++degree;
    }
    return degree;
}

static int find_tour(int r, int c, int step) {
    tour[r][c] = step;
    if (step == tour_n * tour_m) return 1;
    int next_r[8], next_c[8], degree[8], count = 0;
    for (int i = 0; i < 8; ++i) {
        int nr = r + knight_rows[i], nc = c + knight_cols[i];
        if (nr < 0 || nr >= tour_n || nc < 0 || nc >= tour_m || tour[nr][nc]) continue;
        int d = knight_degree(nr, nc), at = count++;
        while (at > 0 && degree[at - 1] > d) {
            degree[at] = degree[at - 1];
            next_r[at] = next_r[at - 1];
            next_c[at] = next_c[at - 1];
            --at;
        }
        degree[at] = d;
        next_r[at] = nr;
        next_c[at] = nc;
    }
    for (int i = 0; i < count; ++i)
        if (find_tour(next_r[i], next_c[i], step + 1)) return 1;
    tour[r][c] = 0;
    return 0;
}

static int mag_n, mag_col[16], mag_used[16], mag_d1[32], mag_d2[32];
static long long mag_count;

static void count_maharaja(int row) {
    if (row == mag_n) { ++mag_count; return; }
    for (int col = 0; col < mag_n; ++col) {
        if (mag_used[col] || mag_d1[row - col + mag_n] || mag_d2[row + col]) continue;
        int valid = 1;
        for (int prev = 0; prev < row; ++prev)
            if ((row - prev == 2 && abs(col - mag_col[prev]) == 1) ||
                (row - prev == 1 && abs(col - mag_col[prev]) == 2)) valid = 0;
        if (!valid) continue;
        mag_used[col] = mag_d1[row - col + mag_n] = mag_d2[row + col] = 1;
        mag_col[row] = col;
        count_maharaja(row + 1);
        mag_used[col] = mag_d1[row - col + mag_n] = mag_d2[row + col] = 0;
    }
}

static int fill_n, fill_order[8][8], best_fill[8][8], fill_best;
static int fill_dr[8] = {-3, 3, 0, 0, -2, -2, 2, 2};
static int fill_dc[8] = {0, 0, -3, 3, -2, 2, -2, 2};

static void longest_fill(int r, int c, int step) {
    fill_order[r][c] = step;
    if (step > fill_best) {
        fill_best = step;
        memcpy(best_fill, fill_order, sizeof(best_fill));
    }
    if (step + fill_n * fill_n - step <= fill_best) {
        fill_order[r][c] = 0;
        return;
    }
    for (int i = 0; i < 8; ++i) {
        int nr = r + fill_dr[i], nc = c + fill_dc[i];
        if (nr >= 0 && nr < fill_n && nc >= 0 && nc < fill_n && !fill_order[nr][nc])
            longest_fill(nr, nc, step + 1);
    }
    fill_order[r][c] = 0;
}

static void make_odd_square(int n, int square[BOARD][BOARD]) {
    int r = 0, c = n / 2;
    for (int value = 1; value <= n * n; ++value) {
        square[r][c] = value;
        int nr = (r - 1 + n) % n, nc = (c + 1) % n;
        if (square[nr][nc]) r = (r + 1) % n;
        else { r = nr; c = nc; }
    }
}

static void magic_square(int n, int square[BOARD][BOARD]) {
    if (n % 2 == 1) {
        make_odd_square(n, square);
    } else if (n % 4 == 0) {
        for (int r = 0; r < n; ++r)
            for (int c = 0; c < n; ++c) {
                int value = r * n + c + 1;
                square[r][c] = (r % 4 == c % 4 || r % 4 + c % 4 == 3)
                                   ? n * n + 1 - value : value;
            }
    } else {
        int half = n / 2, sub[BOARD][BOARD] = {{0}};
        make_odd_square(half, sub);
        int block = half * half;
        for (int r = 0; r < half; ++r)
            for (int c = 0; c < half; ++c) {
                square[r][c] = sub[r][c];
                square[r][c + half] = sub[r][c] + 2 * block;
                square[r + half][c] = sub[r][c] + 3 * block;
                square[r + half][c + half] = sub[r][c] + block;
            }
        int k = (n - 2) / 4;
        for (int r = 0; r < half; ++r) {
            for (int c = 0; c < k; ++c) {
                int t = square[r][c]; square[r][c] = square[r + half][c];
                square[r + half][c] = t;
            }
            for (int c = n - k + 1; c < n; ++c) {
                int t = square[r][c]; square[r][c] = square[r + half][c];
                square[r + half][c] = t;
            }
        }
        int middle = half / 2, t = square[middle][0];
        square[middle][0] = square[middle + half][0];
        square[middle + half][0] = t;
        t = square[middle][k];
        square[middle][k] = square[middle + half][k];
        square[middle + half][k] = t;
    }
}

static int maze_n, maze_m, maze_sr, maze_sc, maze_er, maze_ec;
static char maze[BOARD][BOARD];
static int maze_dist[BOARD][BOARD], maze_prev_r[BOARD][BOARD], maze_prev_c[BOARD][BOARD];
static int queue_r[BOARD * BOARD], queue_c[BOARD * BOARD];

static void solve_maze(FILE *in) {
    if (fscanf(in, "%d%d%d%d%d%d", &maze_n, &maze_m, &maze_sr, &maze_sc,
               &maze_er, &maze_ec) != 6 || maze_n < 1 || maze_n >= BOARD ||
        maze_m < 1 || maze_m >= BOARD) { fprintf(stderr, "Invalid maze\n"); exit(1); }
    --maze_sr; --maze_sc; --maze_er; --maze_ec;
    for (int r = 0; r < maze_n; ++r)
        if (fscanf(in, "%31s", maze[r]) != 1 || (int)strlen(maze[r]) != maze_m) {
            fprintf(stderr, "Invalid maze row\n"); exit(1);
        }
    if (maze_sr < 0 || maze_sr >= maze_n || maze_er < 0 || maze_er >= maze_n ||
        maze_sc < 0 || maze_sc >= maze_m || maze_ec < 0 || maze_ec >= maze_m ||
        maze[maze_sr][maze_sc] == '#' || maze[maze_er][maze_ec] == '#') {
        printf("NO PATH\n"); return;
    }
    for (int r = 0; r < maze_n; ++r)
        for (int c = 0; c < maze_m; ++c) maze_dist[r][c] = -1;
    int front = 0, back = 0;
    queue_r[back] = maze_sr; queue_c[back++] = maze_sc;
    maze_dist[maze_sr][maze_sc] = 0;
    int dr[4] = {-1, 1, 0, 0}, dc[4] = {0, 0, -1, 1};
    while (front < back) {
        int r = queue_r[front], c = queue_c[front++];
        for (int i = 0; i < 4; ++i) {
            int nr = r + dr[i], nc = c + dc[i];
            if (nr < 0 || nr >= maze_n || nc < 0 || nc >= maze_m ||
                maze[nr][nc] == '#' || maze_dist[nr][nc] >= 0) continue;
            maze_dist[nr][nc] = maze_dist[r][c] + 1;
            maze_prev_r[nr][nc] = r; maze_prev_c[nr][nc] = c;
            queue_r[back] = nr; queue_c[back++] = nc;
        }
    }
    if (maze_dist[maze_er][maze_ec] < 0) { printf("NO PATH\n"); return; }
    int path_r[BOARD * BOARD], path_c[BOARD * BOARD], size = 0;
    int r = maze_er, c = maze_ec;
    while (r != maze_sr || c != maze_sc) {
        path_r[size] = r; path_c[size++] = c;
        int nr = maze_prev_r[r][c], nc = maze_prev_c[r][c]; r = nr; c = nc;
    }
    path_r[size] = r; path_c[size++] = c;
    printf("Distance: %d\nPath:", maze_dist[maze_er][maze_ec]);
    while (size--) printf(" (%d,%d)", path_r[size] + 1, path_c[size] + 1);
    putchar('\n');
}

static void solve_knapsack(FILE *in) {
    int n, capacity, weights[100], values[100];
    if (fscanf(in, "%d%d", &n, &capacity) != 2 || n < 1 || n > 100 ||
        capacity < 0 || capacity > 1000000) { fprintf(stderr, "Invalid knapsack\n"); exit(1); }
    for (int i = 0; i < n; ++i)
        if (fscanf(in, "%d%d", &weights[i], &values[i]) != 2 ||
            weights[i] <= 0 || values[i] < 0) { fprintf(stderr, "Invalid item\n"); exit(1); }
    long long *dp = (long long *)malloc((size_t)(capacity + 1) * sizeof(long long));
    if (!dp) { fprintf(stderr, "Out of memory\n"); exit(1); }
    for (int w = 0; w <= capacity; ++w) dp[w] = -1;
    dp[0] = 0;
    for (int w = 1; w <= capacity; ++w)
        for (int i = 0; i < n; ++i)
            if (weights[i] <= w && dp[w - weights[i]] >= 0) {
                long long value = dp[w - weights[i]] + values[i];
                if (value > dp[w]) dp[w] = value;
            }
    if (dp[capacity] < 0) printf("IMPOSSIBLE\n");
    else printf("Maximum value: %lld\n", dp[capacity]);
    free(dp);
}

static void solve_tsp(FILE *in) {
    int n;
    if (fscanf(in, "%d", &n) != 1 || n < 1 || n > 17) {
        fprintf(stderr, "TSP supports 1..17 cities\n"); exit(1);
    }
    int weight[17][17];
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            if (fscanf(in, "%d", &weight[i][j]) != 1 || weight[i][j] < 0) {
                fprintf(stderr, "Invalid distance matrix\n"); exit(1);
            }
    if (n == 1) { printf("Minimum: 0\nRoute: 1 1\n"); return; }
    int bits = n - 1, states = 1 << bits;
    size_t cells = (size_t)states * bits;
    long long *dp = (long long *)malloc(cells * sizeof(long long));
    short *previous = (short *)malloc(cells * sizeof(short));
    if (!dp || !previous) { fprintf(stderr, "Out of memory\n"); exit(1); }
    for (size_t i = 0; i < cells; ++i) { dp[i] = LLONG_MAX / 4; previous[i] = -1; }
#define DP(mask, last) dp[(size_t)(mask) * bits + (last)]
#define PREV(mask, last) previous[(size_t)(mask) * bits + (last)]
    for (int j = 0; j < bits; ++j) DP(1 << j, j) = weight[0][j + 1];
    for (int mask = 1; mask < states; ++mask)
        for (int last = 0; last < bits; ++last) {
            if (!(mask & (1 << last)) || DP(mask, last) >= LLONG_MAX / 8) continue;
            for (int next = 0; next < bits; ++next) {
                if (mask & (1 << next)) continue;
                int next_mask = mask | (1 << next);
                long long candidate = DP(mask, last) + weight[last + 1][next + 1];
                if (candidate < DP(next_mask, next)) {
                    DP(next_mask, next) = candidate;
                    PREV(next_mask, next) = (short)last;
                }
            }
        }
    int full = states - 1, last = -1;
    long long best = LLONG_MAX / 4;
    for (int j = 0; j < bits; ++j)
        if (DP(full, j) + weight[j + 1][0] < best) {
            best = DP(full, j) + weight[j + 1][0]; last = j;
        }
    int route[18], size = 0, mask = full;
    while (last >= 0) {
        route[size++] = last + 1;
        int old = PREV(mask, last);
        mask ^= 1 << last;
        last = old;
    }
    printf("Minimum: %lld\nRoute: 1", best);
    while (size) printf(" %d", route[--size] + 1);
    printf(" 1\n");
    free(dp); free(previous);
}

static int safe_best;
static void score_safe_board(void) {
    int attacked[16][16] = {{0}};
    for (int r = 0; r < queen_n; ++r) {
        int c = queen_col[r];
        for (int i = 0; i < queen_n; ++i) {
            attacked[r][i] = attacked[i][c] = 1;
            if (r + i - c >= 0 && r + i - c < queen_n)
                attacked[r + i - c][i] = 1;
            if (r - i + c >= 0 && r - i + c < queen_n)
                attacked[r - i + c][i] = 1;
        }
    }
    int safe = 0;
    for (int r = 0; r < queen_n; ++r)
        for (int c = 0; c < queen_n; ++c)
            if (!attacked[r][c]) ++safe;
    if (safe > safe_best) safe_best = safe;
}

static void maximize_safe(int row) {
    if (row == queen_n) { score_safe_board(); return; }
    for (int c = 0; c < queen_n; ++c) {
        if (queen_used_col[c] || queen_d1[row - c + queen_n] || queen_d2[row + c]) continue;
        queen_used_col[c] = queen_d1[row - c + queen_n] = queen_d2[row + c] = 1;
        queen_col[row] = c;
        maximize_safe(row + 1);
        queen_used_col[c] = queen_d1[row - c + queen_n] = queen_d2[row + c] = 0;
    }
}

static uint64_t cover[64], full_cover, chosen;
static int cover_n, cover_best;

static void dominate(uint64_t uncovered, int count) {
    if (!uncovered) { if (count < cover_best) cover_best = count; return; }
    if (count >= cover_best) return;
    int square = 0;
    while (!(uncovered & (UINT64_C(1) << square))) ++square;
    for (int q = 0; q < cover_n * cover_n; ++q) {
        uint64_t bit = UINT64_C(1) << q;
        if (!(cover[q] & (UINT64_C(1) << square)) || (chosen & bit)) continue;
        chosen |= bit;
        dominate(uncovered & ~cover[q], count + 1);
        chosen &= ~bit;
    }
}

static void solve_domination(int n) {
    if (n < 1 || n > 8) { fprintf(stderr, "Domination mode supports n=1..8\n"); exit(1); }
    cover_n = n; cover_best = n * n + 1; chosen = 0; full_cover = 0;
    for (int q = 0; q < n * n; ++q) {
        int r = q / n, c = q % n;
        cover[q] = 0;
        for (int s = 0; s < n * n; ++s) {
            int rr = s / n, cc = s % n;
            if (r == rr || c == cc || abs(r - rr) == abs(c - cc))
                cover[q] |= UINT64_C(1) << s;
        }
        full_cover |= UINT64_C(1) << q;
    }
    dominate(full_cover, 0);
    printf("Minimum queens: %d\n", cover_best);
}

int main(int argc, char **argv) {
    FILE *in = stdin;
    int mode;
    if (argc > 1) {
        in = fopen(argv[1], "r");
        if (!in) { perror(argv[1]); return 1; }
    }
    if (fscanf(in, "%d", &mode) != 1) { fprintf(stderr, "Missing task number\n"); return 1; }
    if (mode == 1) {
        if (fscanf(in, "%d", &queen_n) != 1 || queen_n < 1 || queen_n > 14) {
            fprintf(stderr, "N-queens supports n=1..14\n"); return 1;
        }
        count_queens(0);
        printf("Solutions: %lld\n", queen_count);
    } else if (mode == 2) {
        if (fscanf(in, "%d%d%d%d", &tour_n, &tour_m, &tour_start_r, &tour_start_c) != 4 ||
            tour_n < 1 || tour_n > 8 || tour_m < 1 || tour_m > 8 ||
            tour_start_r < 1 || tour_start_r > tour_n ||
            tour_start_c < 1 || tour_start_c > tour_m) {
            fprintf(stderr, "Invalid knight board or start\n"); return 1;
        }
        if (find_tour(tour_start_r - 1, tour_start_c - 1, 1)) {
            printf("Tour found:\n");
            for (int r = 0; r < tour_n; ++r) {
                for (int c = 0; c < tour_m; ++c) printf("%s%d", c ? " " : "", tour[r][c]);
                putchar('\n');
            }
        } else printf("No tour\n");
    } else if (mode == 3) {
        if (fscanf(in, "%d", &mag_n) != 1 || mag_n < 1 || mag_n > 12) {
            fprintf(stderr, "Maharaja mode supports n=1..12\n"); return 1;
        }
        count_maharaja(0);
        printf("Solutions: %lld\n", mag_count);
    } else if (mode == 4) {
        int n, sr, sc;
        if (fscanf(in, "%d%d%d", &n, &sr, &sc) != 3 ||
            n < 1 || n > 5 || sr < 1 || sr > n || sc < 1 || sc > n) {
            fprintf(stderr, "Board-fill mode supports n=1..5 and a valid start\n"); return 1;
        }
        fill_n = n;
        longest_fill(sr - 1, sc - 1, 1);
        printf("Filled: %d\n", fill_best);
        for (int r = 0; r < n; ++r) {
            for (int c = 0; c < n; ++c) printf("%s%d", c ? " " : "", best_fill[r][c]);
            putchar('\n');
        }
    } else if (mode == 5) {
        int n, square[BOARD][BOARD] = {{0}};
        if (fscanf(in, "%d", &n) != 1 || n < 1 || n >= BOARD || n == 2) {
            fprintf(stderr, "Magic square supports n=1 or n>=3\n"); return 1;
        }
        magic_square(n, square);
        for (int r = 0; r < n; ++r) {
            for (int c = 0; c < n; ++c) printf("%s%d", c ? " " : "", square[r][c]);
            putchar('\n');
        }
    } else if (mode == 6) solve_maze(in);
    else if (mode == 7) solve_knapsack(in);
    else if (mode == 8) solve_tsp(in);
    else if (mode == 9) {
        if (fscanf(in, "%d", &queen_n) != 1 || queen_n < 1 || queen_n > 12) {
            fprintf(stderr, "Safe-square mode supports n=1..12\n"); return 1;
        }
        maximize_safe(0);
        printf("Maximum safe squares: %d\n", safe_best);
    } else if (mode == 10) {
        int n;
        if (fscanf(in, "%d", &n) != 1) { fprintf(stderr, "Missing board size\n"); return 1; }
        solve_domination(n);
    } else {
        fprintf(stderr, "Task number must be 1..10\n");
        return 1;
    }
    if (in != stdin) fclose(in);
    return 0;
}
