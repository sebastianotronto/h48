#include "eo.h"

coord_t *all_coordinates[] = {
	&coordinate_eo,
	NULL
};

STATIC void append_coord_name(const coord_t *, char *);

STATIC void
append_coord_name(const coord_t *coord, char *str)
{
	int i, j;

	i = 0;
	j = strlen(str);
	while (coord->name[i]) str[j++] = coord->name[i++];

	str[j] = '\0';
}
