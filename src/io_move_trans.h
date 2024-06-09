_static uint8_t readmove(char);
_static uint8_t readmodifier(char);
_static uint8_t readtrans(const char *);
_static int writemoves(uint8_t *, int, char *);
_static void writetrans(uint8_t, char *);

_static uint8_t
readmove(char c)
{
	switch (c) {
	case 'U':
		return _move_U;
	case 'D':
		return _move_D;
	case 'R':
		return _move_R;
	case 'L':
		return _move_L;
	case 'F':
		return _move_F;
	case 'B':
		return _move_B;
	default:
		return _error;
	}
}

_static uint8_t
readmodifier(char c)
{
	switch (c) {
	case '1': /* Fallthrough */
	case '2': /* Fallthrough */
	case '3':
		return c - '0' - 1;
	case '\'':
		return 2;
	default:
		return 0;
	}
}

_static uint8_t
readtrans(const char *buf)
{
	uint8_t t;

	for (t = 0; t < 48; t++)
		if (!strncmp(buf, transstr[t], 11))
			return t;

	DBG_LOG("readtrans error\n");
	return _error;
}

_static int
writemoves(uint8_t *m, int n, char *buf)
{
	int i;
	size_t len;
	const char *s;
	char *b;

	for (i = 0, b = buf; i < n; i++, b++) {
		s = movestr[m[i]];
		len = strlen(s);
		memcpy(b, s, len);
		b += len;	
		*b = ' ';
	}

	if (b != buf)
		b--; /* Remove last space */
	*b = '\0';

	return b - buf;
}

_static void
writetrans(uint8_t t, char *buf)
{
	if (t >= 48)
		memcpy(buf, "error trans", 11);
	else
		memcpy(buf, transstr[t], 11);
	buf[11] = '\0';
}
