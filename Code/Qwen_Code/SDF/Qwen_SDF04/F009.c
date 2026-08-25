// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char* desc;
} Event;

typedef struct {
    char* name;
    Event* events;
    size_t e_count;
    size_t e_cap;
} Group;

Group* groups = NULL;
size_t g_count = 0;
size_t g_cap = 0;

void cleanup() {
    for (size_t i = 0; i < g_count; i++) {
        for (size_t j = 0; j < groups[i].e_count; j++) {
            free(groups[i].events[j].desc);
        }
        free(groups[i].events);
        free(groups[i].name);
    }
    free(groups);
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
        
        if (strcmp(cmd, "EVENT") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* group = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* desc = ptr;
            
            Group* g = NULL;
            for (size_t i = 0; i < g_count; i++) {
                if (strcmp(groups[i].name, group) == 0) {
                    g = &groups[i];
                    break;
                }
            }
            if (!g) {
                if (g_count == g_cap) {
                    size_t new_cap = g_cap == 0 ? 4 : g_cap * 2;
                    Group* temp = realloc(groups, new_cap * sizeof(Group));
                    if (!temp) { cleanup(); free(line); return 1; }
                    groups = temp;
                    g_cap = new_cap;
                }
                g = &groups[g_count];
                g->name = strdup(group);
                g->events = NULL;
                g->e_count = 0;
                g->e_cap = 0;
                if (!g->name) { cleanup(); free(line); return 1; }
                g_count++;
            }
            
            if (g->e_count == g->e_cap) {
                size_t new_cap = g->e_cap == 0 ? 4 : g->e_cap * 2;
                Event* temp = realloc(g->events, new_cap * sizeof(Event));
                if (!temp) { cleanup(); free(line); return 1; }
                g->events = temp;
                g->e_cap = new_cap;
            }
            g->events[g->e_count].desc = strdup(desc);
            if (!g->events[g->e_count].desc) { cleanup(); free(line); return 1; }
            g->e_count++;
        } else if (strcmp(cmd, "MERGE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* gA = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* gB = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* newG = ptr;
            
            Group* grpA = NULL;
            Group* grpB = NULL;
            for (size_t i = 0; i < g_count; i++) {
                if (strcmp(groups[i].name, gA) == 0) grpA = &groups[i];
                if (strcmp(groups[i].name, gB) == 0) grpB = &groups[i];
            }
            
            if (grpA && grpB) {
                Group* g = NULL;
                for (size_t i = 0; i < g_count; i++) {
                    if (strcmp(groups[i].name, newG) == 0) {
                        g = &groups[i];
                        break;
                    }
                }
                if (!g) {
                    if (g_count == g_cap) {
                        size_t new_cap = g_cap == 0 ? 4 : g_cap * 2;
                        Group* temp = realloc(groups, new_cap * sizeof(Group));
                        if (!temp) { cleanup(); free(line); return 1; }
                        groups = temp;
                        g_cap = new_cap;
                    }
                    g = &groups[g_count];
                    g->name = strdup(newG);
                    g->events = NULL;
                    g->e_count = 0;
                    g->e_cap = 0;
                    if (!g->name) { cleanup(); free(line); return 1; }
                    g_count++;
                }
                
                for (size_t i = 0; i < grpA->e_count; i++) {
                    if (g->e_count == g->e_cap) {
                        size_t new_cap = g->e_cap == 0 ? 4 : g->e_cap * 2;
                        Event* temp = realloc(g->events, new_cap * sizeof(Event));
                        if (!temp) { cleanup(); free(line); return 1; }
                        g->events = temp;
                        g->e_cap = new_cap;
                    }
                    g->events[g->e_count].desc = strdup(grpA->events[i].desc);
                    if (!g->events[g->e_count].desc) { cleanup(); free(line); return 1; }
                    g->e_count++;
                }
                for (size_t i = 0; i < grpB->e_count; i++) {
                    if (g->e_count == g->e_cap) {
                        size_t new_cap = g->e_cap == 0 ? 4 : g->e_cap * 2;
                        Event* temp = realloc(g->events, new_cap * sizeof(Event));
                        if (!temp) { cleanup(); free(line); return 1; }
                        g->events = temp;
                        g->e_cap = new_cap;
                    }
                    g->events[g->e_count].desc = strdup(grpB->events[i].desc);
                    if (!g->events[g->e_count].desc) { cleanup(); free(line); return 1; }
                    g->e_count++;
                }
            }
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* group = ptr;
            for (size_t i = 0; i < g_count; i++) {
                if (strcmp(groups[i].name, group) == 0) {
                    for (size_t j = 0; j < groups[i].e_count; j++) {
                        free(groups[i].events[j].desc);
                    }
                    free(groups[i].events);
                    free(groups[i].name);
                    for (size_t j = i; j < g_count - 1; j++) {
                        groups[j] = groups[j + 1];
                    }
                    g_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* group = ptr;
            for (size_t i = 0; i < g_count; i++) {
                if (strcmp(groups[i].name, group) == 0) {
                    printf("%zu\n", groups[i].e_count);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < g_count; i++) {
                printf("%s %zu\n", groups[i].name, groups[i].e_count);
            }
        }
    }
    
    free(line);
    cleanup();
    return 0;
}