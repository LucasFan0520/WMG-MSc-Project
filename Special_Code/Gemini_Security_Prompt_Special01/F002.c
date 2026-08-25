// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
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
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
}

typedef struct {
    char *tag;
    char *message;
} MessagePair;

int main(void) {
    MessagePair *list = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p != '\0') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "ADD") == 0) {
            char *tag = p;
            while (*tag == ' ') tag++;
            char *tag_end = tag;
            while (*tag_end && *tag_end != ' ') tag_end++;
            char *msg = tag_end;
            if (*tag_end != '\0') {
                *tag_end = '\0';
                msg++;
            }
            while (*msg == ' ') msg++;
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                MessagePair *new_list = realloc(list, capacity * sizeof(MessagePair));
                if (!new_list) {
                    free(line);
                    break;
                }
                list = new_list;
            }
            list[count].tag = mystrdup(tag);
            list[count].message = mystrdup(msg);
            count++;
        } else if (strcmp(cmd, "CONCAT") == 0) {
            char *t1 = p;
            while (*t1 == ' ') t1++;
            char *t1_end = t1;
            while (*t1_end && *t1_end != ' ') t1_end++;
            if (*t1_end != '\0') {
                *t1_end = '\0';
                t1_end++;
            }
            char *t2 = t1_end;
            while (*t2 == ' ') t2++;
            char *t2_end = t2;
            while (*t2_end && *t2_end != ' ') t2_end++;
            if (*t2_end != '\0') {
                *t2_end = '\0';
                t2_end++;
            }
            char *nt = t2_end;
            while (*nt == ' ') nt++;
            char *nt_end = nt;
            while (*nt_end && *nt_end != ' ') nt_end++;
            *nt_end = '\0';
            char *m1 = NULL;
            char *m2 = NULL;
            for (size_t i = 0; i < count; i++) {
                if (list[i].tag && strcmp(list[i].tag, t1) == 0) {
                    m1 = list[i].message;
                    break;
                }
            }
            for (size_t i = 0; i < count; i++) {
                if (list[i].tag && strcmp(list[i].tag, t2) == 0) {
                    m2 = list[i].message;
                    break;
                }
            }
            if (m1 && m2) {
                size_t len1 = strlen(m1);
                size_t len2 = strlen(m2);
                char *new_msg = malloc(len1 + len2 + 2);
                if (new_msg) {
                    strcpy(new_msg, m1);
                    strcat(new_msg, " ");
                    strcat(new_msg, m2);
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        MessagePair *new_list = realloc(list, capacity * sizeof(MessagePair));
                        if (!new_list) {
                            free(new_msg);
                            free(line);
                            break;
                        }
                        list = new_list;
                    }
                    list[count].tag = mystrdup(nt);
                    list[count].message = new_msg;
                    count++;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *tag = p;
            while (*tag == ' ') tag++;
            char *tag_end = tag;
            while (*tag_end && *tag_end != ' ') tag_end++;
            *tag_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (list[i].tag && strcmp(list[i].tag, tag) == 0) {
                    free(list[i].tag);
                    free(list[i].message);
                    list[i].tag = NULL;
                    list[i].message = NULL;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *tag = p;
            while (*tag == ' ') tag++;
            char *tag_end = tag;
            while (*tag_end && *tag_end != ' ') tag_end++;
            *tag_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (list[i].tag && strcmp(list[i].tag, tag) == 0) {
                    char *t = list[i].message;
                    while (*t) {
                        if (*t == ' ') putchar('_');
                        else putchar(*t);
                        t++;
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (list[i].tag) {
                    printf("%s %s\n", list[i].tag, list[i].message);
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        if (list[i].tag) free(list[i].tag);
        if (list[i].message) free(list[i].message);
    }
    free(list);
    return 0;
}
