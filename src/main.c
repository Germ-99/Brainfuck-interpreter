#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_SIZE 30000

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <brainfuck_file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open file %s\n", argv[1]);
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char *program = malloc(file_size + 1);
    if (!program) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(file);
        return 1;
    }

    size_t prog_size = fread(program, 1, file_size, file);
    program[prog_size] = '\0';
    fclose(file);

    int *bracket_pairs = malloc(prog_size * sizeof(int));
    if (!bracket_pairs) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        free(program);
        return 1;
    }
    memset(bracket_pairs, -1, prog_size * sizeof(int));

    int *bracket_stack = malloc(prog_size * sizeof(int));
    if (!bracket_stack) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        free(program);
        free(bracket_pairs);
        return 1;
    }
    int stack_size = 0;

    for (size_t i = 0; i < prog_size; i++) {
        if (program[i] == '[') {
            bracket_stack[stack_size++] = i;
        } else if (program[i] == ']') {
            if (stack_size <= 0) {
                fprintf(stderr, "Error: Unmatched closing bracket at position %zu\n", i);
                free(program);
                free(bracket_pairs);
                free(bracket_stack);
                return 1;
            }
            int start = bracket_stack[--stack_size];
            bracket_pairs[start] = i;
            bracket_pairs[i] = start;
        }
    }

    if (stack_size > 0) {
        fprintf(stderr, "Error: Unmatched opening bracket at position %d\n", bracket_stack[--stack_size]);
        free(program);
        free(bracket_pairs);
        free(bracket_stack);
        return 1;
    }
    free(bracket_stack);

    unsigned char memory[MEMORY_SIZE] = {0};
    int pointer = 0;

    for (size_t i = 0; i < prog_size; i++) {
        switch (program[i]) {
            case '>':
                pointer = (pointer + 1) % MEMORY_SIZE;
                break;
            case '<':
                pointer = (pointer + MEMORY_SIZE - 1) % MEMORY_SIZE;
                break;
            case '+':
                memory[pointer]++;
                break;
            case '-':
                memory[pointer]--;
                break;
            case '.':
                putchar(memory[pointer]);
                fflush(stdout);
                break;
            case ',':
                {
                    int input = getchar();
                    memory[pointer] = (input != EOF) ? (unsigned char)input : 0;
                }
                break;
            case '[':
                if (memory[pointer] == 0) {
                    i = bracket_pairs[i];
                }
                break;
            case ']':
                if (memory[pointer] != 0) {
                    i = bracket_pairs[i];
                }
                break;
        }
    }

    free(program);
    free(bracket_pairs);
    return 0;
}
