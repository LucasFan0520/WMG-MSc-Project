// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 100
#define MAX_NOTE 1000

typedef struct {
    char name[MAX_NAME];
    char note[MAX_NOTE];
} Record;

Record records[10000];
int rec_count = 0;

int find_rec(const char *name) {
    for (int i = 0; i < rec_count; i++) if (strcmp(records[i].name, name) == 0) return i;
    return -1;
}

int main() {
    char cmd[20], name[MAX_NAME], note[MAX_NOTE];
    int n;
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "BATCH") == 0) {
            scanf("%d", &n);
            Record temp[1000];
            int valid = 1;
            for (int i = 0; i < n; i++) {
                if (scanf("%s", temp[i].name) != 1) { valid = 0; break; }
                fgets(temp[i].note, MAX_NOTE, stdin);
                if (temp[i].note[0] == ' ') memmove(temp[i].note, temp[i].note + 1, strlen(temp[i].note));
                if (temp[i].note[strlen(temp[i].note) - 1] == '\n') temp[i].note[strlen(temp[i].note) - 1] = '\0';
            }
            if (valid) {
                for (int i = 0; i < n; i++) {
                    strcpy(records[rec_count].name, temp[i].name);
                    strcpy(records[rec_count].note, temp[i].note);
                    rec_count++;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", name);
            int idx = find_rec(name);
            if (idx != -1) {
                for (int i = idx; i < rec_count - 1; i++) records[i] = records[i + 1];
                rec_count--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", name);
            int idx = find_rec(name);
            if (idx != -1) printf("%s %s\n", records[idx].name, records[idx].note);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < rec_count; i++) printf("%s %s\n", records[i].name, records[i].note);
        }
    }
    return 0;
}