/* F011.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *body;
} Draft;

static Draft *drafts = NULL;
static int dcount = 0;
static int dcap = 0;

static char *undo_title = NULL;
static char *undo_body = NULL;
static int has_undo = 0;

static int ensure_cap(void) {
    if (dcount >= dcap) {
        int newcap = dcap == 0 ? 8 : dcap * 2;
        Draft *tmp = realloc(drafts, sizeof(Draft) * newcap);
        if (!tmp) return -1;
        drafts = tmp;
        dcap = newcap;
    }
    return 0;
}

static char *dup_str(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

static int find_draft(const char *title) {
    for (int i = 0; i < dcount; i++) {
        if (strcmp(drafts[i].title, title) == 0)
            return i;
    }
    return -1;
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[64] = {0};
        int offset = 0;
        if (sscanf(line, "%63s%n", cmd, &offset) != 1) continue;

        if (strcmp(cmd, "CREATE") == 0) {
            char title[256] = {0};
            int off2 = 0;
            if (sscanf(line + offset, " %255s%n", title, &off2) != 1) continue;
            const char *body = line + offset + off2;
            while (*body == ' ') body++;
            if (find_draft(title) >= 0) continue;
            if (ensure_cap() != 0) continue;
            drafts[dcount].title = dup_str(title);
            drafts[dcount].body = dup_str(body);
            if (!drafts[dcount].title || !drafts[dcount].body) {
                free(drafts[dcount].title);
                free(drafts[dcount].body);
                continue;
            }
            dcount++;
        } else if (strcmp(cmd, "REVISE") == 0) {
            char title[256] = {0};
            int off2 = 0;
            if (sscanf(line + offset, " %255s%n", title, &off2) != 1) continue;
            const char *body = line + offset + off2;
            while (*body == ' ') body++;
            int idx = find_draft(title);
            if (idx < 0) continue;
            char *nb = dup_str(body);
            if (!nb) continue;
            free(drafts[idx].body);
            drafts[idx].body = nb;
        } else if (strcmp(cmd, "DISCARD") == 0) {
            char title[256] = {0};
            if (sscanf(line + offset, " %255s", title) != 1) continue;
            int idx = find_draft(title);
            if (idx < 0) continue;
            if (has_undo) {
                free(undo_title);
                free(undo_body);
            }
            undo_title = drafts[idx].title;
            undo_body = drafts[idx].body;
            has_undo = 1;
            for (int i = idx; i < dcount - 1; i++)
                drafts[i] = drafts[i + 1];
            dcount--;
        } else if (strcmp(cmd, "RESTORE") == 0) {
            if (!has_undo) continue;
            if (find_draft(undo_title) >= 0) {
                free(undo_title);
                free(undo_body);
                has_undo = 0;
                continue;
            }
            if (ensure_cap() != 0) continue;
            drafts[dcount].title = undo_title;
            drafts[dcount].body = undo_body;
            dcount++;
            undo_title = NULL;
            undo_body = NULL;
            has_undo = 0;
        } else if (strcmp(cmd, "FIND") == 0) {
            char title[256] = {0};
            if (sscanf(line + offset, " %255s", title) != 1) continue;
            int idx = find_draft(title);
            if (idx >= 0) {
                printf("%s %s\n", drafts[idx].title, drafts[idx].body);
            } else {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < dcount; i++) {
                printf("%s %s\n", drafts[i].title, drafts[i].body);
            }
        }
    }
    for (int i = 0; i < dcount; i++) {
        free(drafts[i].title);
        free(drafts[i].body);
    }
    free(drafts);
    if (has_undo) {
        free(undo_title);
        free(undo_body);
    }
    return 0;
}