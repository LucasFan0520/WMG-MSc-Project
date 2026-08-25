// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Draft {
    char *title, *body;
    struct Draft *next;
} Draft;

Draft *head = NULL;
Draft *discarded = NULL;

void read_rest(char *buf) {
    if (!fgets(buf, 1024, stdin)) buf[0] = '\0';
    if (buf[0] == ' ') memmove(buf, buf + 1, strlen(buf));
    size_t l = strlen(buf);
    if (l > 0 && buf[l-1] == '\n') buf[l-1] = '\0';
}

int main() {
    char cmd[20], title[256], body[1024];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "CREATE") == 0) {
            scanf("%s", title); read_rest(body);
            Draft *d = malloc(sizeof(Draft));
            d->title = strdup(title); d->body = strdup(body); d->next = head; head = d;
        } else if (strcmp(cmd, "REVISE") == 0) {
            scanf("%s", title); read_rest(body);
            for (Draft *d = head; d; d = d->next)
                if (strcmp(d->title, title) == 0) { free(d->body); d->body = strdup(body); break; }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            scanf("%s", title);
            Draft *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->title, title) == 0) {
                    if (prev) prev->next = curr->next; else head = curr->next;
                    if (discarded) { free(discarded->title); free(discarded->body); free(discarded); }
                    discarded = curr;
                    discarded->next = NULL;
                    break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "RESTORE") == 0) {
            if (discarded) {
                discarded->next = head; head = discarded;
                discarded = NULL;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", title);
            for (Draft *d = head; d; d = d->next)
                if (strcmp(d->title, title) == 0) { printf("%s\n", d->body); break; }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (Draft *d = head; d; d = d->next) printf("%s %s\n", d->title, d->body);
        }
    }
    return 0;
}