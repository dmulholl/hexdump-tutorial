hexdump:
	@mkdir -p bin
	cc -o bin/hexdump src/hexdump.c src/args.c
