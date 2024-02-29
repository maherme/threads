#ifndef BITSOP_H
#define BITSOP_H

#define IS_BIT_SET(n, bit)  (((n) & (1 << (bit))) != 0)
#define TOGGLE_BIT(n, bit)  ((n) ^= (1 << (bit)))
#define COMPLEMENT(n)       (~(n))
#define UNSET_BIT(n, bit)   ((n) &= ~(1 << (bit)))
#define SET_BIT(n, bit)     ((n) |= (1 << (bit)))

#endif /* BITSOP_H */
