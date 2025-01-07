#include <stdio.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024  // Adjust to the longest possible line length

void print_line_with_limit(const char *line, int limit) {
    int len = strlen(line);

    if (len > limit) {
        // Print the first 'limit' characters followed by [...]
        for (int i = 0; i < limit; i++) {
            putchar(line[i]);
        }
        printf("[...]\n");
    } else {
        // Print the full line if within the limit
        printf("%s", line);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <filename> <char_limit>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    int char_limit = atoi(argv[2]);  // Get the character limit from arguments
    if (char_limit <= 0) {
        fprintf(stderr, "Error: Invalid character limit.\n");
        return 1;
    }

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        print_line_with_limit(line, char_limit);
    }

    fclose(file);
    return 0;
}

