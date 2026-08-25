// F010.c
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
    char *key;
    char *value;
} IndexPair;

int main(void) {
    IndexPair *pairs = NULL;
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
            char *key = p;
            while (*key == ' ') key++;
            char *key_end = key;
            while (*key_end && *key_end != ' ') key_end++;
            char *val = key_end;
            if (*key_end != '\0') {
                *key_end = '\0';
                val++;
            }
            while (*val == ' ') val++;
            char *val_end = val;
            while (*val_end && *val_end != ' ') val_end++;
            *val_end = '\0';
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                IndexPair *new_p = realloc(pairs, capacity * sizeof(IndexPair));
                if (new_p) pairs = new_p;
            }
            pairs[count].key = mystrdup(key);
            pairs[count].value = mystrdup(val);
            count++;
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *key = p;
            while (*key == ' ') key++;
            char *key_end = key;
            while (*key_end && *key_end != ' ') key_end++;
            char *val = key_end;
            if (*key_end != '\0') {
                *key_end = '\0';
                val++;
            }
            while (*val == ' ') val++;
            char *val_end = val;
            while (*val_end && *val_end != ' ') val_end++;
            *val_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(pairs[i].key, key) == 0 && strcmp(pairs[i].value, val) == 0) {
                    free(pairs[i].key);
                    free(pairs[i].value);
                    for (size_t j = i; j < count - 1; j++) {
                        pairs[j] = pairs[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char *key = p;
            while (*key == ' ') key++;
            char *key_end = key;
            while (*key_end && *key_end != ' ') key_end++;
            *key_end = '\0';
            for (size_t i = 0; i < count; ) {
                if (strcmp(pairs[i].key, key) == 0) {
                    free(pairs[i].key);
                    free(pairs[i].value);
                    for (size_t j = i; j < count - 1; j++) {
                        pairs[j] = pairs[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "VALUES") == 0) {
            char *key = p;
            while (*key == ' ') key++;
            char *key_end = key;
            while (*key_end && *key_end != ' ') key_end++;
            *key_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(pairs[i].key, key) == 0) {
                    printf("%s\n", pairs[i].value);
                }
            }
        } else if (strcmp(cmd, "KEYS") == 0) {
            for (size_t i = 0; i < count; i++) {
                int already_printed = 0;
                for (size_t j = 0; j < i; j++) {
                    if (strcmp(pairs[j].key, pairs[i].key) == 0) {
                        already_printed = 1;
                        break;
                    }
                }
                if (!already_printed) {
                    printf("%s\n", pairs[i].key);
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(pairs[i].key);
        free(pairs[i].value);
    }
    free(pairs);
    return 0;
}
