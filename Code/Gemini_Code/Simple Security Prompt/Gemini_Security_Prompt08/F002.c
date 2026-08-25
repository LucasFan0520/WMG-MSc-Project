// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) { free(buf); return NULL; }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *tag;
    char *message;
} Message;

int main(void) {
    Message *list = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') { free(line); continue; }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) { free(line); break; }
        while (*p == ' ') p++;
        char *arg1 = p;
        if (strcmp(cmd, "ADD") == 0) {
            char *tag = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *message = p;
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                Message *nb = realloc(list, cap * sizeof(Message));
                if (nb) list = nb;
            }
            if (count < cap) {
                list[count].tag = strdup(tag);
                list[count].message = strdup(message);
                count++;
            }
        } else if (strcmp(cmd, "CONCAT") == 0) {
            char *tag1 = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *tag2 = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *newtag = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            char *m1 = NULL;
            char *m2 = NULL;
            for (size_t i = 0; i < count; i++) {
                if (!m1 && strcmp(list[i].tag, tag1) == 0) m1 = list[i].message;
                if (!m2 && strcmp(list[i].tag, tag2) == 0) m2 = list[i].message;
            }
            if (m1 && m2) {
                size_t nlen = strlen(m1) + strlen(m2) + 2;
                char *nmsg = malloc(nlen);
                if (nmsg) {
                    sprintf(nmsg, "%s %s", m1, m2);
                    if (count >= cap) {
                        cap = cap == 0 ? 4 : cap * 2;
                        Message *nb = realloc(list, cap * sizeof(Message));
                        if (nb) list = nb;
                    }
                    if (count < cap) {
                        list[count].tag = strdup(newtag);
                        list[count].message = nmsg;
                        count++;
                    } else {
                        free(nmsg);
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            size_t i = 0;
            while (i < count) {
                if (strcmp(list[i].tag, arg1) == 0) {
                    free(list[i].tag);
                    free(list[i].message);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].tag, arg1) == 0) {
                    for (size_t j = 0; list[i].message[j] != '\0'; j++) {
                        if (list[i].message[j] == ' ') putchar('_');
                        else putchar(list[i].message[j]);
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", list[i].tag, list[i].message);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].tag);
        free(list[i].message);
    }
    free(list);
    return 0;
}
