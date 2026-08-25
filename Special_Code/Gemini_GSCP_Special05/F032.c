// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else {
        buf[len] = '\0';
    }
    return buf;
}

char *safe_dup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) {
        strcpy(d, s);
    }
    return d;
}

typedef struct {
    char *key;
    char *value;
} Setting;

int main(void) {
    Setting *main_store = NULL;
    size_t m_count = 0;
    size_t m_cap = 0;
    Setting *stage_store = NULL;
    size_t s_count = 0;
    size_t s_cap = 0;
    int in_tx = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "SET ", 4) == 0) {
            char *p = line + 4;
            char *key = p;
            while (*p && *p != ' ') p++;
            char *value = "";
            if (*p == ' ') {
                *p = '\0';
                value = p + 1;
            }
            size_t found = (size_t)-1;
            for (size_t i = 0; i < m_count; i++) {
                if (strcmp(main_store[i].key, key) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != (size_t)-1) {
                free(main_store[found].value);
                main_store[found].value = safe_dup(value);
            } else {
                if (m_count >= m_cap) {
                    m_cap = m_cap == 0 ? 4 : m_cap * 2;
                    Setting *nb = realloc(main_store, m_cap * sizeof(Setting));
                    if (!nb) {
                        free(line);
                        break;
                    }
                    main_store = nb;
                }
                main_store[m_count].key = safe_dup(key);
                main_store[m_count].value = safe_dup(value);
                m_count++;
            }
        } else if (strcmp(line, "BEGIN") == 0) {
            in_tx = 1;
            for (size_t i = 0; i < s_count; i++) {
                free(stage_store[i].key);
                free(stage_store[i].value);
            }
            s_count = 0;
        } else if (strncmp(line, "TSET ", 5) == 0) {
            if (in_tx) {
                char *p = line + 5;
                char *key = p;
                while (*p && *p != ' ') p++;
                char *value = "";
                if (*p == ' ') {
                    *p = '\0';
                    value = p + 1;
                }
                size_t found = (size_t)-1;
                for (size_t i = 0; i < s_count; i++) {
                    if (strcmp(stage_store[i].key, key) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != (size_t)-1) {
                    free(stage_store[found].value);
                    stage_store[found].value = safe_dup(value);
                } else {
                    if (s_count >= s_cap) {
                        s_cap = s_cap == 0 ? 4 : s_cap * 2;
                        Setting *nb = realloc(stage_store, s_cap * sizeof(Setting));
                        if (!nb) {
                            free(line);
                            break;
                        }
                        stage_store = nb;
                    }
                    stage_store[s_count].key = safe_dup(key);
                    stage_store[s_count].value = safe_dup(value);
                    s_count++;
                }
            }
        } else if (strcmp(line, "COMMIT") == 0) {
            if (in_tx) {
                for (size_t i = 0; i < s_count; i++) {
                    size_t found = (size_t)-1;
                    for (size_t j = 0; j < m_count; j++) {
                        if (strcmp(main_store[j].key, stage_store[i].key) == 0) {
                            found = j;
                            break;
                        }
                    }
                    if (found != (size_t)-1) {
                        free(main_store[found].value);
                        main_store[found].value = stage_store[i].value;
                        free(stage_store[i].key);
                    } else {
                        if (m_count >= m_cap) {
                            m_cap = m_cap == 0 ? 4 : m_cap * 2;
                            Setting *nb = realloc(main_store, m_cap * sizeof(Setting));
                            if (!nb) break;
                            main_store = nb;
                        }
                        main_store[m_count].key = stage_store[i].key;
                        main_store[m_count].value = stage_store[i].value;
                        m_count++;
                    }
                }
                s_count = 0;
                in_tx = 0;
            }
        } else if (strcmp(line, "ABORT") == 0) {
            if (in_tx) {
                for (size_t i = 0; i < s_count; i++) {
                    free(stage_store[i].key);
                    free(stage_store[i].value);
                }
                s_count = 0;
                in_tx = 0;
            }
        } else if (strncmp(line, "GET ", 4) == 0) {
            char *key = line + 4;
            int found = 0;
            if (in_tx) {
                for (size_t i = 0; i < s_count; i++) {
                    if (strcmp(stage_store[i].key, key) == 0) {
                        printf("%s\n", stage_store[i].value);
                        found = 1;
                        break;
                    }
                }
            }
            if (!found) {
                for (size_t i = 0; i < m_count; i++) {
                    if (strcmp(main_store[i].key, key) == 0) {
                        printf("%s\n", main_store[i].value);
                        found = 1;
                        break;
                    }
                }
            }
        } else if (strcmp(line, "DUMP") == 0) {
            for (size_t i = 0; i < m_count; i++) {
                printf("%s=%s\n", main_store[i].key, main_store[i].value);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < m_count; i++) {
        free(main_store[i].key);
        free(main_store[i].value);
    }
    free(main_store);
    for (size_t i = 0; i < s_count; i++) {
        free(stage_store[i].key);
        free(stage_store[i].value);
    }
    free(stage_store);
    return 0;
}
