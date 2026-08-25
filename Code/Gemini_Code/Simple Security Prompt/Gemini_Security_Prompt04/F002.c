// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
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
    buf[len] = '\0';
    if (len > 0 && buf[len-1] == '\r') {
        buf[len-1] = '\0';
    }
    return buf;
}

typedef struct {
    char *tag;
    char *msg;
} Record;

int main(void) {
    Record *records = NULL;
    int count = 0;
    int cap = 0;
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
            char *tag = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *msg = p + 1;
                if (count >= cap) {
                    cap = cap == 0 ? 16 : cap * 2;
                    records = realloc(records, cap * sizeof(Record));
                }
                records[count].tag = strdup(tag);
                records[count].msg = strdup(msg);
                count++;
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
                    char *newtag = p + 1;
                    while (*newtag == ' ') newtag++;
                    int idx1 = -1, idx2 = -1;
                    for (int i = 0; i < count; i++) {
                        if (idx1 == -1 && strcmp(records[i].tag, tag1) == 0) idx1 = i;
                        if (idx2 == -1 && strcmp(records[i].tag, tag2) == 0) idx2 = i;
                    }
                    if (idx1 != -1 && idx2 != -1) {
                        size_t l1 = strlen(records[idx1].msg);
                        size_t l2 = strlen(records[idx2].msg);
                        char *nm = malloc(l1 + l2 + 2);
                        strcpy(nm, records[idx1].msg);
                        strcat(nm, " ");
                        strcat(nm, records[idx2].msg);
                        if (count >= cap) {
                            cap = cap == 0 ? 16 : cap * 2;
                            records = realloc(records, cap * sizeof(Record));
                        }
                        records[count].tag = strdup(newtag);
                        records[count].msg = nm;
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *tag = line + 7;
            while (*tag == ' ') tag++;
            int i = 0;
            while (i < count) {
                if (strcmp(records[i].tag, tag) == 0) {
                    free(records[i].tag);
                    free(records[i].msg);
                    for (int j = i; j < count - 1; j++) {
                        records[j] = records[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *tag = line + 5;
            while (*tag == ' ') tag++;
            int idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(records[i].tag, tag) == 0) {
                    idx = i;
                    break;
                }
            }
            if (idx != -1) {
                char *m = records[idx].msg;
                while (*m) {
                    if (*m == ' ') putchar('_');
                    else putchar(*m);
                    m++;
                }
                putchar('\n');
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", records[i].tag, records[i].msg);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(records[i].tag);
        free(records[i].msg);
    }
    free(records);
    return 0;
}
