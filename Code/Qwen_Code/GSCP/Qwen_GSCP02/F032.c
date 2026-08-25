/* F032.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Setting;

static Setting *settings = NULL;
static int scount = 0;
static int scap = 0;

static Setting *staged = NULL;
static int stcount = 0;
static int stcap = 0;
static int in_transaction = 0;

static char *dup_str(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

static int find_setting(const char *key) {
    for (int i = 0; i < scount; i++)
        if (strcmp(settings[i].key, key) == 0) return i;
    return -1;
}

static int find_staged(const char *key) {
    for (int i = 0; i < stcount; i++)
        if (strcmp(staged[i].key, key) == 0) return i;
    return -1;
}

static void apply_staged(void) {
    for (int i = 0; i < stcount; i++) {
        int idx = find_setting(staged[i].key);
        if (idx >= 0) {
            free(settings[idx].value);
            settings[idx].value = staged[i].value;
            free(staged[i].key);
        } else {
            if (scount >= scap) {
                scap = scap == 0 ? 8 : scap * 2;
                settings = realloc(settings, sizeof(Setting) * scap);
            }
            settings[scount++] = staged[i];
        }
    }
    free(staged);
    staged = NULL;
    stcount = 0;
    stcap = 0;
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[64] = {0};
        int offset = 0;
        if (sscanf(line, "%63s%n", cmd, &offset) != 1) continue;

        if (strcmp(cmd, "SET") == 0) {
            char key[256] = {0};
            int off2 = 0;
            if (sscanf(line + offset, "%255s%n", key, &off2) != 1) continue;
            const char *val = line + offset + off2;
            while (*val == ' ') val++;
            int idx = find_setting(key);
            if (idx >= 0) {
                free(settings[idx].value);
                settings[idx].value = dup_str(val);
            } else {
                if (scount >= scap) {
                    scap = scap == 0 ? 8 : scap * 2;
                    settings = realloc(settings, sizeof(Setting) * scap);
                }
                settings[scount].key = dup_str(key);
                settings[scount].value = dup_str(val);
                scount++;
            }
        } else if (strcmp(cmd, "BEGIN") == 0) {
            in_transaction = 1;
            staged = NULL;
            stcount = 0;
            stcap = 0;
        } else if (strcmp(cmd, "TSET") == 0) {
            if (!in_transaction) continue;
            char key[256] = {0};
            int off2 = 0;
            if (sscanf(line + offset, "%255s%n", key, &off2) != 1) continue;
            const char *val = line + offset + off2;
            while (*val == ' ') val++;
            int idx = find_staged(key);
            if (idx >= 0) {
                free(staged[idx].value);
                staged[idx].value = dup_str(val);
            } else {
                if (stcount >= stcap) {
                    stcap = stcap == 0 ? 4 : stcap * 2;
                    staged = realloc(staged, sizeof(Setting) * stcap);
                }
                staged[stcount].key = dup_str(key);
                staged[stcount].value = dup_str(val);
                stcount++;
            }
        } else if (strcmp(cmd, "COMMIT") == 0) {
            if (!in_transaction) continue;
            apply_staged();
            in_transaction = 0;
        } else if (strcmp(cmd, "ABORT") == 0) {
            if (!in_transaction) continue;
            for (int i = 0; i < stcount; i++) {
                free(staged[i].key);
                free(staged[i].value);
            }
            free(staged);
            staged = NULL;
            stcount = 0;
            stcap = 0;
            in_transaction = 0;
        } else if (strcmp(cmd, "GET") == 0) {
            char key[256] = {0};
            if (sscanf(line + offset, "%255s", key) != 1) continue;
            int idx = find_setting(key);
            if (idx >= 0) printf("%s\n", settings[idx].value);
            else printf("NOT_FOUND\n");
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (int i = 0; i < scount; i++) {
                printf("%s=%s\n", settings[i].key, settings[i].value);
            }
        }
    }
    if (in_transaction) {
        for (int i = 0; i < stcount; i++) {
            free(staged[i].key);
            free(staged[i].value);
        }
        free(staged);
    }
    for (int i = 0; i < scount; i++) {
        free(settings[i].key);
        free(settings[i].value);
    }
    free(settings);
    return 0;
}