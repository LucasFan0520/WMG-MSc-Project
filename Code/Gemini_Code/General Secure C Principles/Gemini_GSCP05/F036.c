// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t capacity = 16;
    size_t len = 0;
    char *str = malloc(capacity);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
        if (len + 1 >= capacity) {
            capacity *= 2;
            char *new_str = realloc(str, capacity);
            if (!new_str) {
                free(str);
                return NULL;
            }
            str = new_str;
        }
        str[len++] = (char)c;
    }
    if (len == 0 && c == EOF) {
        free(str);
        return NULL;
    }
    if (len > 0 && str[len - 1] == '\n') {
        len--;
    }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *name;
    char *data;
} Record36;

int main(void) {
    Record36 *records = NULL;
    int count = 0;
    int capacity = 0;
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
                char *colon = strchr(p, ':');
                if (colon) {
                    *colon = '\0';
                    char *endptr;
                    long len_val = strtol(p, &endptr, 10);
                    if (endptr != p && len_val >= 0) {
                        char *data_start = colon + 1;
                        if ((long)strlen(data_start) >= len_val) {
                            if (count >= capacity) {
                                capacity = capacity == 0 ? 4 : capacity * 2;
                                Record36 *new_r = realloc(records, capacity * sizeof(Record36));
                                if (new_r) records = new_r;
                            }
                            if (count < capacity) {
                                records[count].name = strdup(name);
                                records[count].data = malloc(len_val + 1);
                                if (records[count].data) {
                                    memcpy(records[count].data, data_start, len_val);
                                    records[count].data[len_val] = '\0';
                                }
                                count++;
                            }
                        }
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            for (int i = 0; i < count; i++) {
                if (strcmp(records[i].name, name) == 0) {
                    free(records[i].name);
                    free(records[i].data);
                    for (int j = i; j < count - 1; j++) {
                        records[j] = records[j + 1];
                    }
                    count--;
                    break;
                }
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
