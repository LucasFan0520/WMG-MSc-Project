// F029.c
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
    char *key;
    char *val;
} Rec;

int main() {
    Rec *recs = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "DEDUP") == 0) {
                int i = 0;
                while (i < count) {
                    int dup = 0;
                    for (int j = 0; j < i; j++) {
                        if (strcmp(recs[j].key, recs[i].key) == 0) {
                            dup = 1;
                            break;
                        }
                    }
                    if (dup) {
                        free(recs[i].key);
                        free(recs[i].val);
                        for (int j = i; j < count - 1; j++) {
                            recs[j] = recs[j + 1];
                        }
                        count--;
                    } else {
                        i++;
                    }
                }
                if (count == 0) { free(recs); recs = NULL; }
                else { recs = realloc(recs, sizeof(Rec) * count); }
            } else if (strcmp(cmd, "REPORT") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s %s\n", recs[i].key, recs[i].val);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "ADD") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                recs = realloc(recs, sizeof(Rec) * (count + 1));
                recs[count].key = strdup(args);
                recs[count].val = strdup(p2 + 1);
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            int i = 0;
            while (i < count) {
                if (strcmp(recs[i].key, args) == 0) {
                    free(recs[i].key);
                    free(recs[i].val);
                    for (int j = i; j < count - 1; j++) {
                        recs[j] = recs[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
            if (count == 0) { free(recs); recs = NULL; }
            else { recs = realloc(recs, sizeof(Rec) * count); }
        } else if (strcmp(cmd, "FIND") == 0) {
            for (int i = 0; i < count; i++) {
                if (strcmp(recs[i].key, args) == 0) {
                    printf("%s\n", recs[i].val);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(recs[i].key);
        free(recs[i].val);
    }
    free(recs);
    return 0;
}
