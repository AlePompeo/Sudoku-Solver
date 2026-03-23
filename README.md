# 🧩 Parallel Sudoku Solver in C

A high-performance Sudoku solver written in C, combining a classic **Depth First Search (DFS)** backtracking algorithm with **POSIX multithreading** (`pthreads`) and **OpenMP** compiler optimizations.

> ⚡ Fastest recorded run on a Ryzen Mobile (6-core CPU): **0.054 seconds**

---

## 🚀 Features

- **DFS + Backtracking** — the solver explores all possible candidates recursively, pruning dead ends immediately.
- **Multithreaded execution** — each empty cell spawns a dedicated thread, allowing parallel exploration of independent solution branches.
- **State machine design** — each thread is modeled as a finite state machine (`RUNNING`, `SUCCEDED`, `FAILED`), enabling clean coordination and early exit on success.
- **Compiler-level optimization** — uses `#pragma GCC optimize("Ofast")` and the `-fopenmp` flag for maximum throughput.

---

## 🛠️ Build & Run

### Requirements
- GCC with OpenMP support
- POSIX-compatible OS (Linux / macOS)

### Compile
```bash
gcc -fopenmp sudoku.c -o sudoku
```

### Run
```bash
./sudoku
```

---

## 🧠 How It Works

### Algorithm: DFS with Backtracking

The core solver (`mySolver`) scans the grid column by column looking for empty cells (value `0`). For each empty cell it:
1. Tries every digit from `1` to `9`.
2. Validates the candidate against row, column, and 3×3 box constraints (`isAllowed`).
3. Recurses deeper if valid.
4. Backtracks by resetting the cell to `0` if no digit leads to a solution.

This is equivalent to exploring a **decision tree** where each node represents a partial board state.

### Parallelism: Thread-per-Cell

The `solve` function creates one thread per empty cell found on the initial board. Each thread (`runner`) receives a **copy** of the board, pins a candidate digit to its assigned cell, and independently runs the full DFS solver on it.

The main thread polls all runners in a loop and:
- Copies the solved board back as soon as one thread reports `SUCCEDED`.
- Terminates gracefully if all threads report `FAILED`.

```
Initial board
     │
     ├── Thread (cell A, digit 1) ──► DFS ──► SUCCEDED ✓
     ├── Thread (cell A, digit 2) ──► DFS ──► ...
     ├── Thread (cell B, digit 5) ──► DFS ──► ...
     └── ...
```

### Constraint Check: `isAllowed`

For a candidate digit `n` at position `(row, col)`, the function checks three constraints in a single loop:
- **Row** — no duplicate in the same row.
- **Column** — no duplicate in the same column.
- **3×3 Box** — uses integer arithmetic to map the linear loop index `i` to the correct sub-grid cell, avoiding expensive modulo operations.

---

## 📋 Predefined Puzzles

The project ships with four built-in Sudoku puzzles (`problem1` – `problem4`) of varying difficulty. Switch between them in `main()`:

```c
solve(&problem4);  // change to problem1, problem2, or problem3
```

---

## 📊 Output Format

The solved grid is printed with visual separators between the 3×3 boxes:

```
+-----+-----+-----+
|8|1|2|3|6|7|4|9|5|
|4|3|7|5|9|2|6|5|1|
...
+-----+-----+-----+
```

---

## 📁 Project Structure

```
.
└── sudoku.c       # Single-file implementation
```

---

## ⚠️ Notes

- The polling loop in `solve()` is a **busy-wait** — suitable for short-lived solver threads but not ideal for long-running workloads.
- Thread safety relies on each thread operating on its **own copy** of the board, so no mutexes are needed.
- The `-fopenmp` flag enables OpenMP's runtime optimizations even if no explicit `#pragma omp` directives are used in this version.

---

## 📄 License

MIT
