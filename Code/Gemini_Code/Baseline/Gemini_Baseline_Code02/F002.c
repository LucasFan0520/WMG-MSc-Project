// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *str = malloc(size);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_str = realloc(str, size);
            if (!new_str) { free(str); return NULL; }
            str = new_str;
        }
        str[len++] = c;
    }
    if (len == 0 && c == EOF) { free(str); return NULL; }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *tag;
    char *message;
} Record;

int main() {
    Record *records = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "REPORT") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s %s\n", records[i].tag, records[i].message);
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
                records = realloc(records, sizeof(Record) * (count + 1));
                records[count].tag = strdup(tag);
                records[count].message = strdup(msg);
                count++;
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
                    char *newtag = p3 + 1;
                    char *m1 = NULL;
                    char *m2 = NULL;
                    for (int i = 0; i < count; i++) {
                        if (!m1 && strcmp(records[i].tag, t1) == 0) m1 = records[i].message;
                        if (!m2 && strcmp(records[i].tag, t2) == 0) m2 = records[i].message;
                    }
                    if (m1 && m2) {
                        size_t nlen = strlen(m1) + strlen(m2) + 2;
                        char *nmsg = malloc(nlen);
                        sprintf(nmsg, "%s %s", m1, m2);
                        records = realloc(records, sizeof(Record) * (count + 1));
                        records[count].tag = strdup(newtag);
                        records[count].message = nmsg;
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *tag = args;
            int i = 0;
            while (i < count) {
                if (strcmp(records[i].tag, tag) == 0) {
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
            if (count == 0) { free(records); records = NULL; }
            else { records = realloc(records, sizeof(Record) * count); }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *tag = args;
            for (int i = 0; i < count; i++) {
                if (strcmp(records[i].tag, tag) == 0) {
                    for (int j = 0; records[i].message[j]; j++) {
                        if (records[i].message[j] == ' ') printf("_");
                        else printf("%c", records[i].message[j]);
                    }
                    printf("\n");
                    break;
                }
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
