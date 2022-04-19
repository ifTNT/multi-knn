# EMCSS-Pthread-KNN

Pthread personal project of 2022 EMCSS

## Methods

### Notation
- n: numbers of testing data
- m: numbers of training data
- k: numbers of nearest neighbor
- p: numbers of threads

### A. Single Thread Brute Force (SBF)

Complexity: $O(nm)$

### B. Multithread Brute Force (MBF)

Complexity: $O(nm/p)$

### C. Single Thread Tree Search (STS)

Complexity: $O(n \cdot lg(m))$

### D. Multithread Tree Search (MTS)

Complexity: $O(\frac{n \cdot lg(m)}{p})$

## Dependency

- Pthread
- CMake 3.10 or above

## Build

```
mkdir build
cd build
cmake ..
make
```

## Execute

```
./build/knn
```

## Author

Yung-Hsiung Hu @2022
