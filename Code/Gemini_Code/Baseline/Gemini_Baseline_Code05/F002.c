// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *tag;
    char *message;
} Record;

Record *items = NULL;
int count = 0;
int capacity = 0;

void add_item(const char *tag, const char *message) {
    if (count >= capacity) {
        capacity = capacity == 0 ? 4 : capacity * 2;
        items = realloc(items, capacity * sizeof(Record));
    }
    items[count].tag = strdup(tag);
    items[count].message = strdup(message);
    count++;
}

int find_first(const char *tag) {
    for (int i = 0; i < count; i++) {
        if (strcmp(items[i].tag, tag) == 0) {
            return i;
        }
    }
    return -1;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p1 = line + 4;
            char *p2 = strchr(p1, ' ');
            if (p2) {
                *p2 = 0;
                add_item(p1, p2 + 1);
            }
        } else if (strncmp(line, "CONCAT ", 7) == 0) {
            char *p1 = line + 7;
            char *p2 = strchr(p1, ' ');
            if (p2) {
                *p2 = 0;
                char *p3 = strchr(p2 + 1, ' ');
                if (p3) {
                    *p3 = 0;
                    int idx1 = find_first(p1);
                    int idx2 = find_first(p2 + 1);
                    if (idx1 != -1 && idx2 != -1) {
                        char *m1 = items[idx1].message;
                        char *m2 = items[idx2].message;
                        int len = strlen(m1) + 1 + strlen(m2) + 1;
                        char *nm = malloc(len);
                        sprintf(nm, "%s %s", m1, m2);
                        add_item(p3 + 1, nm);
                        free(nm);
                    }
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *tag = line + 7;
            int i = 0;
            while (i < count) {
                if (strcmp(items[i].tag, tag) == 0) {
                    free(items[i].tag);
                    free(items[i].message);
                    for (int j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *tag = line + 5;
            int idx = find_first(tag);
            if (idx != -1) {
                char *m = items[idx].message;
                while (*m) {
                    if (*m == ' ') {
                        putchar('_');
                    } else {
                        putchar(*m);
                    }
                    m++;
                }
                putchar('\n');
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", items[i].tag, items[i].message);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(items[i].tag);
        free(items[i].message);
    }
    free(items);
    return 0;
}
