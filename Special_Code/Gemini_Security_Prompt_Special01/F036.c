// F036.c
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
    char *data;
} LPR_Record;

int main(void) {
    LPR_Record *records = NULL;
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
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            char *pref = name_end;
            if (*name_end != '\0') {
                *name_end = '\0';
                pref++;
            }
            while (*pref == ' ') pref++;
            char *colon = strchr(pref, ':');
            if (colon) {
                *colon = '\0';
                int len = atoi(pref);
                char *data_start = colon + 1;
                if (len >= 0 && strlen(data_start) >= (size_t)len) {
                    char *data_buf = malloc(len + 1);
                    if (data_buf) {
                        memcpy(data_buf, data_start, len);
                        data_buf[len] = '\0';
                        if (count >= capacity) {
                            capacity = capacity == 0 ? 4 : capacity * 2;
                            LPR_Record *new_r = realloc(records, capacity * sizeof(LPR_Record));
                            if (new_r) records = new_r;
                        }
                        records[count].name = mystrdup(name);
                        records[count].data = data_buf;
                        count++;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            *name_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(records[i].name, name) == 0) {
                    free(records[i].name);
                    free(records[i].data);
                    for (size_t j = i; j < count - 1; j++) {
                        records[j] = records[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            *name_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(records[i].name, name) == 0) {
                    char *t = records[i].data;
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
                printf("%s\n", records[i].name);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(records[i].name);
        free(records[i].data);
    }
    free(records);
    return 0;
}
