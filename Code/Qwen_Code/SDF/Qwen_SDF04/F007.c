// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    int time;
    char* label;
    char* note;
} Activity;

Activity* acts = NULL;
size_t a_count = 0;
size_t a_cap = 0;

void cleanup() {
    for (size_t i = 0; i < a_count; i++) {
        free(acts[i].label);
        free(acts[i].note);
    }
    free(acts);
}

int cmp_act(const void* a, const void* b) {
    const Activity* ia = (const Activity*)a;
    const Activity* ib = (const Activity*)b;
    if (ia->time != ib->time) return ia->time - ib->time;
    return strcmp(ia->label, ib->label);
}

int main() {
    char* line = NULL;
    size_t len = 0;
    
    while (getline(&line, &len, stdin) != -1) {
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        
        char* ptr = line;
        while (*ptr && isspace((unsigned char)*ptr)) ptr++;
        if (!*ptr) continue;
        
        char* cmd = ptr;
        while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
        if (*ptr) {
            *ptr = '\0';
            ptr++;
        }
        
        if (strcmp(cmd, "ADD") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* time_str = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* label = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* note = ptr;
            
            if (a_count == a_cap) {
                size_t new_cap = a_cap == 0 ? 4 : a_cap * 2;
                Activity* temp = realloc(acts, new_cap * sizeof(Activity));
                if (!temp) { cleanup(); free(line); return 1; }
                acts = temp;
                a_cap = new_cap;
            }
            acts[a_count].time = atoi(time_str);
            acts[a_count].label = strdup(label);
            acts[a_count].note = strdup(note);
            if (!acts[a_count].label || !acts[a_count].note) { cleanup(); free(line); return 1; }
            a_count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* time_str = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* label = ptr;
            
            int t = atoi(time_str);
            for (size_t i = 0; i < a_count; ) {
                if (acts[i].time == t && strcmp(acts[i].label, label) == 0) {
                    free(acts[i].label);
                    free(acts[i].note);
                    for (size_t j = i; j < a_count - 1; j++) {
                        acts[j] = acts[j + 1];
                    }
                    a_count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* label = ptr;
            int min_time = -1;
            size_t min_idx = 0;
            for (size_t i = 0; i < a_count; i++) {
                if (strcmp(acts[i].label, label) == 0) {
                    if (min_time == -1 || acts[i].time < min_time) {
                        min_time = acts[i].time;
                        min_idx = i;
                    }
                }
            }
            if (min_time != -1) {
                printf("%d %s %s\n", acts[min_idx].time, acts[min_idx].label, acts[min_idx].note);
            }
        } else if (strcmp(cmd, "SORT") == 0) {
            if (a_count > 0) {
                qsort(acts, a_count, sizeof(Activity), cmp_act);
            }
            for (size_t i = 0; i < a_count; i++) {
                printf("%d %s %s\n", acts[i].time, acts[i].label, acts[i].note);
            }
        } else if (strcmp(cmd, "CLEAR") == 0) {
            for (size_t i = 0; i < a_count; i++) {
                free(acts[i].label);
                free(acts[i].note);
            }
            a_count = 0;
        }
    }
    
    free(line);
    cleanup();
    return 0;
}