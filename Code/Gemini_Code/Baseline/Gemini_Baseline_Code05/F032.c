// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Setting;

Setting *main_store = NULL;
int m_count = 0;
int m_capacity = 0;

Setting *stage_store = NULL;
int s_count = 0;
int s_capacity = 0;

int in_transaction = 0;

int find_setting(Setting *store, int cnt, const char *key) {
    for (int i = 0; i < cnt; i++) {
        if (strcmp(store[i].key, key) == 0) {
            return i;
        }
    }
    return -1;
}

void set_main(const char *key, const char *value) {
    int idx = find_setting(main_store, m_count, key);
    if (idx != -1) {
        free(main_store[idx].value);
        main_store[idx].value = strdup(value);
    } else {
        if (m_count >= m_capacity) {
            m_capacity = m_capacity == 0 ? 4 : m_capacity * 2;
            main_store = realloc(main_store, m_capacity * sizeof(Setting));
        }
        main_store[m_count].key = strdup(key);
        main_store[m_count].value = strdup(value);
        m_count++;
    }
}

void set_stage(const char *key, const char *value) {
    int idx = find_setting(stage_store, s_count, key);
    if (idx != -1) {
        free(stage_store[idx].value);
        stage_store[idx].value = strdup(value);
    } else {
        if (s_count >= s_capacity) {
            s_capacity = s_capacity == 0 ? 4 : s_capacity * 2;
            stage_store = realloc(stage_store, s_capacity * sizeof(Setting));
        }
        stage_store[s_count].key = strdup(key);
        stage_store[s_count].value = strdup(value);
        s_count++;
    }
}

void clear_stage() {
    for (int i = 0; i < s_count; i++) {
        free(stage_store[i].key);
        free(stage_store[i].value);
    }
    s_count = 0;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "SET ", 4) == 0) {
            char *p1 = line + 4;
            char *sp = strchr(p1, ' ');
            if (sp) {
                *sp = 0;
                set_main(p1, sp + 1);
            }
        } else if (strcmp(line, "BEGIN") == 0) {
            clear_stage();
            in_transaction = 1;
        } else if (strncmp(line, "TSET ", 5) == 0) {
            if (in_transaction) {
                char *p1 = line + 5;
                char *sp = strchr(p1, ' ');
                if (sp) {
                    *sp = 0;
                    set_stage(p1, sp + 1);
                }
            }
        } else if (strcmp(line, "COMMIT") == 0) {
            if (in_transaction) {
                for (int i = 0; i < s_count; i++) {
                    set_main(stage_store[i].key, stage_store[i].value);
                }
                clear_stage();
                in_transaction = 0;
            }
        } else if (strcmp(line, "ABORT") == 0) {
            if (in_transaction) {
                clear_stage();
                in_transaction = 0;
            }
        } else if (strncmp(line, "GET ", 4) == 0) {
            char *key = line + 4;
            int idx = find_setting(main_store, m_count, key);
            if (idx != -1) {
                printf("%s\n", main_store[idx].value);
            }
        } else if (strcmp(line, "DUMP") == 0) {
            for (int i = 0; i < m_count; i++) {
                printf("%s=%s\n", main_store[i].key, main_store[i].value);
            }
        }
    }
    for (int i = 0; i < m_count; i++) {
        free(main_store[i].key);
        free(main_store[i].value);
    }
    free(main_store);
    clear_stage();
    free(stage_store);
    return 0;
}
