// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 1024;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) { free(buf); return NULL; }
            buf = new_buf;
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
} Record;

int main() {
    Record *records = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char cmd[64];
        int n = 0;
        if (sscanf(line, "%63s%n", cmd, &n) < 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", records[i].tag, records[i].message);
            }
            free(line);
            continue;
        }
        char *p = line + n;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        if (strcmp(cmd, "ADD") == 0) {
            char *tag_start = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            int tag_len = p - tag_start;
            char *tag = malloc(tag_len + 1);
            memcpy(tag, tag_start, tag_len);
            tag[tag_len] = '\0';
            while (*p == ' ' || *p == '\t') p++;
            char *message = p;
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                records = realloc(records, capacity * sizeof(Record));
            }
            records[count].tag = tag;
            records[count].message = strdup(message);
            count++;
        } else if (strcmp(cmd, "CONCAT") == 0) {
            char t1[256], t2[256], nt[256];
            if (sscanf(p, "%255s %255s %255s", t1, t2, nt) == 3) {
                int f1 = -1, f2 = -1;
                for (int i = 0; i < count; i++) {
                    if (f1 == -1 && strcmp(records[i].tag, t1) == 0) f1 = i;
                    if (f2 == -1 && strcmp(records[i].tag, t2) == 0) f2 = i;
                }
                if (f1 != -1 && f2 != -1) {
                    size_t len1 = strlen(records[f1].message);
                    size_t len2 = strlen(records[f2].message);
                    char *nm = malloc(len1 + len2 + 2);
                    strcpy(nm, records[f1].message);
                    strcat(nm, " ");
                    strcat(nm, records[f2].message);
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        records = realloc(records, capacity * sizeof(Record));
                    }
                    records[count].tag = strdup(nt);
                    records[count].message = nm;
                    count++;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char tag[256];
            if (sscanf(p, "%255s", tag) == 1) {
                int write_idx = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(records[i].tag, tag) == 0) {
                        free(records[i].tag);
                        free(records[i].message);
                    } else {
                        records[write_idx++] = records[i];
                    }
                }
                count = write_idx;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char tag[256];
            if (sscanf(p, "%255s", tag) == 1) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(records[i].tag, tag) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    for (int i = 0; records[found].message[i]; i++) {
                        if (records[found].message[i] == ' ') {
                            printf("_");
                        } else {
                            printf("%c", records[found].message[i]);
                        }
                    }
                    printf("\n");
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
