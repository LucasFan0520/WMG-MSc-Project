// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *tag;
    char *message;
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

void print_underscores(const char *s) {
    while (*s) {
        if (*s == ' ') {
            putchar('_');
        } else {
            putchar(*s);
        }
        s++;
    }
    putchar('\n');
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
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            char *tag = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *msg = p + 1;
                arr = realloc(arr, (count + 1) * sizeof(Message));
                arr[count].tag = strdup(tag);
                arr[count].message = strdup(msg);
                count++;
            }
        } else if (strncmp(line, "CONCAT ", 7) == 0) {
            char *p = line + 7;
            char *tag1 = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                char *tag2 = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    char *newtag = p + 1;
                    char *m1 = NULL;
                    char *m2 = NULL;
                    for (int i = 0; i < count; i++) {
                        if (!m1 && strcmp(arr[i].tag, tag1) == 0) m1 = arr[i].message;
                        if (!m2 && strcmp(arr[i].tag, tag2) == 0) m2 = arr[i].message;
                    }
                    if (m1 && m2) {
                        size_t total_len = strlen(m1) + 1 + strlen(m2) + 1;
                        char *newmsg = malloc(total_len);
                        strcpy(newmsg, m1);
                        strcat(newmsg, " ");
                        strcat(newmsg, m2);
                        arr = realloc(arr, (count + 1) * sizeof(Message));
                        arr[count].tag = strdup(newtag);
                        arr[count].message = newmsg;
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *tag = line + 7;
            int i = 0;
            while (i < count) {
                if (strcmp(arr[i].tag, tag) == 0) {
                    free(arr[i].tag);
                    free(arr[i].message);
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *tag = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].tag, tag) == 0) {
                    print_underscores(arr[i].message);
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", arr[i].tag, arr[i].message);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].tag);
        free(arr[i].message);
    }
    free(arr);
    return 0;
}
