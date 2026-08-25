// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *key; char *val; } Setting;
Setting *committed = NULL;
int cc = 0, ccap = 0;
Setting *staged = NULL;
int sc = 0, scap = 0;
int in_trans = 0;

int find_setting(Setting *arr, int count, const char *key) {
    for (int i = 0; i < count; i++)
        if (strcmp(arr[i].key, key) == 0) return i;
    return -1;
}

void set_committed(const char *key, const char *val) {
    int idx = find_setting(committed, cc, key);
    if (idx >= 0) {
        free(committed[idx].val);
        committed[idx].val = strdup(val);
    } else {
        if (cc == ccap) { ccap = ccap ? ccap * 2 : 4; committed = realloc(committed, sizeof(Setting) * ccap); }
        committed[cc].key = strdup(key);
        committed[cc].val = strdup(val);
        cc++;
    }
}

void set_staged(const char *key, const char *val) {
    int idx = find_setting(staged, sc, key);
    if (idx >= 0) {
        free(staged[idx].val);
        staged[idx].val = strdup(val);
    } else {
        if (sc == scap) { scap = scap ? scap * 2 : 4; staged = realloc(staged, sizeof(Setting) * scap); }
        staged[sc].key = strdup(key);
        staged[sc].val = strdup(val);
        sc++;
    }
}

void begin_transaction(void) {
    if (in_trans) return;
    in_trans = 1;
    for (int i = 0; i < sc; i++) { free(staged[i].key); free(staged[i].val); }
    sc = 0;
    // Copy committed to staged
    for (int i = 0; i < cc; i++) {
        if (sc == scap) { scap = scap ? scap * 2 : 4; staged = realloc(staged, sizeof(Setting) * scap); }
        staged[sc].key = strdup(committed[i].key);
        staged[sc].val = strdup(committed[i].val);
        sc++;
    }
}

void tset(const char *key, const char *val) {
    if (!in_trans) return;
    set_staged(key, val);
}

void commit_transaction(void) {
    if (!in_trans) return;
    // Clear committed
    for (int i = 0; i < cc; i++) { free(committed[i].key); free(committed[i].val); }
    cc = 0;
    // Copy staged to committed
    for (int i = 0; i < sc; i++) {
        if (cc == ccap) { ccap = ccap ? ccap * 2 : 4; committed = realloc(committed, sizeof(Setting) * ccap); }
        committed[cc].key = strdup(staged[i].key);
        committed[cc].val = strdup(staged[i].val);
        cc++;
    }
    // Clear staged
    for (int i = 0; i < sc; i++) { free(staged[i].key); free(staged[i].val); }
    sc = 0;
    in_trans = 0;
}

void abort_transaction(void) {
    if (!in_trans) return;
    for (int i = 0; i < sc; i++) { free(staged[i].key); free(staged[i].val); }
    sc = 0;
    in_trans = 0;
}

void get_setting(const char *key) {
    int idx = find_setting(committed, cc, key);
    if (idx >= 0) printf("%s\n", committed[idx].val);
    else printf("NOT_FOUND\n");
}

void dump_settings(void) {
    for (int i = 0; i < cc; i++)
        printf("%s %s\n", committed[i].key, committed[i].val);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "SET") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", arg1, arg2) == 2)
                set_committed(arg1, arg2);
        } else if (strcmp(cmd, "BEGIN") == 0) begin_transaction();
        else if (strcmp(cmd, "TSET") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", arg1, arg2) == 2)
                tset(arg1, arg2);
        } else if (strcmp(cmd, "COMMIT") == 0) commit_transaction();
        else if (strcmp(cmd, "ABORT") == 0) abort_transaction();
        else if (strcmp(cmd, "GET") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1) get_setting(arg1);
        } else if (strcmp(cmd, "DUMP") == 0) dump_settings();
    }
    for (int i = 0; i < cc; i++) { free(committed[i].key); free(committed[i].val); }
    for (int i = 0; i < sc; i++) { free(staged[i].key); free(staged[i].val); }
    free(committed);
    free(staged);
    return 0;
}