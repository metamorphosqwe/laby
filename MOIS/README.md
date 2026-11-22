# MOIS2-MOIS8

Each `MOISN` directory contains a C11 solution and 20 input files in `tests`.
The programs accept a test file path as their first command-line argument; if
no argument is given, they read from standard input.

Run all 140 tests from PowerShell in this directory:

```powershell
.\run_all_tests.ps1
```

The runner compiles all seven programs with GCC, invokes each test file as an
argument, and compares its output with the matching file in `tests/expected`.

## Input formats

- **MOIS2:** two graphs in sequence. Each graph starts with `n m`, followed by
  `m` undirected edges `u v`; vertices are numbered from 1. The first graph is
  checked for an Euler cycle and the second for a Hamilton cycle.
- **MOIS3:** `n m`, then `m` undirected edges `u v weight`; the source is
  vertex 1 unless a final source vertex is supplied. Prints Dijkstra and
  Floyd-Warshall distances from the source.
- **MOIS4:** `n m`, then `m` weighted undirected edges `u v weight`. Prints
  the Prim and Kruskal minimum spanning trees and their total weights.
- **MOIS5:** directed graph `n1 m1` and `m1` arcs, followed by undirected
  graph `n2 m2` and `m2` edges. Vertices are numbered from 1.
- **MOIS6:** two trees, each encoded as `n m` followed by `m` directed
  parent-child pairs. Vertex 1 is the root; child order may be swapped.
- **MOIS7:** first value selects the task: `1` for a permutation (`n`, then
  its images); `2` for a finite cyclic group (`ADD|MUL modulus count`,
  then the group elements); `3` for one of the two homomorphisms in the
  assignment (`1` or `2`).
- **MOIS8:** first value selects one of the ten assignment problems in the
  order given in `Задания/l8 mois.md`. Each mode then reads its natural input:
  size for queens/magic squares; dimensions and start for a knight tour;
  size and start cell for board filling; dimensions, endpoints, and rows for
  a maze; item count/capacity/items for knapsack; city count/matrix for TSP.
  Search-intensive modes have explicit size limits reported by the program.

The graph and tree programs use numeric vertex labels. The test inputs include
the assignment examples where applicable, disconnected/unreachable cases,
degenerate structures, and large 200-vertex graph/tree cases.
