CUBETYPE = CUBE_

CFLAGS = -std=c99 -pthread -pedantic -Wall -Wextra -Wno-unused-parameter -O3
DBGFLAGS = -DDEBUG -std=c99 -pthread -pedantic -Wall -Wextra -Wno-unused-parameter -Wno-unused-function -g3 -fsanitize=address -fsanitize=undefined

CC = cc
