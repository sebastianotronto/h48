_static int64_t factorial(int64_t);
_static bool isperm(uint8_t *, int64_t);
_static int64_t permtoindex(uint8_t *, int64_t);
_static void indextoperm(int64_t, int64_t, uint8_t *);
_static int permsign(uint8_t *, int64_t);

_static int64_t
factorial(int64_t n)
{
	int64_t i, ret;

	if (n > _max_factorial) {
		LOG("Error: won't compute factorial for n=%" PRId64 " because"
		    " it is larger than %" PRId64 "\n", n, _max_factorial);
		return -1;
	}

	if (n < 0)
		return 0;

	for (i = 1, ret = 1; i <= n; i++)
		ret *= i;

	return ret;
}

_static bool
isperm(uint8_t *a, int64_t n)
{
	int64_t i;
	bool aux[_max_factorial+1];

	if (n > _max_factorial) {
		LOG("Error: won't compute 'isperm()' for n=%" PRId64 " because"
		    " it is larger than %" PRId64 "\n", n, _max_factorial);
		return false;
	}

	memset(aux, false, n);
	
	for (i = 0; i < n; i++) {
		if (a[i] < 0 || a[i] >= n)
			return false;
		else
			aux[a[i]] = true;
	}

	for (i = 0; i < n; i++)
		if (!aux[i])
			return false;

	return true;
}

_static int64_t
permtoindex(uint8_t *a, int64_t n)
{
	int64_t i, j, c, ret;

	if (n > _max_factorial) {
		LOG("Error: won't compute 'permtoindex()' for n=%" PRId64
		    " because it is larger than %" PRId64 "\n",
		    n, _max_factorial);
		return -1;
	}

	if (!isperm(a, n))
		return -1;

	for (i = 0, ret = 0; i < n; i++) {
		for (j = i+1, c = 0; j < n; j++)
			c += (a[i] > a[j]) ? 1 : 0;
		ret += factorial(n-i-1) * c;
	}

	return ret;
}

_static void
indextoperm(int64_t p, int64_t n, uint8_t *r)
{
	int64_t i, j, c;
	uint8_t a[_max_factorial+1];

	if (n > _max_factorial) {
		LOG("Error: won't compute 'permtoindex()' for n=%" PRId64
		    " because it is larger than %" PRId64 "\n",
		    n, _max_factorial);
		goto indextoperm_error;
	}

	memset(a, 0, n);

	if (p < 0 || p >= factorial(n))
		goto indextoperm_error;

	for (i = 0; i < n; i++) {
		for (j = 0, c = 0; c <= p / factorial(n-i-1); j++)
			c += a[j] ? 0 : 1;
		r[i] = j-1;
		a[j-1] = 1;
		p %= factorial(n-i-1);
	}

	if (!isperm(r, n))
		goto indextoperm_error;

	return;

indextoperm_error:
	memset(r, _error, n);
}

_static int
permsign(uint8_t *a, int64_t n)
{
	int i, j;
	uint8_t ret;

	for (i = 0, ret = 0; i < n; i++)
		for (j = i+1; j < n; j++)
			ret += a[i] > a[j] ? 1 : 0;

	return ret % 2;
}
