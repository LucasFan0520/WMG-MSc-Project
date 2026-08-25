// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) { free(buf); return NULL; }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *title;
    char **body;
    size_t body_count;
} Record;

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    FILE *f = fopen(argv[1], "r");
    if (!f) return 1;
    Record *list = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *title = read_line(f);
        if (!title) break;
        Record r;
        r.title = title;
        r.body = NULL;
        r.body_count = 0;
        size_t b_cap = 0;
        while (1) {
            char *b_line = read_line(f);
            if (!b_line) break;
            if (strcmp(b_line, "--") == 0) {
                free(b_line);
                break;
            }
            if (r.body_count >= b_cap) {
                b_cap = b_cap == 0 ? 4 : b_cap * 2;
                char **nb = realloc(r.body, b_cap * sizeof(char *));
                if (nb) r.body = nb;
            }
            r.body[r.body_count++] = b_line;
        }
        if (count >= cap) {
            cap = cap == 0 ? 4 : cap * 2;
            Record *nb = realloc(list, cap * sizeof(Record));
            if (nb) list = nb;
        }
        if (count < cap) {
            list[count++] = r;
        }
    }
    fclose(f);
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') { free(line); continue; }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) { free(line); break; }
        while (*p == ' ') p++;
        char *arg1 = p;
        if (strcmp(cmd, "FIND") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].title, arg1) == 0) {
                    printf("%s\n", list[i].title);
                    for (size_t j = 0; j < list[i].body_count; j++) {
                        printf("%s\n", list[i].body[j]);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].title, arg1) == 0) {
                    free(list[i].title);
                    for (size_t j = 0; j < list[i].body_count; j++) {
                        free(list[i].body[j]);
                    }
                    free(list[i].body);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            Record r;
            r.title = strdup(arg1);
            r.body = NULL;
            r.body_count = 0;
            size_t b_cap = 0;
            while (1) {
                char *b_line = read_line(stdin);
                if (!b_line) break;
                if (strcmp(b_line, "--") == 0) {
                    free(b_line);
                    break;
                }
                if (r.body_count >= b_cap) {
                    b_cap = b_cap == 0 ? 4 : b_cap * 2;
                    char **nb = realloc(r.body, b_cap * sizeof(char *));
                    if (nb) r.body = nb;
                }
                r.body[r.body_count++] = b_line;
            }
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                Record *nb = realloc(list, cap * sizeof(Record));
                if (nb) list = nb;
            }
            if (count < cap) {
                list[count++] = r;
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", list[i].title);
                for (size_t j = 0; j < list[i].body_count; j++) {
                    printf("%s\n", list[i].body[j]);
                }
                if (i < count - 1) {
                    printf("--\n");
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].title);
        for (size_t j = 0; j < list[i].body_count; j++) {
            free(list[i].body[j]);
        }
        free(list[i].body);
    }
    free(list);
    return 0;
}
