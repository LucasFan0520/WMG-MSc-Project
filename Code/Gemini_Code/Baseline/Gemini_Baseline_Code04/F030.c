// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *old_val;
    char *new_val;
} RenameRule;

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

char *replace_substring(const char *src, const char *old, const char *new_s) {
    char *result;
    int i, cnt = 0;
    size_t newlen = strlen(new_s);
    size_t oldlen = strlen(old);
    for (i = 0; src[i] != '\0'; i++) {
        if (strstr(&src[i], old) == &src[i]) {
            cnt++;
            i += oldlen - 1;
        }
    }
    result = (char *)malloc(i + cnt * (newlen - oldlen) + 1);
    i = 0;
    while (*src) {
        if (strstr(src, old) == src) {
            strcpy(&result[i], new_s);
            i += newlen;
            src += oldlen;
        } else {
            result[i++] = *src++;
        }
    }
    result[i] = '\0';
    return result;
}

int main() {
    RenameRule *arr = NULL;
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
            char *old = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *new_val = p + 1;
                arr = realloc(arr, (count + 1) * sizeof(RenameRule));
                arr[count].old_val = strdup(old);
                arr[count].new_val = strdup(new_val);
                count++;
            }
        } else if (strncmp(line, "APPLY ", 6) == 0) {
            char *text = line + 6;
            int applied = 0;
            for (int i = 0; i < count; i++) {
                if (strstr(text, arr[i].old_val) != NULL) {
                    char *res = replace_substring(text, arr[i].old_val, arr[i].new_val);
                    printf("%s\n", res);
                    free(res);
                    applied = 1;
                    break;
                }
            }
            if (!applied) {
                printf("%s\n", text);
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *old = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].old_val, old) == 0) {
                    free(arr[i].old_val);
                    free(arr[i].new_val);
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "SWAP ", 5) == 0) {
            char *p = line + 5;
            char *i_s = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                int idx1 = atoi(i_s);
                int idx2 = atoi(p + 1);
                if (idx1 >= 0 && idx1 < count && idx2 >= 0 && idx2 < count) {
                    RenameRule temp = arr[idx1];
                    arr[idx1] = arr[idx2];
                    arr[idx2] = temp;
                }
            }
        } else if (strcmp(line, "RULES") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", arr[i].old_val, arr[i].new_val);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].old_val);
        free(arr[i].new_val);
    }
    free(arr);
    return 0;
}
