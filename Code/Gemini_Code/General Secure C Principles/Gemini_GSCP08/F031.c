// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nb = realloc(buf, ncap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
            cap = ncap;
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

typedef struct {
    char *name;
    char *note;
} BatchRecord;

int main(void) {
    BatchRecord *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "REPORT") == 0) {
                for (size_t i = 0; i < count; i++) {
                    printf("%s %s\n", arr[i].name, arr[i].note);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "BATCH") == 0) {
            long n = strtol(args, NULL, 10);
            if (n > 0) {
                char **batch_lines = malloc(n * sizeof(char *));
                int valid = 1;
                for (long i = 0; i < n; i++) {
                    batch_lines[i] = read_line(stdin);
                    if (!batch_lines[i]) {
                        valid = 0;
                    }
                }
                if (valid) {
                    for (long i = 0; i < n; i++) {
                        if (strchr(batch_lines[i], ' ') == NULL) {
                            valid = 0;
                        }
                    }
                }
                if (valid) {
                    for (long i = 0; i < n; i++) {
                        char *line_c = batch_lines[i];
                        char *p = strchr(line_c, ' ');
                        *p = '\0';
                        char *name = line_c;
                        char *note = p + 1;
                        if (count >= cap) {
                            size_t ncap = cap == 0 ? 4 : cap * 2;
                            BatchRecord *narr = realloc(arr, ncap * sizeof(BatchRecord));
                            if (narr) {
                                arr = narr;
                                cap = ncap;
                            }
                        }
                        if (count < cap) {
                            arr[count].name = strdup(name);
                            arr[count].note = strdup(note);
                            count++;
                        }
                    }
                }
                for (long i = 0; i < n; i++) {
                    if (batch_lines[i]) free(batch_lines[i]);
                }
                free(batch_lines);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    free(arr[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    printf("%s\n", arr[i].note);
                    break;
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].note);
    }
    free(arr);
    return 0;
}
