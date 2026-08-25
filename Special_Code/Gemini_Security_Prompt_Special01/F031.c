// F031.c
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
    char *name;
    char *note;
} BIItem;

int main(void) {
    BIItem *records = NULL;
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
        if (strcmp(cmd, "BATCH") == 0) {
            char *n_s = p;
            while (*n_s == ' ') n_s++;
            char *n_end = n_s;
            while (*n_end && *n_end != ' ') n_end++;
            *n_end = '\0';
            int n = atoi(n_s);
            if (n < 0) n = 0;
            BIItem *batch = malloc(n * sizeof(BIItem));
            int valid = 1;
            for (int i = 0; i < n; i++) {
                char *bline = read_line(stdin);
                if (!bline) {
                    valid = 0;
                    batch[i].name = NULL;
                    batch[i].note = NULL;
                    continue;
                }
                char *name = bline;
                while (*name == ' ') name++;
                if (*name == '\0') {
                    valid = 0;
                    batch[i].name = NULL;
                    batch[i].note = NULL;
                } else {
                    char *name_end = name;
                    while (*name_end && *name_end != ' ') name_end++;
                    char *note = name_end;
                    if (*name_end != '\0') {
                        *name_end = '\0';
                        note++;
                    }
                    while (*note == ' ') note++;
                    batch[i].name = mystrdup(name);
                    batch[i].note = mystrdup(note);
                }
                free(bline);
            }
            if (valid) {
                for (int i = 0; i < n; i++) {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        BIItem *new_r = realloc(records, capacity * sizeof(BIItem));
                        if (new_r) records = new_r;
                    }
                    records[count++] = batch[i];
                }
            } else {
                for (int i = 0; i < n; i++) {
                    if (batch[i].name) free(batch[i].name);
                    if (batch[i].note) free(batch[i].note);
                }
            }
            free(batch);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            *name_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(records[i].name, name) == 0) {
                    free(records[i].name);
                    free(records[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        records[j] = records[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            *name_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(records[i].name, name) == 0) {
                    printf("%s %s\n", records[i].name, records[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", records[i].name, records[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(records[i].name);
        free(records[i].note);
    }
    free(records);
    return 0;
}
