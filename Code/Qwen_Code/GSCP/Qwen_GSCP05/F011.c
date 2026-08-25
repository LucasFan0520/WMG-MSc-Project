// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *title; char *body; } Draft;
Draft *drafts = NULL; int dc = 0;
Draft *discarded = NULL; int dsc = 0;

int find_draft(const char *t) {
    for (int i = 0; i < dc; i++) if (strcmp(drafts[i].title, t) == 0) return i;
    return -1;
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char c1[20], c2[256], c3[700];
        c2[0] = c3[0] = 0;
        sscanf(line, "%19s %255s %700[^\n]", c1, c2, c3);
        
        if (strcmp(c1, "CREATE") == 0) {
            int i = find_draft(c2);
            if (i < 0) { drafts = realloc(drafts, (dc+1)*sizeof(Draft)); drafts[dc].title = strdup(c2); drafts[dc].body = strdup(c3); dc++; }
        } else if (strcmp(c1, "REVISE") == 0) {
            int i = find_draft(c2);
            if (i >= 0) { free(drafts[i].body); drafts[i].body = strdup(c3); }
        } else if (strcmp(c1, "DISCARD") == 0) {
            int i = find_draft(c2);
            if (i >= 0) {
                discarded = realloc(discarded, (dsc+1)*sizeof(Draft));
                discarded[dsc].title = strdup(drafts[i].title);
                discarded[dsc].body = strdup(drafts[i].body);
                dsc++;
                free(drafts[i].title); free(drafts[i].body);
                for (int j = i; j < dc-1; j++) drafts[j] = drafts[j+1];
                dc--;
            }
        } else if (strcmp(c1, "RESTORE") == 0) {
            if (dsc > 0) {
                drafts = realloc(drafts, (dc+1)*sizeof(Draft));
                drafts[dc] = discarded[dsc-1];
                dc++; dsc--;
            }
        } else if (strcmp(c1, "FIND") == 0) {
            int i = find_draft(c2);
            if (i >= 0) printf("%s %s\n", drafts[i].title, drafts[i].body);
        } else if (strcmp(c1, "PRINT") == 0) {
            for (int i = 0; i < dc; i++) printf("%s %s\n", drafts[i].title, drafts[i].body);
        }
    }
    for (int i = 0; i < dc; i++) { free(drafts[i].title); free(drafts[i].body); }
    for (int i = 0; i < dsc; i++) { free(discarded[i].title); free(discarded[i].body); }
    free(drafts); free(discarded);
    return 0;
}