// F023.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *sender;
    char *subject;
    char *body;
    char **tags;
    int tag_count;
} Message;

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
    Message *arr = NULL;
    int count = 0;
    while (1) {
        char *line = read_line();
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "MAIL ", 5) == 0) {
            char *p = line + 5;
            char *sender = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                char *subject = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    char *body = p + 1;
                    arr = realloc(arr, (count + 1) * sizeof(Message));
                    arr[count].sender = strdup(sender);
                    arr[count].subject = strdup(subject);
                    arr[count].body = strdup(body);
                    arr[count].tags = NULL;
                    arr[count].tag_count = 0;
                    count++;
                }
            }
        } else if (strncmp(line, "TAG ", 4) == 0) {
            char *p = line + 4;
            char *subject = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *tag = p + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].subject, subject) == 0) {
                        int tc = arr[i].tag_count;
                        arr[i].tags = realloc(arr[i].tags, (tc + 1) * sizeof(char *));
                        arr[i].tags[tc] = strdup(tag);
                        arr[i].tag_count++;
                        break;
                    }
                }
            }
        } else if (strncmp(line, "UNTAG ", 6) == 0) {
            char *p = line + 6;
            char *subject = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *tag = p + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].subject, subject) == 0) {
                        for (int j = 0; j < arr[i].tag_count; j++) {
                            if (strcmp(arr[i].tags[j], tag) == 0) {
                                free(arr[i].tags[j]);
                                for (int k = j; k < arr[i].tag_count - 1; k++) {
                                    arr[i].tags[k] = arr[i].tags[k + 1];
                                }
                                arr[i].tag_count--;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *subject = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].subject, subject) == 0) {
                    free(arr[i].sender);
                    free(arr[i].subject);
                    free(arr[i].body);
                    for (int j = 0; j < arr[i].tag_count; j++) {
                        free(arr[i].tags[j]);
                    }
                    free(arr[i].tags);
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "SEARCH ", 7) == 0) {
            char *tag = line + 7;
            for (int i = 0; i < count; i++) {
                for (int j = 0; j < arr[i].tag_count; j++) {
                    if (strcmp(arr[i].tags[j], tag) == 0) {
                        printf("%s\n", arr[i].subject);
                        break;
                    }
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s %s\n", arr[i].sender, arr[i].subject, arr[i].body);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].sender);
        free(arr[i].subject);
        free(arr[i].body);
        for (int j = 0; j < arr[i].tag_count; j++) {
            free(arr[i].tags[j]);
        }
        free(arr[i].tags);
    }
    free(arr);
    return 0;
}
