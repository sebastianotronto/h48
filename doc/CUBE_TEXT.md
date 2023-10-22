# Textual representation of the cube

The functions readcube() and writecube() use different formats to read
and write a cube to text. Not all formats are supported for both input
and output.

## H48 - standard format for h48 (read, write)

Each edge is represented by two letters denoting the sides it belongs to
and one number denoting its orientation (0 oriented, 1 mis-oriented).
Similarly, each corner is represented by three letters and a number
(0 oriented, 1 twisted clockwise, 2 twisted counter-clockwise).
Edge orientation is relative to the F / B axis, corner orientation is
relative to the U / D axis.

The pieces are ordered such that the solved cube looks like this:

UF0 UB0 DB0 DF0 UR0 UL0 DL0 DR0 FR0 FL0 BL0 BR0
UFR0 UBL0 DFL0 DBR0 UFL0 UBR0 DFR0 DBL0

Whitespace (including newlines) between pieces is ignored when reading
the cube, and a single whitespace character is added between pieces
when writing.

The cube after the moves R'U'F looks like this:

FL1 BR0 DB0 UR1 UF0 UB0 DL0 FR0 UL1 DF1 BL0 DR0
UBL1 DBR1 UFR2 DFR2 DFL2 UBL2 UFL2 DBL0

## SRC - representation of the object in C code for cube_array (write)

The exact format depends on the internal cube representation. It is
guaranteed that, if OUT is the output in this format, the line

cube_t cube = OUT;

is interpreted correctly by h48.