// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 128;
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

typedef struct {
    char *tag;
    char *message;
} Message;

int main() {
    Message *list = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
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
                list = realloc(list, (count + 1) * sizeof(Message));
                list[count].tag = strdup(tag);
                list[count].message = strdup(msg);
                count++;
            }
        } else if (strncmp(line, "CONCAT ", 7) == 0) {
            char t1[128], t2[128], nt[128];
            if (sscanf(line + 7, "%127s %127s %127s", t1, t2, nt) == 3) {
                char *m1 = NULL;
                char *m2 = NULL;
                for (int i = 0; i < count; i++) {
                    if (!m1 && strcmp(list[i].tag, t1) == 0) m1 = list[i].message;
                    if (!m2 && strcmp(list[i].tag, t2) == 0) m2 = list[i].message;
                }
                if (m1 && m2) {
                    size_t len1 = strlen(m1);
                    size_t len2 = strlen(m2);
                    char *new_msg = malloc(len1 + len2 + 2);
                    strcpy(new_msg, m1);
                    strcat(new_msg, " ");
                    strcat(new_msg, m2);
                    list = realloc(list, (count + 1) * sizeof(Message));
                    list[count].tag = strdup(nt);
                    list[count].message = new_msg;
                    count++;
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *tag = line + 7;
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
            list = realloc(list, count * sizeof(Message));
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *tag = line + 5;
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].tag, tag) == 0) {
                    char *m = list[i].message;
                    for (int j = 0; m[j] != '\0'; j++) {
                        if (m[j] == ' ') printf("_");
                        else printf("%c", m[j]);
                    }
                    printf("\n");
                    found = 1;
                    break;
                }
            }
            if (!found) printf("NOT_FOUND\n");
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", list[i].tag, list[i].message);
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
