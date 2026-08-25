// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *tag;
    char *msg;
} Record;

char *read_line(FILE *fp) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *nb = realloc(buf, size);
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
    if (len > 0 && buf[len - 1] == '\n') {
        len--;
    }
    buf[len] = '\0';
    return buf;
}

int main(void) {
    Record *list = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ') p++;
            char *tag = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *msg = p;
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Record *nl = realloc(list, capacity * sizeof(Record));
                if (!nl) break;
                list = nl;
            }
            list[count].tag = strdup(tag);
            list[count].msg = strdup(msg);
            count++;
        } else if (strcmp(cmd, "CONCAT") == 0) {
            while (*p == ' ') p++;
            char *tag1 = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *tag2 = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *newtag = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            char *m1 = NULL;
            char *m2 = NULL;
            for (size_t i = 0; i < count; i++) {
                if (!m1 && strcmp(list[i].tag, tag1) == 0) m1 = list[i].msg;
                if (!m2 && strcmp(list[i].tag, tag2) == 0) m2 = list[i].msg;
            }
            if (m1 && m2) {
                size_t len1 = strlen(m1);
                size_t len2 = strlen(m2);
                char *nm = malloc(len1 + len2 + 2);
                if (nm) {
                    strcpy(nm, m1);
                    strcat(nm, " ");
                    strcat(nm, m2);
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Record *nl = realloc(list, capacity * sizeof(Record));
                        if (!nl) {
                            free(nm);
                            break;
                        }
                        list = nl;
                    }
                    list[count].tag = strdup(newtag);
                    list[count].msg = nm;
                    count++;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ') p++;
            char *tag = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            size_t i = 0;
            while (i < count) {
                if (strcmp(list[i].tag, tag) == 0) {
                    free(list[i].tag);
                    free(list[i].msg);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ') p++;
            char *tag = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].tag, tag) == 0) {
                    for (size_t j = 0; list[i].msg[j]; j++) {
                        if (list[i].msg[j] == ' ') putchar('_');
                        else putchar(list[i].msg[j]);
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", list[i].tag, list[i].msg);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].tag);
        free(list[i].msg);
    }
    free(list);
    return 0;
}
