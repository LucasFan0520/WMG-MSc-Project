// F037.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    unsigned char *bytes;
    int size;
} Buffer;

int main(void) {
    Buffer *buffers = NULL;
    int count = 0;
    int capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "NEW ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            char *hex = "";
            if (space) {
                *space = '\0';
                hex = space + 1;
                while (*hex == ' ') hex++;
            }
            if (count >= capacity) {
                int new_cap = capacity == 0 ? 4 : capacity * 2;
                Buffer *tmp = realloc(buffers, new_cap * sizeof(Buffer));
                if (!tmp) return 1;
                buffers = tmp;
                capacity = new_cap;
            }
            buffers[count].name = strdup(name);
            int hex_len = strlen(hex);
            int b_size = hex_len / 2;
            unsigned char *b_arr = malloc(b_size > 0 ? b_size : 1);
            if (!b_arr) return 1;
            int actual_size = 0;
            for (int i = 0; i < b_size; i++) {
                char tmp_h[3] = { hex[i*2], hex[i*2+1], '\0' };
                b_arr[actual_size++] = (unsigned char)strtol(tmp_h, NULL, 16);
            }
            buffers[count].bytes = b_arr;
            buffers[count].size = actual_size;
            count++;
        } else if (strncmp(line, "APPEND ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (!space) continue;
            *space = '\0';
            char *hex = space + 1;
            while (*hex == ' ') hex++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(buffers[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                Buffer *b = &buffers[found];
                int hex_len = strlen(hex);
                int add_size = hex_len / 2;
                if (add_size > 0) {
                    unsigned char *tmp = realloc(b->bytes, b->size + add_size);
                    if (!tmp) return 1;
                    b->bytes = tmp;
                    for (int i = 0; i < add_size; i++) {
                        char tmp_h[3] = { hex[i*2], hex[i*2+1], '\0' };
                        b->bytes[b->size++] = (unsigned char)strtol(tmp_h, NULL, 16);
                    }
                }
            }
        } else if (strncmp(line, "SLICE ", 6) == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *name = p;
            char *space1 = strchr(name, ' ');
            if (!space1) continue;
            *space1 = '\0';
            char *off_str = space1 + 1;
            while (*off_str == ' ') off_str++;
            char *space2 = strchr(off_str, ' ');
            if (!space2) continue;
            *space2 = '\0';
            char *len_str = space2 + 1;
            while (*len_str == ' ') len_str++;
            int offset = atoi(off_str);
            int slen = atoi(len_str);
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(buffers[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                Buffer *b = &buffers[found];
                if (offset >= 0 && offset < b->size && slen > 0) {
                    int end = offset + slen;
                    if (end > b->size) end = b->size;
                    for (int i = offset; i < end; i++) {
                        printf("%02x", b->bytes[i]);
                    }
                    putchar('\n');
                } else {
                    printf("INVALID\n");
                }
            } else {
                printf("NOT_FOUND\n");
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(buffers[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(buffers[found].name);
                free(buffers[found].bytes);
                for (int i = found; i < count - 1; i++) {
                    buffers[i] = buffers[i + 1];
                }
                count--;
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %d\n", buffers[i].name, buffers[i].size);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(buffers[i].name);
        free(buffers[i].bytes);
    }
    free(buffers);
    return 0;
}
