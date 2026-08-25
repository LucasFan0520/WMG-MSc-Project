// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 65536

typedef struct Rem {
    int priority;
    char *title;
    char *note;
    struct Rem *next;
} Rem;

Rem *head = NULL;

char *get_token(char **s) {
    while (**s == ' ') (*s)++;
    if (!**s) return NULL;
    char *start = *s;
    while (**s && **s != ' ') (*s)++;
    if (**s) { **s = '\0'; (*s)++; }
    return start;
}

Rem *find_rem(const char *title) {
    for (Rem *r = head; r; r = r->next)
        if (strcmp(r->title, title) == 0) return r;
    return NULL;
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *ptr = line;
        char *cmd = get_token(&ptr);
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *pstr = get_token(&ptr);
            char *title = get_token(&ptr);
            if (pstr && title) {
                Rem *r = malloc(sizeof(Rem));
                if (r) {
                    r->priority = atoi(pstr);
                    r->title = strdup(title);
                    r->note = strdup(ptr);
                    r->next = head;
                    head = r;
                }
            }
        } else if (strcmp(cmd, "BUMP") == 0) {
            char *title = get_token(&ptr);
            char *dstr = get_token(&ptr);
            if (title && dstr) {
                Rem *r = find_rem(title);
                if (r) r->priority += atoi(dstr);
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *title = get_token(&ptr);
            if (title) {
                Rem *prev = NULL, *curr = head;
                while (curr) {
                    if (strcmp(curr->title, title) == 0) {
                        if (prev) prev->next = curr->next;
                        else head = curr->next;
                        free(curr->title);
                        free(curr->note);
                        free(curr);
                        break;
                    }
                    prev = curr;
                    curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            char *nstr = get_token(&ptr);
            if (nstr) {
                int n = atoi(nstr);
                int count = 0;
                for (Rem *r = head; r; r = r->next) count++;
                Rem **arr = malloc(count * sizeof(Rem *));
                if (arr) {
                    int i = 0;
                    for (Rem *r = head; r; r = r->next) arr[i++] = r;
                    for (int i = 0; i < count - 1; i++) {
                        for (int j = i + 1; j < count; j++) {
                            if (arr[j]->priority > arr[i]->priority) {
                                Rem *tmp = arr[i];
                                arr[i] = arr[j];
                                arr[j] = tmp;
                            }
                        }
                    }
                    int limit = n < count ? n : count;
                    for (int i = 0; i < limit; i++) {
                        printf("%d %s %s\n", arr[i]->priority, arr[i]->title, arr[i]->note);
                    }
                    free(arr);
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Rem *r = head; r; r = r->next) {
                printf("%d %s %s\n", r->priority, r->title, r->note);
            }
        }
    }
    while (head) {
        Rem *next = head->next;
        free(head->title);
        free(head->note);
        free(head);
        head = next;
    }
    return 0;
}