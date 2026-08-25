// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; int score; char *note; } Stu;
Stu *stus = NULL; int sc = 0;

int find_stu(const char *n) {
    for (int i = 0; i < sc; i++) if (strcmp(stus[i].name, n) == 0) return i;
    return -1;
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char c1[20], c2[256], c4[700]; int s;
        c2[0] = c4[0] = 0;
        sscanf(line, "%19s %255s %d %700[^\n]", c1, c2, &s, c4);
        
        if (strcmp(c1, "ADD") == 0) {
            int i = find_stu(c2);
            if (i >= 0) { stus[i].score = s; free(stus[i].note); stus[i].note = strdup(c4); }
            else { stus = realloc(stus, (sc+1)*sizeof(Stu)); stus[sc].name = strdup(c2); stus[sc].score = s; stus[sc].note = strdup(c4); sc++; }
        } else if (strcmp(c1, "UPDATE") == 0) {
            int i = find_stu(c2);
            if (i >= 0) { stus[i].score = s; free(stus[i].note); stus[i].note = strdup(c4); }
        } else if (strcmp(c1, "DROP") == 0) {
            int i = find_stu(c2);
            if (i >= 0) { free(stus[i].name); free(stus[i].note); for(int j=i; j<sc-1; j++) stus[j]=stus[j+1]; sc--; }
        } else if (strcmp(c1, "AVERAGE") == 0) {
            int sum = 0;
            for (int i = 0; i < sc; i++) sum += stus[i].score;
            printf("%d\n", sc > 0 ? sum / sc : 0);
        } else if (strcmp(c1, "LIST") == 0) {
            for (int i = 0; i < sc; i++) printf("%s %d %s\n", stus[i].name, stus[i].score, stus[i].note);
        }
    }
    for (int i = 0; i < sc; i++) { free(stus[i].name); free(stus[i].note); }
    free(stus);
    return 0;
}