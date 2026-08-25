// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *id; char *text; } Para;
Para *p = NULL;
int n = 0;

void print_spaces_as_underscores(const char *s) {
    for (; *s; s++) printf("%c", *s == ' ' ? '_' : *s);
    printf("\n");
}

int find_id(const char *id) {
    for (int i = 0; i < n; i++) if (strcmp(p[i].id, id) == 0) return i;
    return -1;
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[20], id[256], text[800];
        text[0] = 0;
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < n; i++) printf("%s\n", p[i].id);
        } else {
            sscanf(line, "%*s %s", id);
            char *t = strchr(line, ' ');
            if (t) { t++; t = strchr(t, ' '); if (t) strcpy(text, t + 1); }
            
            int idx = find_id(id);
            if (strcmp(cmd, "NEW") == 0) {
                if (idx != -1) { free(p[idx].text); p[idx].text = strdup(text); }
                else { p = realloc(p, (n + 1) * sizeof(Para)); p[n].id = strdup(id); p[n].text = strdup(text); n++; }
            } else if (strcmp(cmd, "APPEND") == 0) {
                if (idx != -1) {
                    char *nt = malloc(strlen(p[idx].text) + strlen(text) + 2);
                    sprintf(nt, "%s %s", p[idx].text, text);
                    free(p[idx].text); p[idx].text = nt;
                }
            } else if (strcmp(cmd, "REPLACE") == 0) {
                if (idx != -1) { free(p[idx].text); p[idx].text = strdup(text); }
            } else if (strcmp(cmd, "DELETE") == 0) {
                if (idx != -1) {
                    free(p[idx].id); free(p[idx].text);
                    for (int i = idx; i < n - 1; i++) p[i] = p[i + 1];
                    n--;
                }
            } else if (strcmp(cmd, "SHOW") == 0) {
                if (idx != -1) print_spaces_as_underscores(p[idx].text);
                else printf("NOT_FOUND\n");
            }
        }
    }
    for (int i = 0; i < n; i++) { free(p[i].id); free(p[i].text); }
    free(p);
    return 0;
}