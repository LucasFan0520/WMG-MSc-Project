// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *tag;
    char *message;
} Record;

int main(void) {
    Record *items = NULL;
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

        if (strcmp(cmd, "ADD") == 0) {
            char *tag = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *message = p;

            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Record *new_items = realloc(items, capacity * sizeof(Record));
                if (!new_items) exit(1);
                items = new_items;
            }
            items[count].tag = strdup(tag);
            items[count].message = strdup(message);
            if (!items[count].tag || !items[count].message) exit(1);
            count++;
        } else if (strcmp(cmd, "CONCAT") == 0) {
            char *tag1 = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;

            char *tag2 = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;

            char *newtag = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            char *msg1 = NULL;
            char *msg2 = NULL;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].tag, tag1) == 0) {
                    msg1 = items[i].message;
                    break;
                }
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].tag, tag2) == 0) {
                    msg2 = items[i].message;
                    break;
                }
            }

            if (msg1 && msg2) {
                size_t l1 = strlen(msg1);
                size_t l2 = strlen(msg2);
                char *combined = malloc(l1 + l2 + 2);
                if (!combined) exit(1);
                strcpy(combined, msg1);
                strcat(combined, " ");
                strcat(combined, msg2);

                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Record *new_items = realloc(items, capacity * sizeof(Record));
                    if (!new_items) exit(1);
                    items = new_items;
                }
                items[count].tag = strdup(newtag);
                items[count].message = combined;
                if (!items[count].tag || !items[count].message) exit(1);
                count++;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *tag = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; ) {
                if (strcmp(items[i].tag, tag) == 0) {
                    free(items[i].tag);
                    free(items[i].message);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *tag = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].tag, tag) == 0) {
                    char *m = items[i].message;
                    for (size_t j = 0; m[j] != '\0'; j++) {
                        if (m[j] == ' ') putchar('_');
                        else putchar(m[j]);
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", items[i].tag, items[i].message);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(items[i].tag);
        free(items[i].message);
    }
    free(items);
    free(line);
    return 0;
}
