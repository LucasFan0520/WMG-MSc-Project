// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *title;
    char *body;
} Draft;

int main() {
    Draft *drafts = NULL;
    int count = 0;
    Draft last_discarded = {NULL, NULL};
    int has_discarded = 0;
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[16];
        int read_bytes = 0;
        if (sscanf(line, "%15s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *args = line + read_bytes;
        while (*args == ' ') args++;
        if (strcmp(cmd, "CREATE") == 0 || strcmp(cmd, "REVISE") == 0) {
            char title[256];
            int title_bytes = 0;
            if (sscanf(args, "%255s%n", title, &title_bytes) == 1) {
                char *body = args + title_bytes;
                while (*body == ' ') body++;
                if (strcmp(cmd, "CREATE") == 0) {
                    drafts = realloc(drafts, (count + 1) * sizeof(Draft));
                    drafts[count].title = strdup(title);
                    drafts[count].body = strdup(body);
                    count++;
                } else {
                    for (int i = 0; i < count; i++) {
                        if (strcmp(drafts[i].title, title) == 0) {
                            free(drafts[i].body);
                            drafts[i].body = strdup(body);
                            break;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            char title[256];
            if (sscanf(args, "%255s", title) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(drafts[i].title, title) == 0) {
                        if (has_discarded) {
                            free(last_discarded.title);
                            free(last_discarded.body);
                        }
                        last_discarded.title = drafts[i].title;
                        last_discarded.body = drafts[i].body;
                        has_discarded = 1;
                        for (int j = i; j < count - 1; j++) {
                            drafts[j] = drafts[j + 1];
                        }
                        count--;
                        drafts = realloc(drafts, count * sizeof(Draft));
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "RESTORE") == 0) {
            if (has_discarded) {
                drafts = realloc(drafts, (count + 1) * sizeof(Draft));
                drafts[count] = last_discarded;
                count++;
                has_discarded = 0;
                last_discarded.title = NULL;
                last_discarded.body = NULL;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char title[256];
            if (sscanf(args, "%255s", title) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(drafts[i].title, title) == 0) {
                        printf("%s %s\n", drafts[i].title, drafts[i].body);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
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
        free(last_discarded.title);
        free(last_discarded.body);
    }
    return 0;
}
