// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Stop { char *name; char *note; } Stop;
Stop *stops = NULL; int count = 0, cap = 0;

void ensure_cap() { if (count == cap) { cap = cap ? cap * 2 : 4; stops = realloc(stops, cap * sizeof(Stop)); } }
void cmd_add(char *n, char *t) { ensure_cap(); stops[count].name = strdup(n); stops[count].note = strdup(t); count++; }
void cmd_insert(int i, char *n, char *t) {
    if (i < 0 || i > count) return; ensure_cap();
    for (int j = count; j > i; j--) stops[j] = stops[j-1];
    stops[i].name = strdup(n); stops[i].note = strdup(t); count++;
}
void cmd_move(int f, int t) {
    if (f < 0 || f >= count || t < 0 || t >= count || f == t) return;
    Stop tmp = stops[f];
    if (f < t) { for (int i = f; i < t; i++) stops[i] = stops[i+1]; stops[t] = tmp; }
    else { for (int i = f; i > t; i--) stops[i] = stops[i-1]; stops[t] = tmp; }
}
void cmd_remove(int i) {
    if (i < 0 || i >= count) return;
    free(stops[i].name); free(stops[i].note);
    for (int j = i; j < count - 1; j++) stops[j] = stops[j+1];
    count--;
}
void cmd_find(char *n) { for (int i = 0; i < count; i++) if (!strcmp(stops[i].name, n)) { printf("%s\n", stops[i].note); return; } }
void cmd_print() { for (int i = 0; i < count; i++) printf("%s %s\n", stops[i].name, stops[i].note); }
void free_all() { for (int i = 0; i < count; i++) { free(stops[i].name); free(stops[i].note); } free(stops); }
int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0; if (!strcmp(line, "END")) break;
        char cmd[32], a1[256], a2[4096] = {0}; int i, j;
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (!strcmp(cmd, "ADD") && sscanf(line, "%*s %255s %4095[^\n]", a1, a2) >= 1) cmd_add(a1, a2);
        else if (!strcmp(cmd, "INSERT") && sscanf(line, "%*s %d %255s %4095[^\n]", &i, a1, a2) >= 2) cmd_insert(i, a1, a2);
        else if (!strcmp(cmd, "MOVE") && sscanf(line, "%*s %d %d", &i, &j) == 2) cmd_move(i, j);
        else if (!strcmp(cmd, "REMOVE") && sscanf(line, "%*s %d", &i) == 1) cmd_remove(i);
        else if (!strcmp(cmd, "FIND") && sscanf(line, "%*s %255s", a1) == 1) cmd_find(a1);
        else if (!strcmp(cmd, "PRINT")) cmd_print();
    }
    free_all(); return 0;
}