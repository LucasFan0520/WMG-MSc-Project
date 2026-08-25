// F032.c
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
    int in_tx = 0;
    char *line = NULL;
    size_t cap = 0;
    while (getline(&line, &cap, stdin) != -1) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "SET ", 4) == 0) {
            char *key = line + 4;
            while (*key == ' ') key++;
            char *val = strchr(key, ' ');
            if (val) {
                *val = '\0';
                val++;
                while (*val == ' ') val++;
            }
            if (!val) val = "";
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
                Setting *tmp = realloc(committed, (c_count + 1) * sizeof(Setting));
                if (tmp) {
                    committed = tmp;
                    committed[c_count].key = strdup(key);
                    committed[c_count].value = strdup(val);
                    c_count++;
                }
            }
        } else if (strcmp(line, "BEGIN") == 0) {
            for (int i = 0; i < s_count; i++) {
                free(staged[i].key);
                free(staged[i].value);
            }
            free(staged);
            staged = NULL;
            s_count = 0;
            in_tx = 1;
        } else if (strncmp(line, "TSET ", 5) == 0) {
            if (in_tx) {
                char *key = line + 5;
                while (*key == ' ') key++;
                char *val = strchr(key, ' ');
                if (val) {
                    *val = '\0';
                    val++;
                    while (*val == ' ') val++;
                }
                if (!val) val = "";
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
                    Setting *tmp = realloc(staged, (s_count + 1) * sizeof(Setting));
                    if (tmp) {
                        staged = tmp;
                        staged[s_count].key = strdup(key);
                        staged[s_count].value = strdup(val);
                        s_count++;
                    }
                }
            }
        } else if (strcmp(line, "COMMIT") == 0) {
            if (in_tx) {
                for (int i = 0; i < s_count; i++) {
                    int found = -1;
                    for (int j = 0; j < c_count; j++) {
                        if (strcmp(committed[j].key, staged[i].key) == 0) {
                            found = j;
                            break;
                        }
                    }
                    if (found != -1) {
                        char *nv = strdup(staged[i].value);
                        if (nv) {
                            free(committed[found].value);
                            committed[found].value = nv;
                        }
                    } else {
                        Setting *tmp = realloc(committed, (c_count + 1) * sizeof(Setting));
                        if (tmp) {
                            committed = tmp;
                            committed[c_count].key = strdup(staged[i].key);
                            committed[c_count].value = strdup(staged[i].value);
                            c_count++;
                        }
                    }
                }
                for (int i = 0; i < s_count; i++) {
                    free(staged[i].key);
                    free(staged[i].value);
                }
                free(staged);
                staged = NULL;
                s_count = 0;
                in_tx = 0;
            }
        } else if (strcmp(line, "ABORT") == 0) {
            if (in_tx) {
                for (int i = 0; i < s_count; i++) {
                    free(staged[i].key);
                    free(staged[i].value);
                }
                free(staged);
                staged = NULL;
                s_count = 0;
                in_tx = 0;
            }
        } else if (strncmp(line, "GET ", 4) == 0) {
            char *key = line + 4;
            while (*key == ' ') key++;
            char *val = NULL;
            if (in_tx) {
                for (int i = 0; i < s_count; i++) {
                    if (strcmp(staged[i].key, key) == 0) {
                        val = staged[i].value;
                        break;
                    }
                }
            }
            if (!val) {
                for (int i = 0; i < c_count; i++) {
                    if (strcmp(committed[i].key, key) == 0) {
                        val = committed[i].value;
                        break;
                    }
                }
            }
            if (val) {
                printf("%s\n", val);
            } else {
                printf("NULL\n");
            }
        } else if (strcmp(line, "DUMP") == 0) {
            for (int i = 0; i < c_count; i++) {
                printf("%s=%s\n", committed[i].key, committed[i].value);
            }
        }
    }
    free(line);
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
    return 0;
}
