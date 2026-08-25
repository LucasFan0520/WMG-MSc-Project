/* F010.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char **values;
    int vcount;
    int vcap;
} ValList;

typedef struct {
    char *key;
    ValList vals;
} Entry;

static Entry *entries = NULL;
static int ecount = 0;
static int ecap = 0;

static int ensure_ecap(void) {
    if (ecount >= ecap) {
        int newcap = ecap == 0 ? 8 : ecap * 2;
        Entry *tmp = realloc(entries, sizeof(Entry) * newcap);
        if (!tmp) return -1;
        entries = tmp;
        ecap = newcap;
    }
    return 0;
}

static char *dup_str(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

static int find_entry(const char *key) {
    for (int i = 0; i < ecount; i++) {
        if (strcmp(entries[i].key, key) == 0)
            return i;
    }
    return -1;
}

static int vallist_add(ValList *vl, const char *val) {
    if (vl->vcount >= vl->vcap) {
        int newcap = vl->vcap == 0 ? 4 : vl->vcap * 2;
        char **tmp = realloc(vl->values, sizeof(char *) * newcap);
        if (!tmp) return -1;
        vl->values = tmp;
        vl->vcap = newcap;
    }
    vl->values[vl->vcount] = dup_str(val);
    if (!vl->values[vl->vcount]) return -1;
    vl->vcount++;
    return 0;
}

static void vallist_free(ValList *vl) {
    for (int i = 0; i < vl->vcount; i++)
        free(vl->values[i]);
    free(vl->values);
    vl->values = NULL;
    vl->vcount = 0;
    vl->vcap = 0;
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

        if (strcmp(cmd, "ADD") == 0) {
            char key[256] = {0}, val[256] = {0};
            if (sscanf(line + offset, " %255s %255s", key, val) != 2) continue;
            int idx = find_entry(key);
            if (idx < 0) {
                if (ensure_ecap() != 0) continue;
                entries[ecount].key = dup_str(key);
                if (!entries[ecount].key) continue;
                entries[ecount].vals.values = NULL;
                entries[ecount].vals.vcount = 0;
                entries[ecount].vals.vcap = 0;
                idx = ecount;
                ecount++;
            }
            vallist_add(&entries[idx].vals, val);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char key[256] = {0}, val[256] = {0};
            if (sscanf(line + offset, " %255s %255s", key, val) != 2) continue;
            int idx = find_entry(key);
            if (idx < 0) continue;
            for (int i = 0; i < entries[idx].vals.vcount; i++) {
                if (strcmp(entries[idx].vals.values[i], val) == 0) {
                    free(entries[idx].vals.values[i]);
                    for (int j = i; j < entries[idx].vals.vcount - 1; j++)
                        entries[idx].vals.values[j] = entries[idx].vals.values[j + 1];
                    entries[idx].vals.vcount--;
                    break;
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char key[256] = {0};
            if (sscanf(line + offset, " %255s", key) != 1) continue;
            int idx = find_entry(key);
            if (idx < 0) continue;
            free(entries[idx].key);
            vallist_free(&entries[idx].vals);
            for (int i = idx; i < ecount - 1; i++)
                entries[i] = entries[i + 1];
            ecount--;
        } else if (strcmp(cmd, "VALUES") == 0) {
            char key[256] = {0};
            if (sscanf(line + offset, " %255s", key) != 1) continue;
            int idx = find_entry(key);
            if (idx >= 0) {
                for (int i = 0; i < entries[idx].vals.vcount; i++) {
                    printf("%s\n", entries[idx].vals.values[i]);
                }
            }
        } else if (strcmp(cmd, "KEYS") == 0) {
            for (int i = 0; i < ecount; i++) {
                printf("%s\n", entries[i].key);
            }
        }
    }
    for (int i = 0; i < ecount; i++) {
        free(entries[i].key);
        vallist_free(&entries[i].vals);
    }
    free(entries);
    return 0;
}