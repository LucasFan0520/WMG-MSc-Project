// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Variable;

int main(void) {
    Variable *items = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    long read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }

        if (strcmp(line, "END") == 0) {
            break;
        }

        char *p = line;
        while (*p == ' ' || *p == '	') p++;
        if (*p == '\0') continue;

        char *cmd = p;
        while (*p && *p != ' ' && *p != '	') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        while (*p == ' ' || *p == '	') p++;

        if (strcmp(cmd, "SET") == 0) {
            char *key = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *value = p;

            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].key, key) == 0) {
                    found = (int)i;
                    break;
                }
            }

            if (found != -1) {
                free(items[found].value);
                items[found].value = strdup(value);
                if (!items[found].value) exit(1);
            } else {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Variable *new_items = realloc(items, capacity * sizeof(Variable));
                    if (!new_items) exit(1);
                    items = new_items;
                }
                items[count].key = strdup(key);
                items[count].value = strdup(value);
                if (!items[count].key || !items[count].value) exit(1);
                count++;
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char *key = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].key, key) == 0) {
                    free(items[i].key);
                    free(items[i].value);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "RENDER") == 0) {
            char *text = p;
            for (size_t i = 0; text[i] != '\0'; ) {
                if (text[i] == '{') {
                    size_t j = i + 1;
                    while (text[j] != '\0' && text[j] != '}') {
                        j++;
                    }
                    if (text[j] == '}') {
                        size_t klen = j - (i + 1);
                        char *kbuf = malloc(klen + 1);
                        if (!kbuf) exit(1);
                        memcpy(kbuf, &text[i + 1], klen);
                        kbuf[klen] = '\0';

                        char *val = NULL;
                        for (size_t v = 0; v < count; v++) {
                            if (strcmp(items[v].key, kbuf) == 0) {
                                val = items[v].value;
                                break;
                            }
                        }

                        if (val) {
                            printf("%s", val);
                        } else {
                            fwrite(&text[i], 1, j - i + 1, stdout);
                        }
                        free(kbuf);
                        i = j + 1;
                    } else {
                        putchar(text[i]);
                        i++;
                    }
                } else {
                    putchar(text[i]);
                    i++;
                }
            }
            putchar('\n');
        } else if (strcmp(cmd, "VARS") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", items[i].key, items[i].value);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(items[i].key);
        free(items[i].value);
    }
    free(items);
    free(line);
    return 0;
}
