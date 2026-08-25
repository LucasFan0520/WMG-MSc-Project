// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    unsigned char *bytes;
    int size;
} Buffer;

char *read_line() {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    int c;
    while ((c = fgetc(stdin)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            buf = realloc(buf, cap);
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

int hex_to_bytes(const char *hex, unsigned char **res) {
    int len = strlen(hex);
    int count = len / 2;
    *res = malloc(count);
    for (int i = 0; i < count; i++) {
        unsigned int val;
        sscanf(hex + 2 * i, "%2x", &val);
        (*res)[i] = (unsigned char)val;
    }
    return count;
}

int main() {
    Buffer *arr = NULL;
    int count = 0;
    while (1) {
        char *line = read_line();
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "NEW ", 4) == 0) {
            char *p = line + 4;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *hex = p + 1;
                arr = realloc(arr, (count + 1) * sizeof(Buffer));
                arr[count].name = strdup(name);
                unsigned char *b = NULL;
                int sz = hex_to_bytes(hex, &b);
                arr[count].bytes = b;
                arr[count].size = sz;
                count++;
            }
        } else if (strncmp(line, "APPEND ", 7) == 0) {
            char *p = line + 7;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *hex = p + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name) == 0) {
                        unsigned char *b = NULL;
                        int sz = hex_to_bytes(hex, &b);
                        arr[i].bytes = realloc(arr[i].bytes, arr[i].size + sz);
                        memcpy(arr[i].bytes + arr[i].size, b, sz);
                        arr[i].size += sz;
                        free(b);
                        break;
                    }
                }
            }
        } else if (strncmp(line, "SLICE ", 6) == 0) {
            char *p = line + 6;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                char *offset_s = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    int offset = atoi(offset_s);
                    int length = atoi(p + 1);
                    for (int i = 0; i < count; i++) {
                        if (strcmp(arr[i].name, name) == 0) {
                            if (offset >= 0 && offset < arr[i].size && length > 0) {
                                int limit = offset + length;
                                if (limit > arr[i].size) limit = arr[i].size;
                                for (int j = offset; j < limit; j++) {
                                    printf("%02x", arr[i].bytes[j]);
                                }
                                printf("\n");
                            }
                            break;
                        }
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    free(arr[i].bytes);
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %d\n", arr[i].name, arr[i].size);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].bytes);
    }
    free(arr);
    return 0;
}
