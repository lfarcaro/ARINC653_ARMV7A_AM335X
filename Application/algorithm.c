// General includes
#include "algorithm.h"

// Algorithm startup method
void ALGORITHM_STARTUP(void) {
}

// Algorithm initialization method
void ALGORITHM_INITIALIZE(void) {
	unsigned int i;
	for (i = 0; i < BSORT_SIZE; i++) {
		bsort_sa[i] = BSORT_SIZE - i;
	}
}

// Algorithm execution method
void ALGORITHM_EXECUTE(void) {
	for (bsort_el = 0; bsort_el < BSORT_SIZE; bsort_el++) {
		bsort_b = 0;
		for (bsort_il = 0; bsort_il < (BSORT_SIZE-1); bsort_il++) {
			if (bsort_sa[bsort_il] > bsort_sa[bsort_il + 1]) {
				bsort_t = bsort_sa[bsort_il];
				bsort_sa[bsort_il] = bsort_sa[bsort_il + 1];
				bsort_sa[bsort_il + 1] = bsort_t;
				bsort_b = 1;
			}
		}
		if (bsort_b == 0) {
			break;
		}
	}
}
