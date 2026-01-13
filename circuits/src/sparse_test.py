from __future__ import annotations
from typing import Self


class Permutation:
    def __init__(self, n: int, transpositions: list[tuple[int, int]] = []) -> None:
        self._perm = list(range(n))
        self.n = n

        for t in transpositions:
            self._perm[t[0]], self._perm[t[1]] = self._perm[t[1]], self._perm[t[0]]

    def __call__(self, i: int) -> int:
        return self._perm[i]

    def __mul__(self, other: Permutation) -> Permutation:
        if self.n != other.n: raise ValueError("Permutations must belong to the same group!")
        p = Permutation(self.n)
        for i in range(self.n):
            p._perm[i] = self._perm[other._perm[i]]
        return p

    def __imul__(self, other: Permutation | tuple[int, int]) -> Permutation:
        if isinstance(other, Permutation):
            self._perm = (self * other)._perm[:]
            return self
        else:
            self._perm[other[0]], self._perm[other[1]] = self._perm[other[1]], self._perm[other[0]]
            return self

    def __str__(self) -> str:
        return str(self._perm)
    
    def __repr__(self) -> str:
        return str(self)

# sorted csc form
class CSC[T]:
    def __init__(self, r: int, s: int, data: list[T], rows: list[int], ptrs: list[int]) -> None:
        self.data = data
        self.rows = rows
        self.ptrs = ptrs
        self.r = r
        self.s = s

    @classmethod
    def from_triplets(cls, r: int, s: int, triplets: list[tuple[T, int, int]]) -> Self:
        triplets.sort(key=lambda x: x[2])

        data: list[T] = []
        rows: list[int] = []
        ptrs = [0]

        last_col = 0
        for v, i, j in triplets:
            while j > last_col:
                ptrs.append(len(data))
                last_col += 1
            data.append(v)
            rows.append(i)
        
        ptrs.append(len(data))
                        
        return cls(r, s, data, rows, ptrs)

    def __str__(self) -> str:
        return f"{self.r}, {self.s}\n{self.data}\n{self.rows}\n{self.ptrs}"

    def _get_col_from_id(self, k: int) -> int:
        if k >= self.ptrs[self.s]: raise ValueError("Too many non-zeros!")
    
        interval = [0, self.s]
        
        while True:
            middle = (interval[1] + interval[0]) // 2
            value = self.ptrs[middle]
            
            if value == k:
                return middle
            elif value > k:
                if self.ptrs[middle - 1] < k:
                    return middle - 1
                interval[1] = middle - 1
            else:
                if self.ptrs[middle + 1] > k:
                    return middle
                interval[0] = middle + 1

    def generate_pivoting(self) -> None:
        if self.r != self.s: raise ValueError("Matrix must be square!")

        # row perm
        p = Permutation(self.r)
        # column perm
        q = Permutation(self.s)

        # create footprint
        footprint: list[list[int]] = [[] for _ in range(self.s)]

        j = 0
        for l, i in enumerate(self.rows):
            if l >= self.ptrs[j + 1]: j += 1
            footprint[j].append(i)
                
        for k in range(self.r):
            num_nz_col = [len(col) for col in footprint]
            num_nz_row = [0] * self.r

            for col in footprint:
                for row in col:
                    num_nz_row[row] += 1

            min_markowitz: int | None = None
            pivot_i = 0
            pivot_j = 0

            for j, col in enumerate(footprint):
                for i in col:
                    if i < p(k) or j < q(k): continue

                    markowitz = (num_nz_row[i] - 1) * (num_nz_col[j] - 1)
                    if min_markowitz is None or markowitz < min_markowitz:
                        min_markowitz = markowitz
                        pivot_i = i
                        pivot_j = j

            p *= (p(k), pivot_i)
            q *= (q(k), pivot_j)

            pivot_col = footprint[p(k)]
            fill_ins = {x for x in pivot_col if x > }

            for j, col in enumerate(footprint):
                if q(j) <= k: continue
                

        print("")

matrix = [
    [ 5, -2,  6,  7],
    [ 3,  5,  0,  0],
    [ 1,  0,  2,  0],
    [-7,  0,  0, -3],
]

R = len(matrix)
S = len(matrix[0]) if R > 0 else 0

sparse_triplets = [(matrix[i][j], i, j) for i in range(R) for j in range(S) if matrix[i][j] != 0]


sparse_csc = CSC[int].from_triplets(R, S, sparse_triplets)

print(sparse_csc)

sparse_csc.generate_pivoting()


a = Permutation(5)

a *= (0, 1)
print(a)
a *= (0, 2)
print(a)
a *= (3, 4)
print(a)
a *= (3, 1)
print(a)
