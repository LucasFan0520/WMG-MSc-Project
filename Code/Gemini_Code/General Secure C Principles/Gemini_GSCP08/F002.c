// F002.c
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
    char *tag;
    char *message;
} Record;

int main(void) {
    Record *arr = NULL;
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
                    printf("%s %s\n", arr[i].tag, arr[i].message);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "ADD") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                char *tag = args;
                char *msg = p2 + 1;
                if (count >= cap) {
                    size_t ncap = cap == 0 ? 4 : cap * 2;
                    Record *narr = realloc(arr, ncap * sizeof(Record));
                    if (narr) {
                        arr = narr;
                        cap = ncap;
                    }
                }
                if (count < cap) {
                    arr[count].tag = strdup(tag);
                    arr[count].message = strdup(msg);
                    count++;
                }
            }
        } else if (strcmp(cmd, "CONCAT") == 0) {
            char *t1 = args;
            char *p2 = strchr(t1, ' ');
            if (p2) {
                *p2 = '\0';
                char *t2 = p2 + 1;
                char *p3 = strchr(t2, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *nt = p3 + 1;
                    char *m1 = NULL;
                    char *m2 = NULL;
                    for (size_t i = 0; i < count; i++) {
                        if (!m1 && strcmp(arr[i].tag, t1) == 0) {
                            m1 = arr[i].message;
                        }
                        if (!m2 && strcmp(arr[i].tag, t2) == 0) {
                            m2 = arr[i].message;
                        }
                    }
                    if (m1 && m2) {
                        size_t nlen = strlen(m1) + 1 + strlen(m2) + 1;
                        char *nmsg = malloc(nlen);
                        if (nmsg) {
                            sprintf(nmsg, "%s %s", m1, m2);
                            if (count >= cap) {
                                size_t ncap = cap == 0 ? 4 : cap * 2;
                                Record *narr = realloc(arr, ncap * sizeof(Record));
                                if (narr) {
                                    arr = narr;
                                    cap = ncap;
                                }
                            }
                            if (count < cap) {
                                arr[count].tag = strdup(nt);
                                arr[count].message = nmsg;
                                count++;
                            } else {
                                free(nmsg);
                            }
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *tag = args;
            size_t i = 0;
            while (i < count) {
                if (strcmp(arr[i].tag, tag) == 0) {
                    free(arr[i].tag);
                    free(arr[i].message);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *tag = args;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].tag, tag) == 0) {
                    for (size_t j = 0; arr[i].message[j] != '\0'; j++) {
                        if (arr[i].message[j] == ' ') {
                            putchar('_');
                        } else {
                            putchar(arr[i].message[j]);
                        }
                    }
                    putchar('\n');
                    break;
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].tag);
        free(arr[i].message);
    }
    free(arr);
    return 0;
}
