// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; int score; char *note; } Stu;
Stu *s = NULL;
int n = 0;

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[20], name[256];
        int sc;
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "UPDATE") == 0) {
            sscanf(line, "%*s %s %d", name, &sc);
            char *note = strchr(line, ' '); note++; note = strchr(note, ' '); note++; note = strchr(note, ' ');
            int idx = -1;
            for (int i = 0; i < n; i++) if (strcmp(s[i].name, name) == 0) { idx = i; break; }
            if (idx != -1) { s[idx].score = sc; free(s[idx].note); s[idx].note = strdup(note ? note : ""); }
            else { s = realloc(s, (n + 1) * sizeof(Stu)); s[n].name = strdup(name); s[n].score = sc; s[n].note = strdup(note ? note : ""); n++; }
        } else if (strcmp(cmd, "DROP") == 0) {
            sscanf(line, "%*s %s", name);
            for (int i = 0; i < n; i++) {
                if (strcmp(s[i].name, name) == 0) {
                    free(s[i].name); free(s[i].note);
                    for (int j = i; j < n - 1; j++) s[j] = s[j + 1];
                    n--; break;
                }
            }
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            if (n == 0) printf("0\n");
            else {
                int sum = 0;
                for (int i = 0; i < n; i++) sum += s[i].score;
                printf("%d\n", sum / n);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < n; i++) printf("%s %d %s\n", s[i].name, s[i].score, s[i].note);
        }
    }
    for (int i = 0; i < n; i++) { free(s[i].name); free(s[i].note); }
    free(s);
    return 0;
}