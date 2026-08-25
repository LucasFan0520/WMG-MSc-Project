// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *stop;
    char *note;
} Stop;

Stop *stops = NULL;
int s_count = 0;

int find_stop(const char *name) {
    for (int i = 0; i < s_count; i++) {
        if (strcmp(stops[i].stop, name) == 0) {
            return i;
        }
    }
    return -1;
}

int main() {
    char cmd[32];
    char stop[1024];
    char note[10240];
    int index;
    int from_idx;
    int to_idx;

    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s", stop);
            fgets(note, sizeof(note), stdin);
            if (note[0] == ' ') {
                memmove(note, note + 1, strlen(note));
            }
            note[strcspn(note, "\n")] = '\0';
            stops = realloc(stops, sizeof(Stop) * (s_count + 1));
            stops[s_count].stop = strdup(stop);
            stops[s_count].note = strdup(note);
            s_count++;
        } else if (strcmp(cmd, "INSERT") == 0) {
            scanf("%d %s", &index, stop);
            fgets(note, sizeof(note), stdin);
            if (note[0] == ' ') {
                memmove(note, note + 1, strlen(note));
            }
            note[strcspn(note, "\n")] = '\0';
            if (index < 0 || index > s_count) {
                continue;
            }
            stops = realloc(stops, sizeof(Stop) * (s_count + 1));
            for (int i = s_count; i > index; i--) {
                stops[i] = stops[i - 1];
            }
            stops[index].stop = strdup(stop);
            stops[index].note = strdup(note);
            s_count++;
        } else if (strcmp(cmd, "MOVE") == 0) {
            scanf("%d %d", &from_idx, &to_idx);
            if (from_idx < 0 || from_idx >= s_count || to_idx < 0 || to_idx >= s_count) {
                continue;
            }
            Stop tmp = stops[from_idx];
            if (from_idx < to_idx) {
                for (int i = from_idx; i < to_idx; i++) {
                    stops[i] = stops[i + 1];
                }
            } else {
                for (int i = from_idx; i > to_idx; i--) {
                    stops[i] = stops[i - 1];
                }
            }
            stops[to_idx] = tmp;
        } else if (strcmp(cmd, "REMOVE") == 0) {
            scanf("%d", &index);
            if (index < 0 || index >= s_count) {
                continue;
            }
            free(stops[index].stop);
            free(stops[index].note);
            for (int i = index; i < s_count - 1; i++) {
                stops[i] = stops[i + 1];
            }
            s_count--;
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", stop);
            int idx = find_stop(stop);
            if (idx != -1) {
                printf("%s\n", stops[idx].note);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < s_count; i++) {
                printf("%s %s\n", stops[i].stop, stops[i].note);
            }
        }
    }

    for (int i = 0; i < s_count; i++) {
        free(stops[i].stop);
        free(stops[i].note);
    }
    free(stops);

    return 0;
}