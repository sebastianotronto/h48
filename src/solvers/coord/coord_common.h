coord_t *all_coordinates[] = {
	&coordinate_eo,
	NULL
};

STATIC void append_coord_name(const coord_t *, char *);
STATIC coord_t *parse_coord(const char *, int);
STATIC uint8_t parse_axis(const char *, int);

STATIC void
append_coord_name(const coord_t *coord, char *str)
{
	int i, j;

	i = 0;
	j = strlen(str);
	while (coord->name[i]) str[j++] = coord->name[i++];

	str[j] = '\0';
}

STATIC coord_t *
parse_coord(const char *coord, int n)
{
	int i;

	for (i = 0; all_coordinates[i] != NULL; i++)
		if (!strncmp(all_coordinates[i]->name, coord, n))
			return all_coordinates[i];

	return NULL;
}

STATIC uint8_t
parse_axis(const char *axis, int n)
{
	if (!strncmp(axis, "UD", n) || !strncmp(axis, "DU", n)) {
		return AXIS_UD;
	} else if (!strncmp(axis, "RL", n) || !strncmp(axis, "LR", n)) {
		return AXIS_RL;
	} else if (!strncmp(axis, "FB", n) || !strncmp(axis, "BF", n)) {
		return AXIS_FB;
	}

	return UINT8_ERROR;
}
