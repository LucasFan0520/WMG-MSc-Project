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
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        int len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[32];
        int read_bytes = 0;
        if (sscanf(line, "%31s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *ptr = line + read_bytes;
        while (*ptr == ' ') ptr++;
        if (strcmp(cmd, "CREATE") == 0 || strcmp(cmd, "REVISE") == 0) {
            char title[256];
            int t_len = 0;
            if (sscanf(ptr, "%255s%n", title, &t_len) > 0) {
                char *body = ptr + t_len;
                while (*body == ' ') body++;
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(drafts[i].title, title) == 0) {
                        found = i;
                        break;
                    }
                }
                if (strcmp(cmd, "CREATE") == 0) {
                    if (found == -1) {
                        drafts = realloc(drafts, (count + 1) * sizeof(Draft));
                        drafts[count].title = strdup(title);
                        drafts[count].body = strdup(body);
                        count++;
                    }
                } else {
                    if (found != -1) {
                        free(drafts[found].body);
                        drafts[found].body = strdup(body);
                    }
                }
            }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            char title[256];
            if (sscanf(ptr, "%255s", title) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(drafts[i].title, title) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(last_discarded.title);
                    free(last_discarded.body);
                    last_discarded.title = strdup(drafts[found].title);
                    last_discarded.body = strdup(drafts[found].body);
                    free(drafts[found].title);
                    free(drafts[found].body);
                    for (int i = found; i < count - 1; i++) {
                        drafts[i] = drafts[i + 1];
                    }
                    count--;
                    if (count > 0) {
                        drafts = realloc(drafts, count * sizeof(Draft));
                    } else {
                        free(drafts);
                        drafts = NULL;
                    }
                }
            }
        } else if (strcmp(cmd, "RESTORE") == 0) {
            if (last_discarded.title) {
                drafts = realloc(drafts, (count + 1) * sizeof(Draft));
                drafts[count].title = strdup(last_discarded.title);
                drafts[count].body = strdup(last_discarded.body);
                count++;
                free(last_discarded.title);
                free(last_discarded.body);
                last_discarded.title = NULL;
                last_discarded.body = NULL;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char title[256];
            if (sscanf(ptr, "%255s", title) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(drafts[i].title, title) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    printf("%s %s\n", drafts[found].title, drafts[found].body);
                } else {
                    printf("NOT_FOUND\n");
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
    free(last_discarded.title);
    free(last_discarded.body);
    return 0;
}
