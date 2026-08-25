// F011.c
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
int has_discarded = 0;

int find_draft(const char *title) {
    for (int i = 0; i < count; i++) {
        if (strcmp(drafts[i].title, title) == 0) {
            return i;
        }
    }
    return -1;
}

void add_draft(const char *title, const char *body) {
    if (count >= capacity) {
        capacity = capacity == 0 ? 4 : capacity * 2;
        drafts = realloc(drafts, capacity * sizeof(Draft));
    }
    drafts[count].title = strdup(title);
    drafts[count].body = strdup(body);
    count++;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "CREATE ", 7) == 0) {
            char *p1 = line + 7;
            char *p2 = strchr(p1, ' ');
            if (p2) {
                *p2 = 0;
                add_draft(p1, p2 + 1);
            }
        } else if (strncmp(line, "REVISE ", 7) == 0) {
            char *p1 = line + 7;
            char *p2 = strchr(p1, ' ');
            if (p2) {
                *p2 = 0;
                int idx = find_draft(p1);
                if (idx != -1) {
                    free(drafts[idx].body);
                    drafts[idx].body = strdup(p2 + 1);
                }
            }
        } else if (strncmp(line, "DISCARD ", 8) == 0) {
            char *title = line + 8;
            int idx = find_draft(title);
            if (idx != -1) {
                if (has_discarded) {
                    free(disc_title);
                    free(disc_body);
                }
                disc_title = strdup(drafts[idx].title);
                disc_body = strdup(drafts[idx].body);
                has_discarded = 1;

                free(drafts[idx].title);
                free(drafts[idx].body);
                for (int i = idx; i < count - 1; i++) {
                    drafts[i] = drafts[i + 1];
                }
                count--;
            }
        } else if (strcmp(line, "RESTORE") == 0) {
            if (has_discarded) {
                add_draft(disc_title, disc_body);
                free(disc_title);
                free(disc_body);
                disc_title = NULL;
                disc_body = NULL;
                has_discarded = 0;
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *title = line + 5;
            int idx = find_draft(title);
            if (idx != -1) {
                printf("%s %s\n", drafts[idx].title, drafts[idx].body);
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", drafts[i].title, drafts[i].body);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(drafts[i].title);
        free(drafts[i].body);
    }
    free(drafts);
    if (has_discarded) {
        free(disc_title);
        free(disc_body);
    }
    return 0;
}
