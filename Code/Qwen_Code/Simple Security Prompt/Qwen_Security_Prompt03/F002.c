// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *tag; char *msg; } Msg;
Msg *m = NULL;
int n = 0;

void print_under(const char *s) { for(;*s;s++) printf("%c", *s==' '?'_':*s); printf("\n"); }

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[20], a[256], b[256], c[256];
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "ADD") == 0) {
            sscanf(line, "%*s %s", a);
            char *t = strchr(line, ' '); t++; t = strchr(t, ' ');
            m = realloc(m, (n + 1) * sizeof(Msg));
            m[n].tag = strdup(a); m[n].msg = strdup(t ? t + 1 : ""); n++;
        } else if (strcmp(cmd, "CONCAT") == 0) {
            sscanf(line, "%*s %s %s %s", a, b, c);
            char *m1 = NULL, *m2 = NULL;
            for (int i = 0; i < n; i++) {
                if (!m1 && strcmp(m[i].tag, a) == 0) m1 = m[i].msg;
                if (!m2 && strcmp(m[i].tag, b) == 0) m2 = m[i].msg;
            }
            if (m1 && m2) {
                char *nm = malloc(strlen(m1) + strlen(m2) + 2);
                sprintf(nm, "%s %s", m1, m2);
                m = realloc(m, (n + 1) * sizeof(Msg));
                m[n].tag = strdup(c); m[n].msg = nm; n++;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            sscanf(line, "%*s %s", a);
            for (int i = 0; i < n; ) {
                if (strcmp(m[i].tag, a) == 0) {
                    free(m[i].tag); free(m[i].msg);
                    for (int j = i; j < n - 1; j++) m[j] = m[j + 1];
                    n--;
                } else i++;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            sscanf(line, "%*s %s", a);
            int found = 0;
            for (int i = 0; i < n; i++) {
                if (strcmp(m[i].tag, a) == 0) { print_under(m[i].msg); found = 1; break; }
            }
            if (!found) printf("NOT_FOUND\n");
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < n; i++) printf("%s %s\n", m[i].tag, m[i].msg);
        }
    }
    for (int i = 0; i < n; i++) { free(m[i].tag); free(m[i].msg); }
    free(m);
    return 0;
}