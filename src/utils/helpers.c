#include "utils.h"

inline static void __ft_qsort_swap(void *a, void *b, size_t width) {
	unsigned char *A = a, *B = b, tmp;

	for (size_t i = 0; i < width; i++) {
		tmp  = A[i];
		A[i] = B[i];
		B[i] = tmp;
	}
}

static size_t
__ft_qsort_partition(void *data, size_t lo, size_t hi, size_t width, int (*comp)(const void *, const void *)) {
	void  *P = data + (width * hi);
	size_t i = lo - 1;

	for (size_t j = lo; j <= hi - 1; j++) {
		if (comp(data + (j * width), P) <= 0) {
			__ft_qsort_swap(data + (++i * width), data + (j * width), width);
		}
	}

	__ft_qsort_swap(data + (++i * width), data + (hi * width), width);

	return i;
}

static void __ft_qsort(void  *data,// NOLINT(*-no-recursion)
                       size_t lo,
                       size_t hi,
                       size_t width,
                       int    (*comp)(const void *, const void *)) {
	if (lo >= hi)
		return;

	size_t P = __ft_qsort_partition(data, lo, hi, width, comp);

	__ft_qsort(data, lo, P - 1, width, comp);
	__ft_qsort(data, P + 1, hi, width, comp);
}

void ft_qsort(void *data, size_t nb_el, size_t width, int (*comp)(const void *, const void *)) {
	__ft_qsort(data, 0, nb_el - 1, width, comp);
}
