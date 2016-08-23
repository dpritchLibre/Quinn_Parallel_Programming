
#define BLOCK_LOW(id, p, n)  ((id) * (n) / (p))
#define BLOCK_HIGH(id, p, n)  (BLOCK_LOW((id) + 1, (p), (n)) - 1)
#define BLOCK_SIZE(id, p, n)  (BLOCK_LOW((id) + 1, (p), (n)) - BLOCK_LOW((id), (p), (n)))
#define BLOCK_OWNER(index, p, n) (((p) * ((index) + 1) - 1) / (n))
