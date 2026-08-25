// F017.c
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
} ConfigPair;

int main(int argc, char **argv) {
    ConfigPair *list = NULL;
    size_t count = 0;
    size_t capacity = 0;
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            while (1) {
                char *fline = read_line(fp);
                if (!fline) break;
                char *eq = strchr(fline, '=');
                if (eq) {
                    *eq = '\0';
                    char *key = fline;
                    char *value = eq + 1;
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        ConfigPair *new_list = realloc(list, capacity * sizeof(ConfigPair));
                        if (new_list) list = new_list;
                    }
                    list[count].key = mystrdup(key);
                    list[count].value = mystrdup(value);
                    count++;
                }
                free(fline);
            }
            fclose(fp);
        }
    }
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
        if (strcmp(cmd, "SET") == 0) {
            char *key = p;
            while (*key == ' ') key++;
            char *key_end = key;
            while (*key_end && *key_end != ' ') key_end++;
            char *value = key_end;
            if (*key_end != '\0') {
                *key_end = '\0';
                value++;
            }
            while (*value == ' ') value++;
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].key, key) == 0) {
                    free(list[i].value);
                    list[i].value = mystrdup(value);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    ConfigPair *new_list = realloc(list, capacity * sizeof(ConfigPair));
                    if (new_list) list = new_list;
                }
                list[count].key = mystrdup(key);
                list[count].value = mystrdup(value);
                count++;
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char *key = p;
            while (*key == ' ') key++;
            char *key_end = key;
            while (*key_end && *key_end != ' ') key_end++;
            *key_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].key, key) == 0) {
                    free(list[i].key);
                    free(list[i].value);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char *key = p;
            while (*key == ' ') key++;
            char *key_end = key;
            while (*key_end && *key_end != ' ') key_end++;
            *key_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].key, key) == 0) {
                    printf("%s\n", list[i].value);
                    break;
                }
            }
        } else if (strcmp(cmd, "WRITE") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s=%s\n", list[i].key, list[i].value);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].key);
        free(list[i].value);
    }
    free(list);
    return 0;
}
