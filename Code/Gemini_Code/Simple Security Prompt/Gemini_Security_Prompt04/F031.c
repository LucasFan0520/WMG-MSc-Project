// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
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
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    if (len > 0 && buf[len-1] == '\r') {
        buf[len-1] = '\0';
    }
    return buf;
}

typedef struct {
    char *name;
    char *note;
} Record;

int main(void) {
    Record *records = NULL;
    int count = 0;
    int cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "BATCH ", 6) == 0) {
            long n = strtol(line + 6, NULL, 10);
            char **batch_lines = malloc(n * sizeof(char *));
            int valid = 1;
            for (int i = 0; i < n; i++) {
                batch_lines[i] = read_line(stdin);
                if (!batch_lines[i]) {
                    valid = 0;
                } else {
                    char *p = batch_lines[i];
                    while (*p == ' ') p++;
                    if (*p == '\0') valid = 0;
                    char *space = strchr(p, ' ');
                    if (!space) valid = 0;
                }
            }
            if (valid) {
                for (int i = 0; i < n; i++) {
                    char *p = batch_lines[i];
                    while (*p == ' ') p++;
                    char *name = p;
                    char *space = strchr(p, ' ');
                    *space = '\0';
                    char *note = space + 1;
                    while (*note == ' ') note++;
                    if (count >= cap) {
                        cap = cap == 0 ? 16 : cap * 2;
                        records = realloc(records, cap * sizeof(Record));
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
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            int i = 0;
            while (i < count) {
                if (strcmp(records[i].name, name) == 0) {
                    free(records[i].name);
                    free(records[i].note);
                    for (int j = i; j < count - 1; j++) {
                        records[j] = records[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            while (*name == ' ') name++;
            for (int i = 0; i < count; i++) {
                if (strcmp(records[i].name, name) == 0) {
                    printf("%s\n", records[i].note);
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", records[i].name, records[i].note);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(records[i].name);
        free(records[i].note);
    }
    free(records);
    return 0;
}
