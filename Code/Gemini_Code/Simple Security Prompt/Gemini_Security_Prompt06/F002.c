// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *tag;
    char *message;
} MessagePair;

char *read_line(FILE *f) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
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

int main() {
    MessagePair *list = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *p1 = strchr(line, ' ');
        if (!p1) {
            if (strcmp(line, "REPORT") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s %s\n", list[i].tag, list[i].message);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *cmd = line;
        char *rest = p1 + 1;
        if (strcmp(cmd, "ADD") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *tag = rest;
                char *msg = p2 + 1;
                list = realloc(list, (count + 1) * sizeof(MessagePair));
                list[count].tag = strdup(tag);
                list[count].message = strdup(msg);
                count++;
            }
        } else if (strcmp(cmd, "CONCAT") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *tag1 = rest;
                char *rest2 = p2 + 1;
                char *p3 = strchr(rest2, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *tag2 = rest2;
                    char *newtag = p3 + 1;
                    int idx1 = -1;
                    int idx2 = -1;
                    for (int i = 0; i < count; i++) {
                        if (idx1 == -1 && strcmp(list[i].tag, tag1) == 0) {
                            idx1 = i;
                        }
                        if (idx2 == -1 && strcmp(list[i].tag, tag2) == 0) {
                            idx2 = i;
                        }
                    }
                    if (idx1 != -1 && idx2 != -1) {
                        size_t len1 = strlen(list[idx1].message);
                        size_t len2 = strlen(list[idx2].message);
                        char *new_msg = malloc(len1 + len2 + 2);
                        strcpy(new_msg, list[idx1].message);
                        strcat(new_msg, " ");
                        strcat(new_msg, list[idx2].message);
                        list = realloc(list, (count + 1) * sizeof(MessagePair));
                        list[count].tag = strdup(newtag);
                        list[count].message = new_msg;
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *tag = rest;
            int i = 0;
            while (i < count) {
                if (strcmp(list[i].tag, tag) == 0) {
                    free(list[i].tag);
                    free(list[i].message);
                    for (int j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
            if (count == 0) {
                free(list);
                list = NULL;
            } else {
                list = realloc(list, count * sizeof(MessagePair));
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *tag = rest;
            int idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].tag, tag) == 0) {
                    idx = i;
                    break;
                }
            }
            if (idx != -1) {
                for (int i = 0; list[idx].message[i] != '\0'; i++) {
                    if (list[idx].message[i] == ' ') {
                        putchar('_');
                    } else {
                        putchar(list[idx].message[i]);
                    }
                }
                putchar('\n');
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(list[i].tag);
        free(list[i].message);
    }
    free(list);
    return 0;
}