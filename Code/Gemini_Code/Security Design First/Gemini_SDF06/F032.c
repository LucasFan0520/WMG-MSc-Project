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
    size_t c_count = 0;
    size_t c_capacity = 0;

    Setting *staged = NULL;
    size_t s_count = 0;
    size_t s_capacity = 0;

    int in_tx = 0;

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') continue;
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "SET") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *key = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *val = p;

            if (*key == '\0') continue;

            int found_idx = -1;
            for (size_t i = 0; i < c_count; i++) {
                if (strcmp(committed[i].key, key) == 0) {
                    found_idx = (int)i;
                    break;
                }
            }
            char *v = strdup(val);
            if (v) {
                if (found_idx != -1) {
                    free(committed[found_idx].value);
                    committed[found_idx].value = v;
                } else {
                    if (c_count >= c_capacity) {
                        size_t new_cap = c_capacity == 0 ? 4 : c_capacity * 2;
                        Setting *new_c = realloc(committed, new_cap * sizeof(Setting));
                        if (new_c) {
                            committed = new_c;
                            c_capacity = new_cap;
                        } else {
                            free(v);
                            continue;
                        }
                    }
                    char *k = strdup(key);
                    if (k) {
                        committed[c_count].key = k;
                        committed[c_count].value = v;
                        c_count++;
                    } else {
                        free(v);
                    }
                }
            }
        } else if (strcmp(cmd, "BEGIN") == 0) {
            in_tx = 1;
            for (size_t i = 0; i < s_count; i++) {
                free(staged[i].key);
                free(staged[i].value);
            }
            s_count = 0;
        } else if (strcmp(cmd, "TSET") == 0) {
            if (!in_tx) continue;
            while (*p == ' ' || *p == '\t') p++;
            char *key = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *val = p;

            if (*key == '\0') continue;

            int found_idx = -1;
            for (size_t i = 0; i < s_count; i++) {
                if (strcmp(staged[i].key, key) == 0) {
                    found_idx = (int)i;
                    break;
                }
            }
            char *v = strdup(val);
            if (v) {
                if (found_idx != -1) {
                    free(staged[found_idx].value);
                    staged[found_idx].value = v;
                } else {
                    if (s_count >= s_capacity) {
                        size_t new_cap = s_capacity == 0 ? 4 : s_capacity * 2;
                        Setting *new_s = realloc(staged, new_cap * sizeof(Setting));
                        if (new_s) {
                            staged = new_s;
                            s_capacity = new_cap;
                        } else {
                            free(v);
                            continue;
                        }
                    }
                    char *k = strdup(key);
                    if (k) {
                        staged[s_count].key = k;
                        staged[s_count].value = v;
                        s_count++;
                    } else {
                        free(v);
                    }
                }
            }
        } else if (strcmp(cmd, "COMMIT") == 0) {
            if (!in_tx) continue;
            for (size_t i = 0; i < s_count; i++) {
                int found_idx = -1;
                for (size_t j = 0; j < c_count; j++) {
                    if (strcmp(committed[j].key, staged[i].key) == 0) {
                        found_idx = (int)j;
                        break;
                    }
                }
                char *v = strdup(staged[i].value);
                if (v) {
                    if (found_idx != -1) {
                        free(committed[found_idx].value);
                        committed[found_idx].value = v;
                    } else {
                        if (c_count >= c_capacity) {
                            size_t new_cap = c_capacity == 0 ? 4 : c_capacity * 2;
                            Setting *new_c = realloc(committed, new_cap * sizeof(Setting));
                            if (new_c) {
                                committed = new_c;
                                c_capacity = new_cap;
                            } else {
                                free(v);
                                continue;
                            }
                        }
                        char *k = strdup(staged[i].key);
                        if (k) {
                            committed[c_count].key = k;
                            committed[c_count].value = v;
                            c_count++;
                        } else {
                            free(v);
                        }
                    }
                }
            }
            for (size_t i = 0; i < s_count; i++) {
                free(staged[i].key);
                free(staged[i].value);
            }
            s_count = 0;
            in_tx = 0;
        } else if (strcmp(cmd, "ABORT") == 0) {
            if (!in_tx) continue;
            for (size_t i = 0; i < s_count; i++) {
                free(staged[i].key);
                free(staged[i].value);
            }
            s_count = 0;
            in_tx = 0;
        } else if (strcmp(cmd, "GET") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *key = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*key == '\0') continue;

            int found = 0;
            if (in_tx) {
                for (size_t i = 0; i < s_count; i++) {
                    if (strcmp(staged[i].key, key) == 0) {
                        printf("%s\n", staged[i].value);
                        found = 1;
                        break;
                    }
                }
            }
            if (!found) {
                for (size_t i = 0; i < c_count; i++) {
                    if (strcmp(committed[i].key, key) == 0) {
                        printf("%s\n", committed[i].value);
                        found = 1;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (size_t i = 0; i < c_count; i++) {
                printf("%s=%s\n", committed[i].key, committed[i].value);
            }
        }
    }

    for (size_t i = 0; i < c_count; i++) {
        free(committed[i].key);
        free(committed[i].value);
    }
    free(committed);
    for (size_t i = 0; i < s_count; i++) {
        free(staged[i].key);
        free(staged[i].value);
    }
    free(staged);
    free(line);
    return 0;
}
