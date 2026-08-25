// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *stop, *note; } Stop;
Stop *stops; int n_stop;

int find_stop(char *stop) {
    for(int i=0; i<n_stop; i++) if(!strcmp(stops[i].stop, stop)) return i;
    return -1;
}

int main() {
    char cmd[20], stop[100], note[1000]; int idx, from, to;
    while(scanf("%s", cmd)==1 && strcmp(cmd, "END")) {
        if(!strcmp(cmd, "ADD")) {
            scanf("%s", stop);
            fgets(note, 1000, stdin);
            while(note[0]==' ') memmove(note, note+1, strlen(note));
            note[strcspn(note, "\n")] = 0;
            stops = realloc(stops, (n_stop+1)*sizeof(Stop));
            stops[n_stop].stop = strdup(stop);
            stops[n_stop].note = strdup(note);
            n_stop++;
        } else if(!strcmp(cmd, "INSERT")) {
            scanf("%d %s", &idx, stop);
            fgets(note, 1000, stdin);
            while(note[0]==' ') memmove(note, note+1, strlen(note));
            note[strcspn(note, "\n")] = 0;
            if(idx >= 0 && idx <= n_stop) {
                stops = realloc(stops, (n_stop+1)*sizeof(Stop));
                for(int i=n_stop; i>idx; i--) stops[i] = stops[i-1];
                stops[idx].stop = strdup(stop);
                stops[idx].note = strdup(note);
                n_stop++;
            }
        } else if(!strcmp(cmd, "MOVE")) {
            scanf("%d %d", &from, &to);
            if(from >= 0 && from < n_stop && to >= 0 && to < n_stop) {
                Stop tmp = stops[from];
                if(from < to) {
                    for(int i=from; i<to; i++) stops[i] = stops[i+1];
                } else {
                    for(int i=from; i>to; i--) stops[i] = stops[i-1];
                }
                stops[to] = tmp;
            }
        } else if(!strcmp(cmd, "REMOVE")) {
            scanf("%d", &idx);
            if(idx >= 0 && idx < n_stop) {
                free(stops[idx].stop);
                free(stops[idx].note);
                for(int i=idx; i<n_stop-1; i++) stops[i] = stops[i+1];
                n_stop--;
            }
        } else if(!strcmp(cmd, "FIND")) {
            scanf("%s", stop);
            int i = find_stop(stop);
            if(i != -1) printf("%s\n", stops[i].note);
        } else if(!strcmp(cmd, "PRINT")) {
            for(int i=0; i<n_stop; i++) {
                printf("%s %s\n", stops[i].stop, stops[i].note);
            }
        }
    }
    return 0;
}