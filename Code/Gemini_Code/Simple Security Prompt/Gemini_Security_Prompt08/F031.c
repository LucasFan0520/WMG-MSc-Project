// F031.c
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
    char *name;
    char *note;
} Record;

int main(void) {
    Record *list = NULL;
    size_t count = 0;
    size_t cap = 0;
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
        if (strcmp(cmd, "BATCH") == 0) {
            int n = atoi(arg1);
            Record *batch_items = malloc(n * sizeof(Record));
            int valid = 1;
            for (int i = 0; i < n; i++) {
                char *b_line = read_line(stdin);
                if (!b_line) {
                    valid = 0;
                    break;
                }
                char *bp = b_line;
                while (*bp == ' ') bp++;
                if (*bp == '\0') {
                    valid = 0;
                    free(b_line);
                    continue;
                }
                char *name = bp;
                while (*bp && *bp != ' ') bp++;
                if (*bp) { *bp = '\0'; bp++; }
                while (*bp == ' ') bp++;
                char *note = bp;
                if (strlen(name) == 0 || strlen(note) == 0) {
                    valid = 0;
                }
                if (valid) {
                    batch_items[i].name = strdup(name);
                    batch_items[i].note = strdup(note);
                }
                free(b_line);
            }
            if (valid) {
                for (int i = 0; i < n; i++) {
                    if (count >= cap) {
                        cap = cap == 0 ? 4 : cap * 2;
                        Record *nb = realloc(list, cap * sizeof(Record));
                        if (nb) list = nb;
                    }
                    if (count < cap) {
                        list[count++] = batch_items[i];
                    } else {
                        free(batch_items[i].name);
                        free(batch_items[i].note);
                    }
                }
            } else {
                for (int i = 0; i < n; i++) {
                    if (i < (int)count) { 
                    }
                }
            }
            free(batch_items);
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, arg1) == 0) {
                    free(list[i].name);
                    free(list[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].name, arg1) == 0) {
                    printf("%s: %s\n", list[i].name, list[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s: %s\n", list[i].name, list[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].name);
        free(list[i].note);
    }
    free(list);
    return 0;
}
