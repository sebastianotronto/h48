#define _swap(x, y) do { x ^= y; y ^= x; x ^= y; } while (0)
#define _min(x, y) ((x) < (y) ? (x) : (y))
#define _max(x, y) ((x) > (y) ? (x) : (y))
#define _div_round_up(n, d) (((n) + (d) - 1) / (d))

STATIC int64_t factorial(int64_t);
STATIC bool isperm(uint8_t *, int64_t);
STATIC int64_t permtoindex(uint8_t *, int64_t);
STATIC void indextoperm(int64_t, int64_t, uint8_t *);
STATIC int permsign(uint8_t *, int64_t);
STATIC int64_t digitstosumzero(uint8_t *, uint8_t, uint8_t);
STATIC void sumzerotodigits(int64_t, uint8_t, uint8_t, uint8_t *);

STATIC int64_t
factorial(int64_t n)
{
	int64_t i, ret;

	if (n > FACTORIAL_MAX) {
		LOG("Error: won't compute factorial for n=%" PRId64 " because"
		    " it is larger than %" PRId64 "\n", n, FACTORIAL_MAX);
		return -1;
	}

	if (n < 0)
		return 0;

	for (i = 1, ret = 1; i <= n; i++)
		ret *= i;

	return ret;
}

STATIC bool
isperm(uint8_t *a, int64_t n)
{
	int64_t i;
	bool aux[FACTORIAL_MAX+1];

	if (n > FACTORIAL_MAX) {
		LOG("Error: won't compute 'isperm()' for n=%" PRId64 " because"
		    " it is larger than %" PRId64 "\n", n, FACTORIAL_MAX);
		return false;
	}

	memset(aux, false, n);
	
	for (i = 0; i < n; i++) {
		if (a[i] >= n)
			return false;
		else
			aux[a[i]] = true;
	}

	for (i = 0; i < n; i++)
		if (!aux[i])
			return false;

	return true;
}

STATIC int64_t
permtoindex(uint8_t *a, int64_t n)
{
	int64_t i, j, c, ret;

	if (n > FACTORIAL_MAX) {
		LOG("Error: won't compute 'permtoindex()' for n=%" PRId64
		    " because it is larger than %" PRId64 "\n",
		    n, FACTORIAL_MAX);
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

STATIC void
indextoperm(int64_t p, int64_t n, uint8_t *r)
{
	int64_t i, j, c;
	uint8_t a[FACTORIAL_MAX+1];

	if (n > FACTORIAL_MAX) {
		LOG("Error: won't compute 'permtoindex()' for n=%" PRId64
		    " because it is larger than %" PRId64 "\n",
		    n, FACTORIAL_MAX);
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
	memset(r, UINT8_ERROR, n);
}

STATIC int
permsign(uint8_t *a, int64_t n)
{
	int i, j;
	uint8_t ret;

	for (i = 0, ret = 0; i < n; i++)
		for (j = i+1; j < n; j++)
			ret += a[i] > a[j] ? 1 : 0;

	return ret % 2;
}

STATIC int64_t
digitstosumzero(uint8_t *a, uint8_t n, uint8_t b)
{
	int64_t ret, p;
	uint8_t i, sum;

	if (!((n == 8 && b == 3 ) || (n == 12 && b == 2))) {
		LOG("Won't compute 'sumzero' for n=%" PRIu8 "and b=%" PRIu8
		    " (use n=8 b=3 or n=12 b=2)\n", n, b);
		return -1;
	}

	for (i = 1, ret = 0, p = 1, sum = 0; i < n; i++, p *= (int64_t)b) {
		if (a[i] >= b) {
			LOG("Error: digit %" PRIu8 " larger than maximum"
			    " (b=%" PRIu8 "\n", a[i], b);
			return -1;
		}
		sum += a[i];
		ret += p * (int64_t)a[i];
	}

	if ((sum + a[0]) % b != 0) {
		LOG("Error: digits do not have sum zero modulo b\n");
		return -1;
	}

	return ret;
}

STATIC void
sumzerotodigits(int64_t d, uint8_t n, uint8_t b, uint8_t *a)
{
	uint8_t sum;
	int64_t i;

	if (!((n == 8 && b == 3 ) || (n == 12 && b == 2))) {
		LOG("Won't compute 'digits' for n=%" PRIu8 "and b=%" PRIu8
		    " (use n=8 b=3 or n=12 b=2)\n");
		goto digitstosumzero_error;
	}

	for (i = 1, sum = 0; i < n; i++, d /= (int64_t)b) {
		a[i] = (uint8_t)(d % (int64_t)b);
		sum += a[i];
	}
	a[0] = (b - (sum % b)) % b;

	return;

digitstosumzero_error:
	memset(a, UINT8_ERROR, n);
}
