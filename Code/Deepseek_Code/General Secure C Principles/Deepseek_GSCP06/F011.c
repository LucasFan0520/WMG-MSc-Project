// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Draft {
    char *title;
    char *body;
    struct Draft *next;
} Draft;

Draft *head = NULL;
Draft *last_discarded = NULL; // store the most recently discarded draft

Draft *find_draft(const char *title) {
    Draft *d = head;
    while (d) {
        if (strcmp(d->title, title) == 0) return d;
        d = d->next;
    }
    return NULL;
}

void create_draft(const char *title, const char *body) {
    if (find_draft(title)) return;
    Draft *d = malloc(sizeof(Draft));
    d->title = strdup(title);
    d->body = strdup(body);
    d->next = head;
    head = d;
}

void revise_draft(const char *title, const char *body) {
    Draft *d = find_draft(title);
    if (!d) return;
    free(d->body);
    d->body = strdup(body);
}

void discard_draft(const char *title) {
    Draft *d = find_draft(title);
    if (!d) return;
    // Remove from list
    Draft *cur = head, *prev = NULL;
    while (cur) {
        if (cur == d) {
            if (prev) prev->next = cur->next;
            else head = cur->next;
            break;
        }
        prev = cur;
        cur = cur->next;
    }
    // Save for restore (free previous if any)
    if (last_discarded) {
        free(last_discarded->title);
        free(last_discarded->body);
        free(last_discarded);
    }
    last_discarded = d;
    // d still has title and body, and next is unlinked.
    d->next = NULL;
}

void restore_draft(void) {
    if (!last_discarded) return;
    // Insert back into head (or anywhere, we'll put at head)
    if (find_draft(last_discarded->title)) {
        // title conflict? Possibly restore should overwrite? Spec says "restores that draft if available"
        // We'll assume no conflict, or overwrite? Better to not add if exists.
        // But spec says if available, restore. We'll add it.
        // If exists, we could overwrite or ignore? Let's add anyway, but to keep unique, we can remove existing.
        Draft *existing = find_draft(last_discarded->title);
        if (existing) {
            // remove existing
            Draft *cur = head, *prev = NULL;
            while (cur) {
                if (cur == existing) {
                    if (prev) prev->next = cur->next;
                    else head = cur->next;
                    free(cur->title);
                    free(cur->body);
                    free(cur);
                    break;
                }
                prev = cur;
                cur = cur->next;
            }
        }
    }
    last_discarded->next = head;
    head = last_discarded;
    last_discarded = NULL;
}

void find_draft_cmd(const char *title) {
    Draft *d = find_draft(title);
    if (d) printf("%s\n", d->body);
}

void print_drafts(void) {
    Draft *d = head;
    while (d) {
        printf("%s %s\n", d->title, d->body);
        d = d->next;
    }
}

void cleanup_drafts(void) {
    Draft *d = head;
    while (d) {
        Draft *tmp = d;
        d = d->next;
        free(tmp->title);
        free(tmp->body);
        free(tmp);
    }
    if (last_discarded) {
        free(last_discarded->title);
        free(last_discarded->body);
        free(last_discarded);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], title[256], rest[2048];
        int n = sscanf(line, "%31s %255s %2047s", cmd, title, rest);
        if (n < 2) continue;
        if (strcmp(cmd, "CREATE") == 0) {
            char *body = line + strlen(cmd) + 1 + strlen(title) + 1;
            create_draft(title, body);
        } else if (strcmp(cmd, "REVISE") == 0) {
            char *body = line + strlen(cmd) + 1 + strlen(title) + 1;
            revise_draft(title, body);
        } else if (strcmp(cmd, "DISCARD") == 0) {
            discard_draft(title);
        } else if (strcmp(cmd, "RESTORE") == 0) {
            restore_draft();
        } else if (strcmp(cmd, "FIND") == 0) {
            find_draft_cmd(title);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_drafts();
        }
    }
    cleanup_drafts();
    return 0;
}