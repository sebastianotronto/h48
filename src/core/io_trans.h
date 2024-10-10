STATIC uint8_t readtrans(const char *);
STATIC void writetrans(uint8_t, char *);

STATIC uint8_t
readtrans(const char *buf)
{
	uint8_t t;

	for (t = 0; t < 48; t++)
		if (!strncmp(buf, transstr[t], 11))
			return t;

	return UINT8_ERROR;
}

STATIC void
writetrans(uint8_t t, char *buf)
{
	if (t >= 48)
		memcpy(buf, "error trans", 11);
	else
		memcpy(buf, transstr[t], 11);
	buf[11] = '\0';
}
