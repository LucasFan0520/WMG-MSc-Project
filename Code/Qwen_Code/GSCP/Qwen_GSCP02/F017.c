/* F017.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} KVPair;

static KVPair *pairs = NULL;
static int pcount = 0;
static int pcap = 0;

static int ensure_cap(void) {
    if (pcount >= pcap) {
        int newcap = pcap == 0 ? 8 : pcap * 2;
        KVPair *tmp = realloc(pairs, sizeof(KVPair) * newcap);
        if (!tmp) return -1;
        pairs = tmp;
        pcap = newcap;
    }
    return 0;
}

static char *dup_str(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

static int find_pair(const char *key) {
    for (int i = 0; i < pcount; i++) {
        if (strcmp(pairs[i].key, key) == 0)
            return i;
    }
    return -1;
}

static void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        if (len > 1 && line[len - 2] == '\r') line[len - 2] = '\0';
        char *eq = strchr(line, '=');
        if (!eq) continue;
        *eq = '\0';
        const char *key = line;
        const char *val = eq + 1;
        if (ensure_cap() != 0) continue;
        pairs[pcount].key = dup_str(key);
        pairs[pcount].value = dup_str(val);
        if (!pairs[pcount].key || !pairs[pcount].value) {
            free(pairs[pcount].key);
            free(pairs[pcount].value);
            continue;
        }
        pcount++;
    }
    fclose(f);
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        load_file(argv[1]);
    }
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
            if (sscanf(line + offset, " %255s%n", key, &off2) != 1) continue;
            const char *val = line + offset + off2;
            while (*val == ' ') val++;
            int idx = find_pair(key);
            if (idx >= 0) {
                char *nv = dup_str(val);
                if (!nv) continue;
                free(pairs[idx].value);
                pairs[idx].value = nv;
            } else {
                if (ensure_cap() != 0) continue;
                pairs[pcount].key = dup_str(key);
                pairs[pcount].value = dup_str(val);
                if (!pairs[pcount].key || !pairs[pcount].value) {
                    free(pairs[pcount].key);
                    free(pairs[pcount].value);
                    continue;
                }
                pcount++;
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char key[256] = {0};
            if (sscanf(line + offset, " %255s", key) != 1) continue;
            int idx = find_pair(key);
            if (idx < 0) continue;
            free(pairs[idx].key);
            free(pairs[idx].value);
            for (int i = idx; i < pcount - 1; i++)
                pairs[i] = pairs[i + 1];
            pcount--;
        } else if (strcmp(cmd, "GET") == 0) {
            char key[256] = {0};
            if (sscanf(line + offset, " %255s", key) != 1) continue;
            int idx = find_pair(key);
            if (idx >= 0) {
                printf("%s\n", pairs[idx].value);
            } else {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(cmd, "WRITE") == 0) {
            for (int i = 0; i < pcount; i++) {
                printf("%s=%s\n", pairs[i].key, pairs[i].value);
            }
        }
    }
    for (int i = 0; i < pcount; i++) {
        free(pairs[i].key);
        free(pairs[i].value);
    }
    free(pairs);
    return 0;
}