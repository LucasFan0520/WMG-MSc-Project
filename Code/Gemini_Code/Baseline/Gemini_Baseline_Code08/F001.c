// F001.c
#define _GNU_SOURCE
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

int find_idx(const char *id) {
    for (int i = 0; i < count; i++) {
        if (strcmp(notebook[i].id, id) == 0) {
            return i;
        }
    }
    return -1;
}

void add_p(const char *id, const char *text) {
    int idx = find_idx(id);
    if (idx != -1) {
        free(notebook[idx].text);
        notebook[idx].text = strdup(text);
        return;
    }
    if (count >= capacity) {
        capacity = capacity == 0 ? 4 : capacity * 2;
        notebook = realloc(notebook, capacity * sizeof(Paragraph));
    }
    notebook[count].id = strdup(id);
    notebook[count].text = strdup(text);
    count++;
}

void append_p(const char *id, const char *text) {
    int idx = find_idx(id);
    if (idx != -1) {
        size_t old_len = strlen(notebook[idx].text);
        size_t add_len = strlen(text);
        notebook[idx].text = realloc(notebook[idx].text, old_len + add_len + 1);
        strcpy(notebook[idx].text + old_len, text);
    }
}

void replace_p(const char *id, const char *text) {
    int idx = find_idx(id);
    if (idx != -1) {
        free(notebook[idx].text);
        notebook[idx].text = strdup(text);
    }
}

void delete_p(const char *id) {
    int idx = find_idx(id);
    if (idx != -1) {
        free(notebook[idx].id);
        free(notebook[idx].text);
        for (int i = idx; i < count - 1; i++) {
            notebook[i] = notebook[i + 1];
        }
        count--;
    }
}

void show_p(const char *id) {
    int idx = find_idx(id);
    if (idx == -1) {
        printf("NOT_FOUND\n");
    } else {
        char *t = notebook[idx].text;
        for (int i = 0; t[i] != '\0'; i++) {
            if (t[i] == ' ') {
                putchar('_');
            } else {
                putchar(t[i]);
            }
        }
        putchar('\n');
    }
}

void list_p(void) {
    for (int i = 0; i < count; i++) {
        printf("%s\n", notebook[i].id);
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "NEW") == 0) {
            char *id = strtok(NULL, " ");
            char *text = strtok(NULL, "");
            if (id && text) add_p(id, text);
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *id = strtok(NULL, " ");
            char *text = strtok(NULL, "");
            if (id && text) append_p(id, text);
        } else if (strcmp(cmd, "REPLACE") == 0) {
            char *id = strtok(NULL, " ");
            char *text = strtok(NULL, "");
            if (id && text) replace_p(id, text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *id = strtok(NULL, " ");
            if (id) delete_p(id);
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *id = strtok(NULL, " ");
            if (id) show_p(id);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_p();
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(notebook[i].id);
        free(notebook[i].text);
    }
    free(notebook);
    return 0;
}
