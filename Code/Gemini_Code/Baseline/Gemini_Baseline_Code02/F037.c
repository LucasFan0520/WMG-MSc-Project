// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *str = malloc(size);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_str = realloc(str, size);
            if (!new_str) { free(str); return NULL; }
            str = new_str;
        }
        str[len++] = c;
    }
    if (len == 0 && c == EOF) { free(str); return NULL; }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *name;
    unsigned char *bytes;
    int size;
} Buffer;

int main() {
    Buffer *buffers = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "LIST") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s %d\n", buffers[i].name, buffers[i].size);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "NEW") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                char *name = args;
                char *hex = p2 + 1;
                int hex_len = strlen(hex);
                int b_size = hex_len / 2;
                unsigned char *b_data = malloc(b_size);
                for (int i = 0; i < b_size; i++) {
                    unsigned int val;
                    sscanf(hex + 2 * i, "%2x", &val);
                    b_data[i] = (unsigned char)val;
                }
                buffers = realloc(buffers, sizeof(Buffer) * (count + 1));
                buffers[count].name = strdup(name);
                buffers[count].bytes = b_data;
                buffers[count].size = b_size;
                count++;
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                char *name = args;
                char *hex = p2 + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(buffers[i].name, name) == 0) {
                        int hex_len = strlen(hex);
                        int append_size = hex_len / 2;
                        buffers[i].bytes = realloc(buffers[i].bytes, buffers[i].size + append_size);
                        for (int j = 0; j < append_size; j++) {
                            unsigned int val;
                            sscanf(hex + 2 * j, "%2x", &val);
                            buffers[i].bytes[buffers[i].size + j] = (unsigned char)val;
                        }
                        buffers[i].size += append_size;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "SLICE") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                char *name = args;
                char *offset_str = p2 + 1;
                char *p3 = strchr(offset_str, ' ');
                if (p3) {
                    *p3 = '\0';
                    int offset = atoi(offset_str);
                    int length = atoi(p3 + 1);
                    for (int i = 0; i < count; i++) {
                        if (strcmp(buffers[i].name, name) == 0) {
                            if (offset >= 0 && offset < buffers[i].size && length > 0) {
                                int end = offset + length;
                                if (end > buffers[i].size) end = buffers[i].size;
                                for (int j = offset; j < end; j++) {
                                    printf("%02x", buffers[i].bytes[j]);
                                }
                                printf("\n");
                            }
                            break;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            for (int i = 0; i < count; i++) {
                if (strcmp(buffers[i].name, args) == 0) {
                    free(buffers[i].name); free(buffers[i].bytes);
                    for (int j = i; j < count - 1; j++) {
                        buffers[j] = buffers[j + 1];
                    }
                    count--;
                    if (count == 0) { free(buffers); buffers = NULL; }
                    else { buffers = realloc(buffers, sizeof(Buffer) * count); }
                    break;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(buffers[i].name); free(buffers[i].bytes);
    }
    free(buffers);
    return 0;
}
