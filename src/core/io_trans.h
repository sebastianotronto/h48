STATIC uint8_t readtrans(const char [static NISSY_SIZE_TRANSFORMATION]);
STATIC void writetrans(uint8_t, char [static NISSY_SIZE_TRANSFORMATION]);

STATIC uint8_t
readtrans(const char buf[static NISSY_SIZE_TRANSFORMATION])
{
	uint8_t t;

	for (t = 0; t < NTRANS; t++)
		if (!strncmp(buf, transstr[t], 11))
			return t;

	return UINT8_ERROR;
}

STATIC void
writetrans(uint8_t t, char buf[static NISSY_SIZE_TRANSFORMATION])
{
	if (t >= 48)
		memcpy(buf, "error trans", 11);
	else
		memcpy(buf, transstr[t], 11);
	buf[11] = '\0';
}
