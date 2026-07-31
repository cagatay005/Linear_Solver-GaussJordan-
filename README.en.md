# Gauss-Jordan Linear Equation Solver

*Other languages: [Türkçe](README.md)*

An educational linear equation system solver written in pure C (C11),
with no external dependencies. It transforms the system entered by the
user into reduced row echelon form (RREF) using Gauss-Jordan
elimination (with partial pivoting) and explains every row operation
performed along the way, step by step.

## Purpose

The program doesn't just compute a result; it makes the entire
mathematical solving process visible (pivot selection, row swapping,
normalization, elimination), determines the type of the system (unique
solution / infinitely many solutions / no solution), generates a
parametric solution for systems with infinitely many solutions, and
verifies the solution it finds for uniquely-solvable systems by
substituting it back into the original equations.

## The Gauss-Jordan Method (Briefly)

Gauss-Jordan elimination repeats the following steps on an augmented
matrix `[A|b]` to bring the matrix into RREF:

1. In the column being processed (left to right), the row with the
   largest absolute value is chosen as the pivot (partial pivoting).
2. If necessary, the pivot row is swapped with the row being worked on.
3. The pivot row is normalized so the pivot element becomes 1.
4. The appropriate multiple of the pivot row is subtracted from every
   other row (both above and below) to zero out that column.
5. The algorithm moves to the next row/column; columns where no valid
   pivot can be found are marked as free-variable columns.

Once the process is complete, `rank(A)` and `rank([A|b])` are compared
to determine whether the system has a unique solution, infinitely many
solutions, or no solution.

## Features

- Supports square, rectangular (more/fewer equations than variables),
  zero/negative/decimal-coefficient systems.
- Numerical stability via partial pivoting.
- Displays every row operation (`R_i <-> R_j`, `R_i <- R_i / c`,
  `R_i <- R_i + c * R_j`) and the matrix after that operation.
- Verbose mode and result-only mode.
- Rank computation, pivot/free-variable detection.
- Dynamically generated parametric solution for systems with
  infinitely many solutions (`t1`, `t2`, ...).
- Displays the contradictory row (`0 = c`) for inconsistent systems.
- Verifies the solution of uniquely-solvable systems by substituting
  it back into the original equations, with residual error reporting.
- Operation statistics (row swap, normalization, elimination counts,
  epsilon value).
- Keyboard or file input; command-line file name support.
- Saves the solution report to `solution_report.txt` (or a chosen
  name).
- Safe input reading based on `fgets` + `strtod`/`strtol`; the program
  never crashes on invalid input, it re-prompts instead.
- Safe dynamic memory management: every allocation is checked, and all
  memory is freed at the end of the program.

## Project Folder Structure

```text
linear_solver/
|-- include/       Header files (matrix.h, gauss_jordan.h, ...)
|-- src/            Source files (main.c, matrix.c, ...)
|-- tests/          Test systems (.txt)
|-- examples/       Example systems
|-- docs/           Screenshots (for the README)
|-- Makefile
|-- README.md
`-- README.en.md
```

Modules:

| Module            | Responsibility                                            |
|-------------------|-------------------------------------------------------------|
| `matrix`          | Dynamic matrix data structure and basic row operations       |
| `parser`          | Safe number parsing (`fgets` + `strtod`/`strtol`)             |
| `input`           | Keyboard/file input, validation, menus                       |
| `gauss_jordan`    | Gauss-Jordan elimination with partial pivoting, RREF          |
| `solution`        | Pivot/free-variable detection, parametric solution generation |
| `verification`    | Verifying the solution against the original system, residual  |
| `output`          | Shared `FILE *`-based terminal/file output functions          |
| `main`            | Orchestration: managing the flow end to end                   |

## Building

Requirements: `gcc` (with C11 support) and `make` (`mingw32-make` on
Windows).

```bash
make
```

Release (optimized) build:

```bash
make release
```

Run:

```bash
make run
```

Run the tests:

```bash
make test
```

Clean:

```bash
make clean
```

Building directly with MinGW on Windows:

```bash
gcc -std=c11 -D__USE_MINGW_ANSI_STDIO=1 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Iinclude -o solver.exe src/*.c
```

> Note: MinGW's old `printf` implementation doesn't support C11 size
> specifiers such as `%zu`, so the `-D__USE_MINGW_ANSI_STDIO=1` flag is
> used. This flag is not needed on Linux/macOS builds (the Makefile
> detects this automatically).

## Usage Examples

### Keyboard input

```bash
./solver
```

```text
1 - Klavyeden giris
2 - Dosyadan giris
Seciminiz: 1
Denklem sayisi: 3
Degisken sayisi: 3

1. denklemin katsayilarini girin:
x1: 2
x2: 1
x3: -1
Sabit terim: 8
...
```

> Note: the program's interactive prompts are in Turkish, since it was
> written for Turkish-speaking users; the underlying logic and code
> are language-independent.

### File input (command line)

```bash
./solver tests/test_unique.txt
```

### File input (from the menu)

```text
1 - Klavyeden giris
2 - Dosyadan giris
Seciminiz: 2
Dosya adi: tests/test_unique.txt
```

## Screenshots

**Verbose mode — uniquely-solvable system, including a row swap (`tests/test_row_swap.txt`)**

![Verbose mode screenshot](docs/screenshot_unique_verbose.png)

**Result-only mode — system with infinitely many solutions, parametric solution (`tests/test_infinite.txt`)**

![Infinite-solution screenshot](docs/screenshot_infinite.png)

**Result-only mode — inconsistent (unsolvable) system (`tests/test_inconsistent.txt`)**

![Inconsistent-system screenshot](docs/screenshot_inconsistent.png)

## File Input Format

```text
equation_count variable_count
coefficient_1 coefficient_2 ... coefficient_n constant_term
...
```

Example (`tests/test_unique.txt`):

```text
3 3
2 1 -1 8
-3 -1 2 -11
-2 1 2 -3
```

## Unique-Solution Example

Input: `tests/test_unique.txt` → Output:

```text
Sistem turu: Tek cozumlu sistem

x1 = 2.000000
x2 = 3.000000
x3 = -1.000000
```

("Tek cozumlu sistem" = "System with a unique solution".)

## Infinite-Solution Example

Input: `tests/test_infinite.txt` (2 equations, 3 variables, the second
equation is twice the first) → Output:

```text
Sistem turu: Sonsuz cozumlu sistem

Serbest degiskenler:
x2 = t1
x3 = t2

Pivot degiskenleri:
x1 = 3.000000 - 2.000000*t1 + 1.000000*t2
x2 = 1.000000*t1
x3 = 1.000000*t2
```

("Sonsuz cozumlu sistem" = "System with infinitely many solutions",
"Serbest degiskenler" = "Free variables", "Pivot degiskenleri" =
"Pivot variables".)

## Inconsistent-System Example

Input: `tests/test_inconsistent.txt` → Output:

```text
Cozum bulunamadi.

Cunku indirgenmis matriste su celiskili satir olustu:

[     0.000000     0.000000 |    -0.500000 ]

Bu satir matematiksel olarak 0 = -0.500000 anlamina gelir.
Dolayisiyla sistem tutarsizdir.
```

("Cozum bulunamadi." = "No solution found." The following lines
explain that the reduced matrix produced this contradictory row, which
mathematically means `0 = -0.500000`, so the system is inconsistent.)

(The constant on this row can be a different nonzero value depending
on the pivots chosen; what matters is that all coefficients are zero
while the constant term is nonzero.)

## Numerical Precision

The program never compares a value with `== 0.0`. Instead, it uses
`DEFAULT_EPSILON` (`1e-10`), defined in `config.h`: a value is
considered zero if `fabs(value) < epsilon`. Pivot selection always
picks the row with the largest absolute value in the current column
(partial pivoting), which reduces rounding error. If a pivot's
absolute value falls below `NEAR_SINGULAR_THRESHOLD` (`1e-6`) but is
still above epsilon, the program prints a "system may be numerically
unstable" warning. All values printed to the screen normalize
negative-zero artifacts such as `-0.000000` to `0.000000`.

## Running the Tests

The systems in `tests/` are run automatically with `make test`; they
are, in order:

- `test_unique.txt` — uniquely-solvable square system
- `test_row_swap.txt` — system requiring a row swap
- `test_infinite.txt` — system with infinitely many solutions
- `test_inconsistent.txt` — inconsistent (unsolvable) system
- `test_rectangular.txt` — more equations than variables, consistent
- `test_decimal.txt` — decimal coefficients
- `test_underdetermined.txt` — more variables than equations
- `test_near_singular.txt` — nearly singular system

To run a single system manually:

```bash
./solver tests/test_infinite.txt
```

## Known Limitations

- Very large systems are rejected (`MAX_EQUATION_COUNT` /
  `MAX_VARIABLE_COUNT`, default 1000), in order to prevent overflow
  and excessive memory usage.
- The program uses `double` precision, not arbitrary precision;
  extremely ill-conditioned systems with very large/small coefficient
  differences may exceed the epsilon bound in residual error (this is
  reported during the verification step).
- Rational arithmetic is not supported; all computations are done with
  `double`.

## Possible Future Additions

- Gaussian elimination (REF only), LU decomposition
- Determinant and matrix inverse computation
- Homogeneous system solving, null space / row-column space
- Cramer's rule, Jacobi, Gauss-Seidel methods
- Rational arithmetic or arbitrary-precision arithmetic mode
- Forward/backward replay of solution steps
- Graphical user interface or HTML/SVG report output
