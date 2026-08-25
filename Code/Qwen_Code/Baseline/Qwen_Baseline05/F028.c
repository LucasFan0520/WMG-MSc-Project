// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STOP 100
#define MAX_NOTE 1000

typedef struct {
    char stop[MAX_STOP];
    char note[MAX_NOTE];
} Stop;

Stop stops[10000];
int stop_count = 0;

int find_stop(const char *stop) {
    for (int i = 0; i < stop_count; i++) if (strcmp(stops[i].stop, stop) == 0) return i;
    return -1;
}

int main() {
    char cmd[20], stop[MAX_STOP], note[MAX_NOTE];
    int index, from, to;
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s", stop);
            fgets(note, MAX_NOTE, stdin);
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            if (note[strlen(note) - 1] == '\n') note[strlen(note) - 1] = '\0';
            strcpy(stops[stop_count].stop, stop);
            strcpy(stops[stop_count].note, note);
            stop_count++;
        } else if (strcmp(cmd, "INSERT") == 0) {
            scanf("%d %s", &index, stop);
            fgets(note, MAX_NOTE, stdin);
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            if (note[strlen(note) - 1] == '\n') note[strlen(note) - 1] = '\0';
            if (index >= 0 && index <= stop_count) {
                for (int i = stop_count; i > index; i--) stops[i] = stops[i - 1];
                strcpy(stops[index].stop, stop);
                strcpy(stops[index].note, note);
                stop_count++;
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            scanf("%d %d", &from, &to);
            if (from >= 0 && from < stop_count && to >= 0 && to < stop_count && from != to) {
                Stop temp = stops[from];
                if (from < to) {
                    for (int i = from; i < to; i++) stops[i] = stops[i + 1];
                } else {
                    for (int i = from; i > to; i--) stops[i] = stops[i - 1];
                }
                stops[to] = temp;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            scanf("%d", &index);
            if (index >= 0 && index < stop_count) {
                for (int i = index; i < stop_count - 1; i++) stops[i] = stops[i + 1];
                stop_count--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", stop);
            int idx = find_stop(stop);
            if (idx != -1) printf("%s\n", stops[idx].note);
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < stop_count; i++) printf("%s %s\n", stops[i].stop, stops[i].note);
        }
    }
    return 0;
}