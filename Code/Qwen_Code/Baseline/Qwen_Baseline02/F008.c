// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; int score; char *note; } Stu;
Stu s[1000]; int ns = 0;

int find(char *name) {
    for (int i = 0; i < ns; i++) if (!strcmp(s[i].name, name)) return i;
    return -1;
}

int main() {
    char cmd[20], name[100], note[10000]; int score;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ADD") || !strcmp(cmd, "UPDATE")) {
            scanf("%s %d", name, &score); fgets(note, sizeof(note), stdin);
            note[strcspn(note, "\n")] = 0;
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            int idx = find(name);
            if (idx != -1) {
                s[idx].score = score; free(s[idx].note); s[idx].note = strdup(note);
            } else {
                s[ns].name = strdup(name); s[ns].score = score; s[ns++].note = strdup(note);
            }
        } else if (!strcmp(cmd, "DROP")) {
            scanf("%s", name); int idx = find(name);
            if (idx != -1) {
                free(s[idx].name); free(s[idx].note);
                for (int i = idx; i < ns - 1; i++) s[i] = s[i + 1];
                ns--;
            }
        } else if (!strcmp(cmd, "AVERAGE")) {
            int sum = 0;
            for (int i = 0; i < ns; i++) sum += s[i].score;
            printf("%d\n", ns > 0 ? sum / ns : 0);
        } else if (!strcmp(cmd, "LIST")) {
            for (int i = 0; i < ns; i++) printf("%s %d %s\n", s[i].name, s[i].score, s[i].note);
        }
    }
    for (int i = 0; i < ns; i++) { free(s[i].name); free(s[i].note); }
    return 0;
}