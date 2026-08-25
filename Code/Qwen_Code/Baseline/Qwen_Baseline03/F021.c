// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char ing[100];
    char note[1000];
} Ing;

typedef struct {
    char name[100];
    Ing ings[1000];
    int cnt;
} Rec;

Rec recs[1000];
int rn = 0;

int findRec(char *name) {
    for (int i = 0; i < rn; i++) {
        if (!strcmp(recs[i].name, name)) return i;
    }
    return -1;
}

int main() {
    char cmd[20], name[100], ing[100], note[1000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "RECIPE")) {
            scanf("%s", name);
            strcpy(recs[rn].name, name);
            recs[rn].cnt = 0;
            rn++;
        } else if (!strcmp(cmd, "ING")) {
            scanf("%s %s", name, ing);
            fgets(note, 1000, stdin);
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            note[strcspn(note, "\n")] = 0;
            int i = findRec(name);
            if (i != -1) {
                strcpy(recs[i].ings[recs[i].cnt].ing, ing);
                strcpy(recs[i].ings[recs[i].cnt].note, note);
                recs[i].cnt++;
            }
        } else if (!strcmp(cmd, "REMOVEING")) {
            scanf("%s %s", name, ing);
            int i = findRec(name);
            if (i != -1) {
                for (int j = 0; j < recs[i].cnt; j++) {
                    if (!strcmp(recs[i].ings[j].ing, ing)) {
                        for (int k = j; k < recs[i].cnt - 1; k++) {
                            recs[i].ings[k] = recs[i].ings[k + 1];
                        }
                        recs[i].cnt--;
                        break;
                    }
                }
            }
        } else if (!strcmp(cmd, "DELETERECIPE")) {
            scanf("%s", name);
            int i = findRec(name);
            if (i != -1) {
                for (int j = i; j < rn - 1; j++) {
                    recs[j] = recs[j + 1];
                }
                rn--;
            }
        } else if (!strcmp(cmd, "SHOW")) {
            scanf("%s", name);
            int i = findRec(name);
            if (i != -1) {
                printf("%s\n", recs[i].name);
                for (int j = 0; j < recs[i].cnt; j++) {
                    for (char *p = recs[i].ings[j].note; *p; p++) {
                        printf("%c", *p == ' ' ? '_' : *p);
                    }
                    printf("\n");
                }
            }
        } else if (!strcmp(cmd, "LIST")) {
            for (int i = 0; i < rn; i++) {
                printf("%s\n", recs[i].name);
            }
        }
    }
    return 0;
}