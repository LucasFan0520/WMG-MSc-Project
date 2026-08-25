// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Record;

char *read_line() {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    int c;
    while ((c = fgetc(stdin)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            buf = realloc(buf, cap);
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

int main() {
    Record *arr = NULL;
    int count = 0;
    while (1) {
        char *line = read_line();
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "BATCH ", 6) == 0) {
            int n = atoi(line + 6);
            Record *batch_items = malloc(n * sizeof(Record));
            int valid = 1;
            for (int i = 0; i < n; i++) {
                char *bline = read_line();
                if (!bline) {
                    valid = 0;
                    break;
                }
                char *p = bline;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    batch_items[i].name = strdup(bline);
                    batch_items[i].note = strdup(p + 1);
                } else {
                    valid = 0;
                }
                free(bline);
            }
            if (valid) {
                arr = realloc(arr, (count + n) * sizeof(Record));
                for (int i = 0; i < n; i++) {
                    arr[count + i] = batch_items[i];
                }
                count += n;
            } else {
                for (int i = 0; i < n; i++) {
                    if (i < count) { 
                    }
                }
            }
            free(batch_items);
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    free(arr[i].note);
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    printf("%s %s\n", arr[i].name, arr[i].note);
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", arr[i].name, arr[i].note);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].note);
    }
    free(arr);
    return 0;
}
