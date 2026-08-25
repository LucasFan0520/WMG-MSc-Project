// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} ImportRec;

char *read_line(FILE *fp) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *nb = realloc(buf, size);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    if (len > 0 && buf[len - 1] == '\n') {
        len--;
    }
    buf[len] = '\0';
    return buf;
}

int main(void) {
    ImportRec *records = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "BATCH") == 0) {
            while (*p == ' ') p++;
            char *n_str = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int n = atoi(n_str);
            char **batch_lines = malloc(n * sizeof(char *));
            int valid = 1;
            for (int i = 0; i < n; i++) {
                batch_lines[i] = read_line(stdin);
                if (!batch_lines[i]) {
                    valid = 0;
                } else {
                    char *b = batch_lines[i];
                    while (*b == ' ') b++;
                    if (*b == '\0') valid = 0;
                    char *sp = strchr(b, ' ');
                    if (!sp) valid = 0;
                }
            }
            if (valid) {
                for (int i = 0; i < n; i++) {
                    char *b = batch_lines[i];
                    while (*b == ' ') b++;
                    char *name = b;
                    char *sp = strchr(b, ' ');
                    *sp = '\0';
                    char *note = sp + 1;
                    while (*note == ' ') note++;
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        ImportRec *nr = realloc(records, capacity * sizeof(ImportRec));
                        if (!nr) break;
                        records = nr;
                    }
                    records[count].name = strdup(name);
                    records[count].note = strdup(note);
                    count++;
                }
            }
            for (int i = 0; i < n; i++) {
                if (batch_lines[i]) free(batch_lines[i]);
            }
            free(batch_lines);
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            size_t i = 0;
            while (i < count) {
                if (strcmp(records[i].name, name) == 0) {
                    free(records[i].name);
                    free(records[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        records[j] = records[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(records[i].name, name) == 0) {
                    printf("%s\n", records[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", records[i].name, records[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(records[i].name);
        free(records[i].note);
    }
    free(records);
    return 0;
}
