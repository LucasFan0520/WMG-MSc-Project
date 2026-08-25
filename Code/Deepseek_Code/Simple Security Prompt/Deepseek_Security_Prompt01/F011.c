// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *title; char *body; } Draft;
Draft *drafts = NULL;
int draft_count = 0, draft_cap = 0;
Draft undo_draft = {NULL, NULL};
int undo_available = 0;

int find_draft(const char *title) {
    for (int i = 0; i < draft_count; i++)
        if (strcmp(drafts[i].title, title) == 0) return i;
    return -1;
}

void create_draft(const char *title, const char *body) {
    if (find_draft(title) >= 0) return;
    if (draft_count == draft_cap) {
        draft_cap = draft_cap ? draft_cap * 2 : 4;
        drafts = realloc(drafts, sizeof(Draft) * draft_cap);
    }
    drafts[draft_count].title = strdup(title);
    drafts[draft_count].body = strdup(body);
    draft_count++;
}

void revise_draft(const char *title, const char *body) {
    int idx = find_draft(title);
    if (idx < 0) return;
    free(drafts[idx].body);
    drafts[idx].body = strdup(body);
}

void discard_draft(const char *title) {
    int idx = find_draft(title);
    if (idx < 0) return;
    if (undo_available) { free(undo_draft.title); free(undo_draft.body); }
    undo_draft.title = strdup(drafts[idx].title);
    undo_draft.body = strdup(drafts[idx].body);
    undo_available = 1;
    free(drafts[idx].title);
    free(drafts[idx].body);
    for (int i = idx; i < draft_count - 1; i++) drafts[i] = drafts[i + 1];
    draft_count--;
}

void restore_draft(void) {
    if (!undo_available) return;
    create_draft(undo_draft.title, undo_draft.body);
    free(undo_draft.title);
    free(undo_draft.body);
    undo_available = 0;
}

void find_draft_print(const char *title) {
    int idx = find_draft(title);
    if (idx < 0) return;
    printf("%s\n", drafts[idx].body);
}

void print_drafts(void) {
    for (int i = 0; i < draft_count; i++)
        printf("%s %s\n", drafts[i].title, drafts[i].body);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], rest[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "CREATE") == 0 || strcmp(cmd, "REVISE") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", arg1, rest) == 2) {
                if (strcmp(cmd, "CREATE") == 0) create_draft(arg1, rest);
                else revise_draft(arg1, rest);
            }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1) discard_draft(arg1);
        } else if (strcmp(cmd, "RESTORE") == 0) restore_draft();
        else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1) find_draft_print(arg1);
        } else if (strcmp(cmd, "PRINT") == 0) print_drafts();
    }
    for (int i = 0; i < draft_count; i++) { free(drafts[i].title); free(drafts[i].body); }
    free(drafts);
    if (undo_available) { free(undo_draft.title); free(undo_draft.body); }
    return 0;
}