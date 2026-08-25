// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
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
    if (len > 0 && buf[len - 1] == '\r') {
        buf[len - 1] = '\0';
    }
    return buf;
}

typedef struct {
    char *tag;
    char *message;
} MessageEntry;

int main(void) {
    MessageEntry *entries = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *tag = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *msg = p;
            if (*tag != '\0') {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    MessageEntry *new_entries = realloc(entries, capacity * sizeof(MessageEntry));
                    if (new_entries) entries = new_entries;
                }
                if (count < capacity) {
                    entries[count].tag = strdup(tag);
                    entries[count].message = strdup(msg);
                    count++;
                }
            }
        } else if (strcmp(cmd, "CONCAT") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *tag1 = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *tag2 = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *newtag = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            char *msg1 = NULL;
            char *msg2 = NULL;
            for (size_t i = 0; i < count; i++) {
                if (!msg1 && strcmp(entries[i].tag, tag1) == 0) {
                    msg1 = entries[i].message;
                }
                if (!msg2 && strcmp(entries[i].tag, tag2) == 0) {
                    msg2 = entries[i].message;
                }
                if (msg1 && msg2) break;
            }
            if (msg1 && msg2 && *newtag != '\0') {
                size_t len1 = strlen(msg1);
                size_t len2 = strlen(msg2);
                char *combined = malloc(len1 + len2 + 2);
                if (combined) {
                    strcpy(combined, msg1);
                    combined[len1] = ' ';
                    strcpy(combined + len1 + 1, msg2);
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        MessageEntry *new_entries = realloc(entries, capacity * sizeof(MessageEntry));
                        if (new_entries) entries = new_entries;
                    }
                    if (count < capacity) {
                        entries[count].tag = strdup(newtag);
                        entries[count].message = combined;
                        count++;
                    } else {
                        free(combined);
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *tag = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            size_t idx = 0;
            while (idx < count) {
                if (strcmp(entries[idx].tag, tag) == 0) {
                    free(entries[idx].tag);
                    free(entries[idx].message);
                    for (size_t j = idx; j < count - 1; j++) {
                        entries[j] = entries[j + 1];
                    }
                    count--;
                } else {
                    idx++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *tag = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(entries[i].tag, tag) == 0) {
                    found = 1;
                    char *t = entries[i].message;
                    while (*t) {
                        if (*t == ' ') putchar('_');
                        else putchar(*t);
                        t++;
                    }
                    putchar('\n');
                    break;
                }
            }
            if (!found) {
                printf("\n");
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", entries[i].tag, entries[i].message);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(entries[i].tag);
        free(entries[i].message);
    }
    free(entries);
    return 0;
}
