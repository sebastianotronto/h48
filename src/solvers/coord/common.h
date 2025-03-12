coord_t *all_coordinates[] = {
	&coordinate_eo,
	NULL
};

STATIC void append_coord_name(const coord_t *, char *);
STATIC coord_t *parse_coord(const char *, int);
STATIC uint8_t parse_axis(const char *, int);
STATIC void parse_coord_and_axis(const char *, int, coord_t **, uint8_t *);

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

STATIC void
parse_coord_and_axis(const char *str, int n, coord_t **coord, uint8_t *axis)
{
	int i;

	for (i = 0; i < n; i++)
		if (str[i] == '_')
			break;

	if (coord != NULL)
		*coord = parse_coord(str, i);

	if (axis != NULL)
		*axis = i == n ? UINT8_ERROR : parse_axis(str+i+1, n-i-1);
}
