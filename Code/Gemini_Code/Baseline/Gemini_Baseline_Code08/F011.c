// F011.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *body;
} Draft;

Draft *drafts = NULL;
int count = 0;
int capacity = 0;

char *disc_title = NULL;
char *disc_body = NULL;

int find_draft(const char *title) {
    for (int i = 0; i < count; i++) {
        if (strcmp(drafts[i].title, title) == 0) {
            return i;
        }
    }
    return -1;
}

void create_d(const char *title, const char *body) {
    int idx = find_draft(title);
    if (idx != -1) {
        free(drafts[idx].body);
        drafts[idx].body = strdup(body);
        return;
    }
    if (count >= capacity) {
        capacity = capacity == 0 ? 4 : capacity * 2;
        drafts = realloc(drafts, capacity * sizeof(Draft));
    }
    drafts[count].title = strdup(title);
    drafts[count].body = strdup(body);
    count++;
}

void revise_d(const char *title, const char *body) {
    int idx = find_draft(title);
    if (idx != -1) {
        free(drafts[idx].body);
        drafts[idx].body = strdup(body);
    }
}

void discard_d(const char *title) {
    int idx = find_draft(title);
    if (idx != -1) {
        free(disc_title);
        free(disc_body);
        disc_title = strdup(drafts[idx].title);
        disc_body = strdup(drafts[idx].body);
        free(drafts[idx].title);
        free(drafts[idx].body);
        for (int i = idx; i < count - 1; i++) {
            drafts[i] = drafts[i + 1];
        }
        count--;
    }
}

void restore_d(void) {
    if (disc_title && disc_body) {
        create_d(disc_title, disc_body);
        free(disc_title);
        free(disc_body);
        disc_title = NULL;
        disc_body = NULL;
    }
}

void find_d(const char *title) {
    int idx = find_draft(title);
    if (idx != -1) {
        printf("%s %s\n", drafts[idx].title, drafts[idx].body);
    }
}

void print_d(void) {
    for (int i = 0; i < count; i++) {
        printf("%s %s\n", drafts[i].title, drafts[i].body);
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
        if (strcmp(cmd, "CREATE") == 0) {
            char *title = strtok(NULL, " ");
            char *body = strtok(NULL, "");
            if (title && body) create_d(title, body);
        } else if (strcmp(cmd, "REVISE") == 0) {
            char *title = strtok(NULL, " ");
            char *body = strtok(NULL, "");
            if (title && body) revise_d(title, body);
        } else if (strcmp(cmd, "DISCARD") == 0) {
            char *title = strtok(NULL, " ");
            if (title) discard_d(title);
        } else if (strcmp(cmd, "RESTORE") == 0) {
            restore_d();
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = strtok(NULL, " ");
            if (title) find_d(title);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_d();
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(drafts[i].title);
        free(drafts[i].body);
    }
    free(drafts);
    free(disc_title);
    free(disc_body);
    return 0;
}
