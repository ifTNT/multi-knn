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

## Dependency

- Pthread
- CMake 3.10 or above

## Build

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

## Execute

```
./build/knn <K> <#Threads>
```

## Author

Yung-Hsiung Hu @2022
