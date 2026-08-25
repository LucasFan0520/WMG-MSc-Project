// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    char *text;
} Paragraph;

Paragraph *notebook = NULL;
int count = 0;
int capacity = 0;

void add_p(const char *id, const char *text) {
    if (count >= capacity) {
        capacity = capacity == 0 ? 4 : capacity * 2;
        notebook = realloc(notebook, capacity * sizeof(Paragraph));
    }
    notebook[count].id = strdup(id);
    notebook[count].text = strdup(text);
    count++;
}

int find_p(const char *id) {
    for (int i = 0; i < count; i++) {
        if (strcmp(notebook[i].id, id) == 0) {
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
        if (strncmp(line, "NEW ", 4) == 0) {
            char *p1 = line + 4;
            char *p2 = strchr(p1, ' ');
            if (p2) {
                *p2 = 0;
                add_p(p1, p2 + 1);
            }
        } else if (strncmp(line, "APPEND ", 7) == 0) {
            char *p1 = line + 7;
            char *p2 = strchr(p1, ' ');
            if (p2) {
                *p2 = 0;
                int idx = find_p(p1);
                if (idx != -1) {
                    int old_len = strlen(notebook[idx].text);
                    int add_len = strlen(p2 + 1);
                    notebook[idx].text = realloc(notebook[idx].text, old_len + add_len + 1);
                    strcat(notebook[idx].text, p2 + 1);
                }
            }
        } else if (strncmp(line, "REPLACE ", 8) == 0) {
            char *p1 = line + 8;
            char *p2 = strchr(p1, ' ');
            if (p2) {
                *p2 = 0;
                int idx = find_p(p1);
                if (idx != -1) {
                    free(notebook[idx].text);
                    notebook[idx].text = strdup(p2 + 1);
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *id = line + 7;
            int idx = find_p(id);
            if (idx != -1) {
                free(notebook[idx].id);
                free(notebook[idx].text);
                for (int i = idx; i < count - 1; i++) {
                    notebook[i] = notebook[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "SHOW ", 5) == 0) {
            char *id = line + 5;
            int idx = find_p(id);
            if (idx != -1) {
                char *t = notebook[idx].text;
                while (*t) {
                    if (*t == ' ') {
                        putchar('_');
                    } else {
                        putchar(*t);
                    }
                    t++;
                }
                putchar('\n');
            } else {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", notebook[i].id);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(notebook[i].id);
        free(notebook[i].text);
    }
    free(notebook);
    return 0;
}
