// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 1024;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) { free(buf); return NULL; }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *name;
    char *encoded;
} RLEString;

int main() {
    RLEString *strings = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char cmd[64];
        int n = 0;
        if (sscanf(line, "%63s%n", cmd, &n) < 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", strings[i].name);
            }
            free(line);
            continue;
        }
        char *p = line + n;
        while (*p == ' ' || *p == '\t') p++;
        if (strcmp(cmd, "STORE") == 0) {
            char name[256], enc[1024];
            if (sscanf(p, "%255s %1023s", name, enc) == 2) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    strings = realloc(strings, capacity * sizeof(RLEString));
                }
                strings[count].name = strdup(name);
                strings[count].encoded = strdup(enc);
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(p, "%255s", name) == 1) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(strings[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(strings[found].name);
                    free(strings[found].encoded);
                    for (int i = found; i < count - 1; i++) {
                        strings[i] = strings[i + 1];
                    }
                    count--;
                }
            }
        } else if (strcmp(cmd, "DECODE") == 0) {
            char name[256];
            if (sscanf(p, "%255s", name) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(strings[i].name, name) == 0) {
                        char *s = strings[i].encoded;
                        int valid = 1;
                        while (*s) {
                            int cnt = 0;
                            int n2 = 0;
                            if (sscanf(s, "%d%n", &cnt, &n2) == 1 && cnt > 0 && cnt < 1000000) {
                                s += n2;
                                char c = *s;
                                if (c) {
                                    for (int k = 0; k < cnt; k++) {
                                        if (c == ' ') printf("_");
                                        else printf("%c", c);
                                    }
                                    s++;
                                } else {
                                    valid = 0;
                                    break;
                                }
                            } else {
                                valid = 0;
                                break;
                            }
                        }
                        if (valid) printf("\n");
                        break;
                    }
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(strings[i].name);
        free(strings[i].encoded);
    }
    free(strings);
    return 0;
}
