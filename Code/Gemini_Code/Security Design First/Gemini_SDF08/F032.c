// F032.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Setting;

int main(void) {
    Setting *committed = NULL;
    int c_count = 0;
    Setting *staged = NULL;
    int s_count = 0;
    int in_trans = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "SET ", 4) == 0) {
            char *p = line + 4;
            char *space = strchr(p, ' ');
            if (space) {
                *space = '\0';
                char *key = p;
                char *val = space + 1;
                int found = -1;
                for (int i = 0; i < c_count; i++) {
                    if (strcmp(committed[i].key, key) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    char *nv = strdup(val);
                    if (nv) {
                        free(committed[found].value);
                        committed[found].value = nv;
                    }
                } else {
                    Setting *tmp = realloc(committed, sizeof(Setting) * (c_count + 1));
                    if (tmp) {
                        committed = tmp;
                        committed[c_count].key = strdup(key);
                        committed[c_count].value = strdup(val);
                        if (committed[c_count].key && committed[c_count].value) {
                            c_count++;
                        }
                    }
                }
            }
        } else if (strcmp(line, "BEGIN") == 0) {
            in_trans = 1;
            for (int i = 0; i < s_count; i++) {
                free(staged[i].key);
                free(staged[i].value);
            }
            free(staged);
            staged = NULL;
            s_count = 0;
        } else if (strncmp(line, "TSET ", 5) == 0) {
            if (in_trans) {
                char *p = line + 5;
                char *space = strchr(p, ' ');
                if (space) {
                    *space = '\0';
                    char *key = p;
                    char *val = space + 1;
                    int found = -1;
                    for (int i = 0; i < s_count; i++) {
                        if (strcmp(staged[i].key, key) == 0) {
                            found = i;
                            break;
                        }
                    }
                    if (found != -1) {
                        char *nv = strdup(val);
                        if (nv) {
                            free(staged[found].value);
                            staged[found].value = nv;
                        }
                    } else {
                        Setting *tmp = realloc(staged, sizeof(Setting) * (s_count + 1));
                        if (tmp) {
                            staged = tmp;
                            staged[s_count].key = strdup(key);
                            staged[s_count].value = strdup(val);
                            if (staged[s_count].key && staged[s_count].value) {
                                s_count++;
                            }
                        }
                    }
                }
            }
        } else if (strcmp(line, "COMMIT") == 0) {
            if (in_trans) {
                for (int i = 0; i < s_count; i++) {
                    int found = -1;
                    for (int j = 0; j < c_count; j++) {
                        if (strcmp(committed[j].key, staged[i].key) == 0) {
                            found = j;
                            break;
                        }
                    }
                    if (found != -1) {
                        free(committed[found].value);
                        committed[found].value = staged[i].value;
                        free(staged[i].key);
                    } else {
                        Setting *tmp = realloc(committed, sizeof(Setting) * (c_count + 1));
                        if (tmp) {
                            committed = tmp;
                            committed[c_count] = staged[i];
                            c_count++;
                        } else {
                            free(staged[i].key);
                            free(staged[i].value);
                        }
                    }
                }
                free(staged);
                staged = NULL;
                s_count = 0;
                in_trans = 0;
            }
        } else if (strcmp(line, "ABORT") == 0) {
            if (in_trans) {
                for (int i = 0; i < s_count; i++) {
                    free(staged[i].key);
                    free(staged[i].value);
                }
                free(staged);
                staged = NULL;
                s_count = 0;
                in_trans = 0;
            }
        } else if (strncmp(line, "GET ", 4) == 0) {
            char *key = line + 4;
            int found = -1;
            for (int i = 0; i < c_count; i++) {
                if (strcmp(committed[i].key, key) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                printf("%s\n", committed[found].value);
            }
        } else if (strcmp(line, "DUMP") == 0) {
            for (int i = 0; i < c_count; i++) {
                printf("%s=%s\n", committed[i].key, committed[i].value);
            }
        }
    }
    for (int i = 0; i < c_count; i++) {
        free(committed[i].key);
        free(committed[i].value);
    }
    free(committed);
    for (int i = 0; i < s_count; i++) {
        free(staged[i].key);
        free(staged[i].value);
    }
    free(staged);
    free(line);
    return 0;
}
