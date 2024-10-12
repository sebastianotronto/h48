uint64_t expected_cocsep[21] = {
	[0] = 1,
	[1] = 6,
	[2] = 63,
	[3] = 468,
	[4] = 3068,
	[5] = 15438,
	[6] = 53814,
	[7] = 71352,
	[8] = 8784,
	[9] = 96
};

uint64_t expected_h48[12][9][21] = {
	[0] = {
		[2] = {
			[0] = 5473562,
			[1] = 34776317,
			[2] = 68566704,
			[3] = 8750867,
		},
		[4] = {
			[0] = 1,
			[1] = 1,
			[2] = 4,
			[3] = 34,
			[4] = 331,
			[5] = 3612,
			[6] = 41605,
			[7] = 474128,
			[8] = 4953846,
			[9] = 34776317,
			[10] = 68566704,
			[11] = 8749194,
			[12] = 1673,
		},
	},
	[1] = {
		[2] = {
			[0] = 6012079,
			[1] = 45822302,
			[2] = 142018732,
			[3] = 41281787,
		},
	},
	[2] = {
		[2] = {
			[0] = 6391286,
			[1] = 55494785,
			[2] = 252389935,
			[3] = 155993794,
		},
	},
	[3] = {
		[2] = {
			[0] = 6686828,
			[1] = 63867852,
			[2] = 392789689,
			[3] = 477195231,
		},

	},
	[4] = {
		[2] = {
			[0] = 77147213,
			[1] = 543379415,
			[2] = 1139570251,
			[3] = 120982321,
		},
	},
	[5] = {
		[2] = {
			[0] = 82471284,
			[1] = 687850732,
			[2] = 2345840746,
			[3] = 645995638,
		},
	},
	[6] = {
		[2] = {
			[0] = 85941099,
			[1] = 804752968,
			[2] = 4077248182,
			[3] = 2556374551,
		},
	},
	[7] = {
		[2] = {
			[0] = 88529761,
			[1] = 897323475,
			[2] = 6126260791,
			[3] = 7936519573,
		},
	},
	[8] = {
		/* Unknown */
		[2] = {0},
	},
	[9] = {
		/* Unknown */
		[2] = {0},
	},
	[10] = {
		/* Unknown */
		[2] = {0},
	},
	[11] = {
		/* Unknown */
		[2] = {0},
	},
};

static bool
distribution_equal(const uint64_t *expected, const uint64_t *actual, int n)
{
	bool equal;
	int i;

	for (i = 0, equal = true; i <= n; i++) {
		if (expected[i] != actual[i]) {
			equal = false;
			printf("Wrong value for %d: expected %" PRIu64
			    ", actual %" PRIu64 "\n",
			    i, expected[i], actual[i]);
		}
	}

	return equal;
}

static bool
check_cocsep(uint64_t data_size, const void *data)
{
	tableinfo_t info;

	readtableinfo(data_size, data, &info);
	return distribution_equal(
	    expected_cocsep, info.distribution, info.maxvalue);
}

static bool
unknown_h48(uint8_t h, uint8_t k)
{
	if (k != 2 && k != 4)
		return true;

	if (k == 4 && h != 0)
		return true;

	return k == 2 && h > 7;
}

STATIC bool
check_distribution(const char *solver, uint64_t data_size, const void *data)
{
	tableinfo_t info = {0};

	if (!strncmp(solver, "h48", 3)) {
		readtableinfo(data_size, data, &info);
		if (!distribution_equal(
		    expected_cocsep, info.distribution, info.maxvalue)) {
			printf("ERROR! cocsep distribution is incorrect\n");
			return false;
		}
		printf("cocsep distribution is correct\n");

		readtableinfo_n(data_size, data, 2, &info);
		if (unknown_h48(info.h48h, info.bits))
			goto check_distribution_unknown;

		if (!distribution_equal(expected_h48[info.h48h][info.bits],
		    info.distribution, info.maxvalue)) {
			printf("ERROR! h48 distribution is incorrect\n");
			return false;
		}

		printf("h48 distribution is correct\n");
		return true;
	}

check_distribution_unknown:
	printf("Distribution unknown, not checked\n");
	return true;
}
