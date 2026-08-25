// F036.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *data;
} Record;

int main(void) {
    Record *records = NULL;
    int count = 0;
    int capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (!space) continue;
            *space = '\0';
            char *prefix = space + 1;
            while (*prefix == ' ') prefix++;
            char *colon = strchr(prefix, ':');
            if (!colon) continue;
            *colon = '\0';
            char *endptr;
            long d_len = strtol(prefix, &endptr, 10);
            if (prefix == endptr || d_len < 0) continue;
            char *data_part = colon + 1;
            if ((long)strlen(data_part) < d_len) continue;
            
            if (count >= capacity) {
                int new_cap = capacity == 0 ? 4 : capacity * 2;
                Record *tmp = realloc(records, new_cap * sizeof(Record));
                if (!tmp) return 1;
                records = tmp;
                capacity = new_cap;
            }
            records[count].name = strdup(name);
            char *d_str = malloc(d_len + 1);
            if (!d_str) return 1;
            strncpy(d_str, data_part, d_len);
            d_str[d_len] = '\0';
            records[count].data = d_str;
            count++;
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
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(records[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                char *d = records[found].data;
                while (*d) {
                    if (*d == ' ') putchar('_');
                    else putchar(*d);
                    d++;
                }
                putchar('\n');
            } else {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", records[i].name);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(records[i].name);
        free(records[i].data);
    }
    free(records);
    return 0;
}
