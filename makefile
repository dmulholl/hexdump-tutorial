# ------------------------------------------------------------------------------
# Variables.
# ------------------------------------------------------------------------------

CFLAGS = -Wall -Wextra --std=c99 --pedantic -Wno-unused-parameter

# ------------------------------------------------------------------------------
# Targets.
# ------------------------------------------------------------------------------

default:
	@mkdir -p bin
	cc -o bin/hexdump src/hexdump.c src/args.c

withflags:
	@mkdir -p bin
	cc $(CFLAGS) -o bin/hexdump src/hexdump.c src/args.c
