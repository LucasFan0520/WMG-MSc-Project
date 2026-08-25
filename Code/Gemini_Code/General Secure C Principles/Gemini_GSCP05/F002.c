// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t capacity = 16;
    size_t len = 0;
    char *str = malloc(capacity);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
        if (len + 1 >= capacity) {
            capacity *= 2;
            char *new_str = realloc(str, capacity);
            if (!new_str) {
                free(str);
                return NULL;
            }
            str = new_str;
        }
        str[len++] = (char)c;
    }
    if (len == 0 && c == EOF) {
        free(str);
        return NULL;
    }
    if (len > 0 && str[len - 1] == '\n') {
        len--;
    }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *tag;
    char *message;
} Record;

int main(void) {
    Record *records = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *tag_start = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *msg_start = p + 1;
                while (*msg_start == ' ') msg_start++;
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Record *new_r = realloc(records, capacity * sizeof(Record));
                    if (new_r) records = new_r;
                }
                if (count < capacity) {
                    records[count].tag = strdup(tag_start);
                    records[count].message = strdup(msg_start);
                    count++;
                }
            }
        } else if (strncmp(line, "CONCAT ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *tag1 = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                char *tag2 = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    p++;
                    while (*p == ' ') p++;
                    char *newtag = p;
                    char *msg1 = NULL;
                    char *msg2 = NULL;
                    for (int i = 0; i < count; i++) {
                        if (!msg1 && strcmp(records[i].tag, tag1) == 0) {
                            msg1 = records[i].message;
                        }
                        if (!msg2 && strcmp(records[i].tag, tag2) == 0) {
                            msg2 = records[i].message;
                        }
                    }
                    if (msg1 && msg2) {
                        size_t len1 = strlen(msg1);
                        size_t len2 = strlen(msg2);
                        char *new_msg = malloc(len1 + len2 + 2);
                        if (new_msg) {
                            strcpy(new_msg, msg1);
                            new_msg[len1] = ' ';
                            strcpy(new_msg + len1 + 1, msg2);
                            if (count >= capacity) {
                                capacity = capacity == 0 ? 4 : capacity * 2;
                                Record *new_r = realloc(records, capacity * sizeof(Record));
                                if (new_r) records = new_r;
                            }
                            if (count < capacity) {
                                records[count].tag = strdup(newtag);
                                records[count].message = new_msg;
                                count++;
                            } else {
                                free(new_msg);
                            }
                        }
                    }
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *tag_start = line + 7;
            while (*tag_start == ' ') tag_start++;
            int i = 0;
            while (i < count) {
                if (strcmp(records[i].tag, tag_start) == 0) {
                    free(records[i].tag);
                    free(records[i].message);
                    for (int j = i; j < count - 1; j++) {
                        records[j] = records[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *tag_start = line + 5;
            while (*tag_start == ' ') tag_start++;
            for (int i = 0; i < count; i++) {
                if (strcmp(records[i].tag, tag_start) == 0) {
                    char *m = records[i].message;
                    while (*m) {
                        if (*m == ' ') putchar('_');
                        else putchar(*m);
                        m++;
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", records[i].tag, records[i].message);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(records[i].tag);
        free(records[i].message);
    }
    free(records);
    return 0;
}
