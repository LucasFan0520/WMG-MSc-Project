// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_REC 10000
#define MAX_LEN 1000

typedef struct {
    char name[100];
    char note[MAX_LEN];
} Rec;

Rec recs[MAX_REC];
int rcount = 0;

int find_rec(const char *name) {
    for (int i = 0; i < rcount; i++) if (strcmp(recs[i].name, name) == 0) return i;
    return -1;
}

int main() {
    char cmd[20], name[100], note[MAX_LEN];
    int n;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "BATCH") == 0) {
            scanf("%d", &n);
            Rec temp[MAX_REC];
            int valid = 1;
            for (int i = 0; i < n; i++) {
                if (scanf("%s", temp[i].name) != 1) {
                    valid = 0;
                    break;
                }
                fgets(temp[i].note, MAX_LEN, stdin);
                temp[i].note[strcspn(temp[i].note, "\n")] = 0;
                if (temp[i].note[0] == ' ') memmove(temp[i].note, temp[i].note + 1, strlen(temp[i].note));
            }
            if (valid) {
                for (int i = 0; i < n; i++) {
                    if (find_rec(temp[i].name) == -1 && rcount < MAX_REC) {
                        strcpy(recs[rcount].name, temp[i].name);
                        strcpy(recs[rcount].note, temp[i].note);
                        rcount++;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", name);
            int idx = find_rec(name);
            if (idx != -1) {
                for (int i = idx; i < rcount - 1; i++) recs[i] = recs[i + 1];
                rcount--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", name);
            int idx = find_rec(name);
            if (idx != -1) printf("%s\n", recs[idx].note);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < rcount; i++) {
                printf("%s %s\n", recs[i].name, recs[i].note);
            }
        }
    }
    return 0;
}