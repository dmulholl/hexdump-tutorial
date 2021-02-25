#include "args.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>


char* helptext =
    "Usage: hexdump [file]\n"
    "\n"
    "Arguments:\n"
    "  [file]               File to read (default: STDIN).\n"
    "\n"
    "Options:\n"
    "  -l, --line <int>     Bytes per line in output (default: 16).\n"
    "  -n, --num <int>      Number of bytes to read (default: all).\n"
    "  -o, --offset <int>   Byte offset at which to begin reading.\n"
    "\n"
    "Flags:\n"
    "  -h, --help           Display this help text and exit.\n"
    "  -v, --version        Display the version number and exit.\n";


void print_line(uint8_t* buffer, int num_bytes, int offset, int line_length) {
    printf("%8X |", offset);

    for (int i = 0; i < line_length; i++) {
        if (i > 0 && i % 8 == 0) {
            printf(" ");
        }
        if (i < num_bytes) {
            printf(" %02X", buffer[i]);
        } else {
            printf("   ");
        }
    }

    printf(" | ");

    for (int i = 0; i < num_bytes; i++) {
        if (buffer[i] > 31 && buffer[i] < 127) {
            printf("%c", buffer[i]);
        } else {
            printf(".");
        }
    }

    printf("\n");
}


void dump_file(FILE* file, int offset, int bytes_to_read, int line_length) {
    uint8_t* buffer = (uint8_t*)malloc(line_length);
    if (buffer == NULL) {
        fprintf(stderr, "Error: insufficient memory.\n");
        exit(1);
    }

    while (true) {
        int max_bytes;

        if (bytes_to_read < 0) {
            max_bytes = line_length;
        } else if (line_length < bytes_to_read) {
            max_bytes = line_length;
        } else {
            max_bytes = bytes_to_read;
        }

        int num_bytes = fread(buffer, sizeof(uint8_t), max_bytes, file);
        if (num_bytes > 0) {
            print_line(buffer, num_bytes, offset, line_length);
            offset += num_bytes;
            bytes_to_read -= num_bytes;
        } else {
            break;
        }
    }

    free(buffer);
}


int main(int argc, char** argv) {
    // Instantiate a new ArgParser instance.
    ArgParser* parser = ap_new();
    ap_helptext(parser, helptext);
    ap_version(parser, "0.1.0");

    // Register our options with their default values.
    ap_int_opt(parser, "line l", 16);
    ap_int_opt(parser, "num n", -1);
    ap_int_opt(parser, "offset o", 0);

    // Parse the command line arguments.
    ap_parse(parser, argc, argv);

    // Default to reading from stdin.
    FILE* file = stdin;
    if (ap_has_args(parser)) {
        char* filename = ap_arg(parser, 0);
        file = fopen(filename, "rb");
        if (file == NULL) {
            fprintf(stderr, "Error: cannot open the file '%s'.\n", filename);
            exit(1);
        }
    }

    // Try seeking to the specified offset.
    int offset = ap_int_value(parser, "offset");
    if (offset != 0) {
        if (fseek(file, offset, SEEK_SET) != 0) {
            fprintf(stderr, "Error: cannot seek to the specified offset.\n");
            exit(1);
        }
    }

    int bytes_to_read = ap_int_value(parser, "num");
    int line_length = ap_int_value(parser, "line");
    dump_file(file, offset, bytes_to_read, line_length);

    fclose(file);
    ap_free(parser);
}
