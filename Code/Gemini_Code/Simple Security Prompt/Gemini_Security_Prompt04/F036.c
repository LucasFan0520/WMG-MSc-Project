// F036.c
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
    char *data;
} PrefixedRecord;

int main(void) {
    PrefixedRecord *records = NULL;
    int count = 0;
    int cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                char *endptr;
                long len = strtol(p, &endptr, 10);
                if (endptr != p && *endptr == ':' && len >= 0) {
                    char *data_start = endptr + 1;
                    if ((long)strlen(data_start) >= len) {
                        if (count >= cap) {
                            cap = cap == 0 ? 16 : cap * 2;
                            records = realloc(records, cap * sizeof(PrefixedRecord));
                        }
                        records[count].name = strdup(name);
                        records[count].data = malloc(len + 1);
                        strncpy(records[count].data, data_start, len);
                        records[count].data[len] = '\0';
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(records[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(records[found].name);
                free(records[found].data);
                for (int i = found; i < count - 1; i++) {
                    records[i] = records[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "SHOW ", 5) == 0) {
            char *name = line + 5;
            while (*name == ' ') name++;
            for (int i = 0; i < count; i++) {
                if (strcmp(records[i].name, name) == 0) {
                    char *d = records[i].data;
                    while (*d) {
                        if (*d == ' ') putchar('_');
                        else putchar(*d);
                        d++;
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", records[i].name);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(records[i].name);
        free(records[i].data);
    }
    free(records);
    return 0;
}
