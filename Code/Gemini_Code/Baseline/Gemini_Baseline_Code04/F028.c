// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *stop;
    char *note;
} Stop;

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
    Stop *arr = NULL;
    int count = 0;
    while (1) {
        char *line = read_line();
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            char *stop = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *note = p + 1;
                arr = realloc(arr, (count + 1) * sizeof(Stop));
                arr[count].stop = strdup(stop);
                arr[count].note = strdup(note);
                count++;
            }
        } else if (strncmp(line, "INSERT ", 7) == 0) {
            char *p = line + 7;
            char *idx_s = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                char *stop = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    char *note = p + 1;
                    int idx = atoi(idx_s);
                    if (idx >= 0 && idx <= count) {
                        arr = realloc(arr, (count + 1) * sizeof(Stop));
                        for (int i = count; i > idx; i--) {
                            arr[i] = arr[i - 1];
                        }
                        arr[idx].stop = strdup(stop);
                        arr[idx].note = strdup(note);
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "MOVE ", 5) == 0) {
            char *p = line + 5;
            char *from_s = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *to_s = p + 1;
                int fromIdx = atoi(from_s);
                int toIdx = atoi(to_s);
                if (fromIdx >= 0 && fromIdx < count && toIdx >= 0 && toIdx < count) {
                    Stop temp = arr[fromIdx];
                    if (fromIdx < toIdx) {
                        for (int i = fromIdx; i < toIdx; i++) {
                            arr[i] = arr[i + 1];
                        }
                    } else {
                        for (int i = fromIdx; i > toIdx; i--) {
                            arr[i] = arr[i - 1];
                        }
                    }
                    arr[toIdx] = temp;
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            int idx = atoi(line + 7);
            if (idx >= 0 && idx < count) {
                free(arr[idx].stop);
                free(arr[idx].note);
                for (int i = idx; i < count - 1; i++) {
                    arr[i] = arr[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *stop = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].stop, stop) == 0) {
                    printf("%s\n", arr[i].note);
                    break;
                }
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", arr[i].stop, arr[i].note);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].stop);
        free(arr[i].note);
    }
    free(arr);
    return 0;
}
