// F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct { char* name; char* payload; } Job;

Job* jobs = NULL;
size_t j_count = 0, j_cap = 0;

void cleanup() {
    for (size_t i = 0; i < j_count; i++) { free(jobs[i].name); free(jobs[i].payload); }
    free(jobs);
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
        if (*ptr) { *ptr = '\0'; ptr++; }

        if (strcmp(cmd, "ENQUEUE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* name = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* payload = ptr;

            if (j_count == j_cap) {
                size_t new_cap = j_cap == 0 ? 4 : j_cap * 2;
                Job* temp = realloc(jobs, new_cap * sizeof(Job));
                if (!temp) { cleanup(); free(line); return 1; }
                jobs = temp; j_cap = new_cap;
            }
            jobs[j_count].name = strdup(name);
            jobs[j_count].payload = strdup(payload);
            if (!jobs[j_count].name || !jobs[j_count].payload) { cleanup(); free(line); return 1; }
            j_count++;
        } else if (strcmp(cmd, "CLONE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* old_name = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* new_name = ptr;

            for (size_t i = 0; i < j_count; i++) {
                if (strcmp(jobs[i].name, old_name) == 0) {
                    if (j_count == j_cap) {
                        size_t new_cap = j_cap == 0 ? 4 : j_cap * 2;
                        Job* temp = realloc(jobs, new_cap * sizeof(Job));
                        if (!temp) { cleanup(); free(line); return 1; }
                        jobs = temp; j_cap = new_cap;
                    }
                    jobs[j_count].name = strdup(new_name);
                    jobs[j_count].payload = strdup(jobs[i].payload);
                    if (!jobs[j_count].name || !jobs[j_count].payload) { cleanup(); free(line); return 1; }
                    j_count++;
                    break;
                }
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            for (size_t i = 0; i < j_count; i++) {
                if (strcmp(jobs[i].name, ptr) == 0) {
                    free(jobs[i].name); free(jobs[i].payload);
                    for (size_t j = i; j < j_count - 1; j++) jobs[j] = jobs[j + 1];
                    j_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "RUN") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            for (size_t i = 0; i < j_count; i++) {
                if (strcmp(jobs[i].name, ptr) == 0) {
                    printf("%s\n", jobs[i].payload);
                    free(jobs[i].name); free(jobs[i].payload);
                    for (size_t j = i; j < j_count - 1; j++) jobs[j] = jobs[j + 1];
                    j_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < j_count; i++) {
                printf("%s %s\n", jobs[i].name, jobs[i].payload);
            }
        }
    }
    free(line);
    cleanup();
    return 0;
}