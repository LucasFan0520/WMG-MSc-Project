// F038.c
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
    char *encoded;
} RLE;

int main() {
    RLE *rles = NULL;
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
                    printf("%s\n", rles[i].name);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "STORE") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                rles = realloc(rles, sizeof(RLE) * (count + 1));
                rles[count].name = strdup(args);
                rles[count].encoded = strdup(p2 + 1);
                count++;
            }
        } else if (strcmp(cmd, "DECODE") == 0) {
            for (int i = 0; i < count; i++) {
                if (strcmp(rles[i].name, args) == 0) {
                    char *enc = rles[i].encoded;
                    int idx = 0;
                    int valid = 1;
                    while (enc[idx] != '\0') {
                        long repeat = 0;
                        while (enc[idx] >= '0' && enc[idx] <= '9') {
                            repeat = repeat * 10 + (enc[idx] - '0');
                            if (repeat > 100000) { valid = 0; break; }
                            idx++;
                        }
                        if (!valid || enc[idx] == '\0') break;
                        idx++;
                    }
                    if (valid) {
                        idx = 0;
                        while (enc[idx] != '\0') {
                            long repeat = 0;
                            while (enc[idx] >= '0' && enc[idx] <= '9') {
                                repeat = repeat * 10 + (enc[idx] - '0');
                                idx++;
                            }
                            char ch = enc[idx];
                            if (ch == ' ') ch = '_';
                            for (long j = 0; j < repeat; j++) {
                                printf("%c", ch);
                            }
                            idx++;
                        }
                        printf("\n");
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            for (int i = 0; i < count; i++) {
                if (strcmp(rles[i].name, args) == 0) {
                    free(rles[i].name); free(rles[i].encoded);
                    for (int j = i; j < count - 1; j++) {
                        rles[j] = rles[j + 1];
                    }
                    count--;
                    if (count == 0) { free(rles); rles = NULL; }
                    else { rles = realloc(rles, sizeof(RLE) * count); }
                    break;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(rles[i].name); free(rles[i].encoded);
    }
    free(rles);
    return 0;
}
