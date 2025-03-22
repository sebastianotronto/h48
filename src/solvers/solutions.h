#define MAXLEN 20

STATIC bool appendchar(size_t n, char [n], size_t *, char);

STATIC bool
appendchar(size_t n, char s[n], size_t *used, char c)
{
	if (n <= *used)
		return false;

	s[(*used)++] = c;

	return true;
}
