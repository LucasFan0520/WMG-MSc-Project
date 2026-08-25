// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct {
    char *id;
    int hlen;
    int plen;
    char *header;
    char *payload;
} Packet;

typedef struct {
    Packet *items;
    size_t count;
    size_t capacity;
} Cache;

static void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len && s[len-1] == '\n') s[len-1] = '\0';
}

static char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *p = malloc(len + 1);
    if (p) memcpy(p, s, len + 1);
    return p;
}

static void packet_free(Packet *p) {
    free(p->id);
    free(p->header);
    free(p->payload);
}

static void cache_init(Cache *c) {
    c->items = NULL;
    c->count = 0;
    c->capacity = 0;
}

static void cache_free(Cache *c) {
    for (size_t i = 0; i < c->count; i++)
        packet_free(&c->items[i]);
    free(c->items);
    c->items = NULL;
    c->count = 0;
    c->capacity = 0;
}

static int cache_find(Cache *c, const char *id) {
    for (size_t i = 0; i < c->count; i++) {
        if (strcmp(c->items[i].id, id) == 0)
            return (int)i;
    }
    return -1;
}

static void cache_packet(Cache *c, const char *id, int hlen, int plen, const char *header, const char *payload) {
    if (cache_find(c, id) >= 0) return;
    if (c->count == c->capacity) {
        size_t newcap = c->capacity ? c->capacity * 2 : 4;
        Packet *tmp = realloc(c->items, newcap * sizeof(Packet));
        if (!tmp) return;
        c->items = tmp;
        c->capacity = newcap;
    }
    Packet *p = &c->items[c->count];
    p->id = safe_strdup(id);
    p->hlen = hlen;
    p->plen = plen;
    p->header = safe_strdup(header);
    p->payload = safe_strdup(payload);
    if (p->id && p->header && p->payload)
        c->count++;
    else {
        free(p->id); free(p->header); free(p->payload);
    }
}

static void cache_drop(Cache *c, const char *id) {
    int idx = cache_find(c, id);
    if (idx < 0) return;
    packet_free(&c->items[idx]);
    for (size_t i = idx; i < c->count - 1; i++)
        c->items[i] = c->items[i+1];
    c->count--;
}

static void cache_header(Cache *c, const char *id) {
    int idx = cache_find(c, id);
    if (idx < 0) return;
    printf("%s\n", c->items[idx].header);
}

static void cache_payload(Cache *c, const char *id) {
    int idx = cache_find(c, id);
    if (idx < 0) return;
    printf("%s\n", c->items[idx].payload);
}

static void cache_stats(Cache *c) {
    long long total = 0;
    for (size_t i = 0; i < c->count; i++)
        total += c->items[i].plen;
    printf("%lld\n", total);
}

static void parse_cmd(const char *line, char *cmd, size_t cmdsz,
                      char *a1, size_t a1sz, char *a2, size_t a2sz, char *a3, size_t a3sz, char *a4, size_t a4sz) {
    const char *p = line;
    while (isspace(*p)) p++;
    size_t i = 0;
    while (*p && !isspace(*p) && i < cmdsz - 1) cmd[i++] = *p++;
    cmd[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && !isspace(*p) && i < a1sz - 1) a1[i++] = *p++;
    a1[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && !isspace(*p) && i < a2sz - 1) a2[i++] = *p++;
    a2[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && !isspace(*p) && i < a3sz - 1) a3[i++] = *p++;
    a3[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && i < a4sz - 1) a4[i++] = *p++;
    a4[i] = '\0';
}

int main(void) {
    Cache c;
    cache_init(&c);
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[64], a3[64], a4[4096];
        parse_cmd(line, cmd, sizeof(cmd), a1, sizeof(a1), a2, sizeof(a2), a3, sizeof(a3), a4, sizeof(a4));
        if (strcmp(cmd, "PACKET") == 0) {
            int hlen = atoi(a2);
            int plen = atoi(a3);
            // validate lengths match following strings
            if (strlen(a4) != (size_t)(hlen + plen)) continue;
            char header[4096], payload[4096];
            strncpy(header, a4, hlen);
            header[hlen] = '\0';
            strcpy(payload, a4 + hlen);
            cache_packet(&c, a1, hlen, plen, header, payload);
        } else if (strcmp(cmd, "DROP") == 0) {
            cache_drop(&c, a1);
        } else if (strcmp(cmd, "HEADER") == 0) {
            cache_header(&c, a1);
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            cache_payload(&c, a1);
        } else if (strcmp(cmd, "STATS") == 0) {
            cache_stats(&c);
        }
    }
    cache_free(&c);
    return 0;
}