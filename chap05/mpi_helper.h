
/* The following functions are for use in the setting where we wish to divide
 * the indices from 0 to n - 1 into size nearly even contiguous groups.  For
 * example, if we had n = 14 elements and 4 groups, we might decompose the
 * element indices into the groups {0, 1, 2}, {3, 4, 5, 6}, {7, 8, 9}, {10, 11,
 * 12, 13}
 */

// Find the lowest index for the rank-th group
#define BLOCK_LOW(rank, size, n)  ((rank) * (n) / (size))

// Find the highest index for the rank-th group
#define BLOCK_HIGH(rank, size, n)  (BLOCK_LOW((rank) + 1, (size), (n)) - 1)

// Find the number of elements (indices) in the rank-th group
#define BLOCK_SIZE(rank, size, n)  (BLOCK_LOW((rank) + 1, (size), (n))  \
				    - BLOCK_LOW((rank), (size), (n)))

// Given an index value idx, find which group it is in
#define BLOCK_OWNER(idx, size, n) (((size) * ((idx) + 1) - 1) / (n))
