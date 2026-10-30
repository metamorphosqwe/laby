#include <stdio.h>
#include <string.h>

#define MAX_V 50
#define MAX_E 200

int adjacency[MAX_V][MAX_V];
int degree[MAX_V];
char labels[MAX_V][8];
int edge_u[MAX_E];
int edge_v[MAX_E];
int visited[MAX_V];
int euler_cycle[MAX_E + 1];
int hamilton_path[MAX_V];
int hamilton_used[MAX_V];

int label_less(const char *a, const char *b)
{
    int la = (int)strlen(a);
    int lb = (int)strlen(b);
    if (la != lb)
        return la < lb;
    return strcmp(a, b) < 0;
}

int read_graph(const char *filename, int *n_out, int *m_out)
{
    FILE *f = fopen(filename, "r");
    if (!f)
        return 0;

    int m = 0;
    if (fscanf(f, "%d", &m) != 1 || m < 1 || m > MAX_E)
    {
        fclose(f);
        return 0;
    }

    char a[MAX_E][8];
    char b[MAX_E][8];
    int n = 0;

    for (int i = 0; i < m; i++)
    {
        if (fscanf(f, "%7s %7s", a[i], b[i]) != 2)
        {
            fclose(f);
            return 0;
        }

        const char *pair[2] = {a[i], b[i]};
        for (int k = 0; k < 2; k++)
        {
            int found = 0;
            for (int j = 0; j < n; j++)
            {
                if (strcmp(labels[j], pair[k]) == 0)
                {
                    found = 1;
                    break;
                }
            }

            if (!found)
            {
                if (n >= MAX_V)
                {
                    fclose(f);
                    return 0;
                }
                strcpy(labels[n++], pair[k]);
            }
        }
    }
    fclose(f);

    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if (label_less(labels[j], labels[i]))
            {
                char t[8];
                strcpy(t, labels[i]);
                strcpy(labels[i], labels[j]);
                strcpy(labels[j], t);
            }
        }
    }

    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (strcmp(labels[j], a[i]) == 0)
                edge_u[i] = j;
            if (strcmp(labels[j], b[i]) == 0)
                edge_v[i] = j;
        }
    }

    *n_out = n;
    *m_out = m;
    return 1;
}

void build_adjacency(int n, int m)
{
    for (int i = 0; i < n; i++)
    {
        degree[i] = 0;
        for (int j = 0; j < n; j++)
            adjacency[i][j] = 0;
    }

    for (int i = 0; i < m; i++)
    {
        int u = edge_u[i];
        int v = edge_v[i];

        adjacency[u][v]++;
        if (u != v)
            adjacency[v][u]++;

        degree[u]++;
        degree[v]++;
    }
}

void print_graph(int n, int m)
{
    printf("Vertices: %d, Edges: %d\n", n, m);
    printf("Edges:");
    for (int i = 0; i < m; i++)
        printf(" (%s,%s)", labels[edge_u[i]], labels[edge_v[i]]);

    printf("\nAdjacency matrix:\n    ");
    for (int j = 0; j < n; j++)
        printf("%-3s ", labels[j]);
    printf("\n");

    for (int i = 0; i < n; i++)
    {
        printf("%-3s ", labels[i]);
        for (int j = 0; j < n; j++)
            printf("%-3d ", adjacency[i][j]);
        printf("\n");
    }

    printf("Degrees:");
    for (int i = 0; i < n; i++)
        printf(" %s=%d", labels[i], degree[i]);
    printf("\n");
}

int is_connected(int n)
{
    int stack[MAX_V];
    int top = 0;
    int start = -1;

    for (int i = 0; i < n; i++)
        visited[i] = 0;

    for (int i = 0; i < n; i++)
    {
        if (degree[i] > 0)
        {
            start = i;
            break;
        }
    }

    if (start == -1)
        return 1;

    stack[top++] = start;
    visited[start] = 1;

    while (top > 0)
    {
        int v = stack[--top];
        for (int to = 0; to < n; to++)
        {
            if (adjacency[v][to] && !visited[to])
            {
                visited[to] = 1;
                stack[top++] = to;
            }
        }
    }

    for (int i = 0; i < n; i++)
    {
        if (!visited[i])
            return 0;
    }
    return 1;
}

int has_euler_cycle(int n)
{
    if (!is_connected(n))
    {
        printf("Graph is not connected\n");
        return 0;
    }

    for (int i = 0; i < n; i++)
    {
        if (degree[i] % 2 != 0)
        {
            printf("Vertex %s has odd degree (%d)\n", labels[i], degree[i]);
            return 0;
        }
    }
    return 1;
}

int find_euler_cycle(int n, int m)
{
    int work[MAX_V][MAX_V];
    int stack[MAX_E + 2];
    int top = 0;
    int len = 0;

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            work[i][j] = adjacency[i][j];

    stack[top++] = 0;

    while (top > 0)
    {
        int v = stack[top - 1];
        int next = -1;

        for (int to = 0; to < n; to++)
        {
            if (work[v][to] > 0)
            {
                next = to;
                break;
            }
        }

        if (next == -1)
        {
            euler_cycle[len++] = v;
            top--;
        }
        else
        {
            work[v][next]--;
            work[next][v]--;
            stack[top++] = next;
        }
    }

    return len == m + 1 ? len : 0;
}

int hamilton_dfs(int n, int pos)
{
    int last = hamilton_path[pos - 1];

    if (pos == n)
        return adjacency[last][hamilton_path[0]] > 0;

    for (int to = 0; to < n; to++)
    {
        if (adjacency[last][to] > 0 && !hamilton_used[to])
        {
            hamilton_used[to] = 1;
            hamilton_path[pos] = to;
            if (hamilton_dfs(n, pos + 1))
                return 1;
            hamilton_used[to] = 0;
        }
    }
    return 0;
}

int find_hamilton_cycle(int n)
{
    if (n < 3)
        return 0;

    for (int i = 0; i < n; i++)
    {
        if (degree[i] < 2)
        {
            printf("Vertex %s has degree < 2\n", labels[i]);
            return 0;
        }
    }

    if (!is_connected(n))
    {
        printf("Graph is not connected\n");
        return 0;
    }

    int dirac = 1;
    for (int i = 0; i < n; i++)
    {
        if (2 * degree[i] < n)
            dirac = 0;
    }
    printf("Dirac condition (deg >= n/2): %s\n", dirac ? "satisfied" : "not satisfied");

    for (int i = 0; i < n; i++)
        hamilton_used[i] = 0;

    hamilton_path[0] = 0;
    hamilton_used[0] = 1;

    return hamilton_dfs(n, 1);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Usage: %s <euler|hamilton> <file>\n", argv[0]);
        return 1;
    }

    int n = 0;
    int m = 0;

    if (!read_graph(argv[2], &n, &m))
    {
        printf("Cannot read file: %s\n", argv[2]);
        return 1;
    }

    build_adjacency(n, m);
    printf("File: %s\n", argv[2]);
    print_graph(n, m);

    if (strcmp(argv[1], "euler") == 0)
    {
        if (!has_euler_cycle(n))
        {
            printf("Euler cycle does not exist\n");
            return 0;
        }

        int len = find_euler_cycle(n, m);
        printf("Euler cycle:");
        for (int i = 0; i < len; i++)
            printf(" %s", labels[euler_cycle[i]]);
        printf("\n");
    }
    else
    {
        if (!find_hamilton_cycle(n))
        {
            printf("Hamilton cycle does not exist\n");
            return 0;
        }

        printf("Hamilton cycle:");
        for (int i = 0; i < n; i++)
            printf(" %s", labels[hamilton_path[i]]);
        printf(" %s\n", labels[hamilton_path[0]]);
    }

    return 0;
}