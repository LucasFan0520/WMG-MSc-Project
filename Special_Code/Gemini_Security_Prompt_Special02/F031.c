// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *name;
    char *note;
} ImporterRecord;

int main() {
    ImporterRecord *items = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        while (*p == ' ') p++;
        char *arg1 = p;
        while (*p && *p != ' ') p++;
        if (*p) *p = '\0';
        if (strcmp(cmd, "BATCH") == 0) {
            int n = atoi(arg1);
            int all_valid = 1;
            char **b_names = malloc(n * sizeof(char *));
            char **b_notes = malloc(n * sizeof(char *));
            for (int i = 0; i < n; i++) {
                char *b_line = read_line(stdin);
                if (!b_line) {
                    all_valid = 0;
                    b_names[i] = NULL;
                    b_notes[i] = NULL;
                    continue;
                }
                char *bp = b_line;
                while (*bp == ' ') bp++;
                char *b_name = bp;
                while (*bp && *bp != ' ') bp++;
                if (*bp) {
                    *bp = '\0';
                    bp++;
                }
                while (*bp == ' ') bp++;
                char *b_note = bp;
                if (strlen(b_name) == 0 || strlen(b_note) == 0) {
                    all_valid = 0;
                }
                b_names[i] = mystrdup(b_name);
                b_notes[i] = mystrdup(b_note);
                free(b_line);
            }
            if (all_valid) {
                for (int i = 0; i < n; i++) {
                    if (count >= cap) {
                        cap = cap == 0 ? 4 : cap * 2;
                        items = realloc(items, cap * sizeof(ImporterRecord));
                    }
                    items[count].name = b_names[i];
                    items[count].note = b_notes[i];
                    count++;
                }
            } else {
                for (int i = 0; i < n; i++) {
                    free(b_names[i]);
                    free(b_notes[i]);
                }
            }
            free(b_names);
            free(b_notes);
        } else if (strcmp(cmd, "DELETE") == 0) {
            size_t i = 0;
            while (i < count) {
                if (strcmp(items[i].name, arg1) == 0) {
                    free(items[i].name);
                    free(items[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, arg1) == 0) {
                    printf("%s %s\n", items[i].name, items[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", items[i].name, items[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].name);
        free(items[i].note);
    }
    free(items);
    return 0;
}
