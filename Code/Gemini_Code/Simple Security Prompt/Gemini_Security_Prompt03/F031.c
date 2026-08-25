// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Record31;

char *read_line(FILE *fp) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
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

int main(void) {
    Record31 *list = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "BATCH") == 0) {
            while (*p == ' ') p++;
            int n = atoi(p);
            Record31 *batch_items = malloc(n * sizeof(Record31));
            int batch_valid = 1;
            for (int i = 0; i < n; i++) {
                char *bline = read_line(stdin);
                if (!bline) {
                    batch_valid = 0;
                    batch_items[i].name = NULL;
                    batch_items[i].note = NULL;
                    continue;
                }
                char *bp = bline;
                while (*bp == ' ') bp++;
                char *name = bp;
                while (*bp && *bp != ' ') bp++;
                if (*bp) { *bp = '\0'; bp++; }
                while (*bp == ' ') bp++;
                char *note = bp;
                if (strlen(name) == 0 || strlen(note) == 0) {
                    batch_valid = 0;
                    batch_items[i].name = NULL;
                    batch_items[i].note = NULL;
                } else {
                    batch_items[i].name = strdup(name);
                    batch_items[i].note = strdup(note);
                }
                free(bline);
            }
            if (batch_valid) {
                list = realloc(list, (count + n) * sizeof(Record31));
                for (int i = 0; i < n; i++) {
                    list[count++] = batch_items[i];
                }
            } else {
                for (int i = 0; i < n; i++) {
                    if (batch_items[i].name) free(batch_items[i].name);
                    if (batch_items[i].note) free(batch_items[i].note);
                }
            }
            free(batch_items);
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (list[i].name && strcmp(list[i].name, name) == 0) {
                    free(list[i].name);
                    free(list[i].note);
                    list[i].name = NULL;
                    list[i].note = NULL;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (list[i].name && strcmp(list[i].name, name) == 0) {
                    printf("%s\n", list[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                if (list[i].name) {
                    printf("%s %s\n", list[i].name, list[i].note);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        if (list[i].name) {
            free(list[i].name);
            free(list[i].note);
        }
    }
    free(list);
    return 0;
}
