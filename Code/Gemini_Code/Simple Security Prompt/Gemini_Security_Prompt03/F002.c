// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *tag;
    char *message;
} Record;

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

int main(void) {
    Record *list = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ') p++;
            char *tag = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *msg = p;
            list = realloc(list, (count + 1) * sizeof(Record));
            list[count].tag = strdup(tag);
            list[count].message = strdup(msg);
            count++;
        } else if (strcmp(cmd, "CONCAT") == 0) {
            while (*p == ' ') p++;
            char *tag1 = p;
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
            char *msg1 = NULL;
            char *msg2 = NULL;
            for (int i = 0; i < count; i++) {
                if (list[i].tag && strcmp(list[i].tag, tag1) == 0) {
                    msg1 = list[i].message;
                    break;
                }
            }
            for (int i = 0; i < count; i++) {
                if (list[i].tag && strcmp(list[i].tag, tag2) == 0) {
                    msg2 = list[i].message;
                    break;
                }
            }
            if (msg1 && msg2) {
                size_t len1 = strlen(msg1);
                size_t len2 = strlen(msg2);
                char *new_msg = malloc(len1 + len2 + 2);
                strcpy(new_msg, msg1);
                strcat(new_msg, " ");
                strcat(new_msg, msg2);
                list = realloc(list, (count + 1) * sizeof(Record));
                list[count].tag = strdup(newtag);
                list[count].message = new_msg;
                count++;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ') p++;
            char *tag = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (list[i].tag && strcmp(list[i].tag, tag) == 0) {
                    free(list[i].tag);
                    free(list[i].message);
                    list[i].tag = NULL;
                    list[i].message = NULL;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ') p++;
            char *tag = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (list[i].tag && strcmp(list[i].tag, tag) == 0) {
                    for (int j = 0; list[i].message[j]; j++) {
                        putchar(list[i].message[j] == ' ' ? '_' : list[i].message[j]);
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                if (list[i].tag) {
                    printf("%s %s\n", list[i].tag, list[i].message);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        if (list[i].tag) free(list[i].tag);
        if (list[i].message) free(list[i].message);
    }
    free(list);
    return 0;
}
