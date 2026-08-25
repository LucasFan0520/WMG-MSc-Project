// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 8192

static char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

typedef struct {
    char *id;
    char *header;
    char *payload;
    size_t payload_len;
} Packet;

static Packet *arr = NULL;
static size_t cnt = 0, cap = 0;

static int grow(void) {
    if (cnt < cap) return 0;
    size_t nc = cap ? cap * 2 : 8;
    if (nc < cap) return -1;
    Packet *t = realloc(arr, nc * sizeof(Packet));
    if (!t) return -1;
    arr = t; cap = nc;
    return 0;
}

static int find_packet(const char *id) {
    for (size_t i = 0; i < cnt; i++)
        if (strcmp(arr[i].id, id) == 0) return (int)i;
    return -1;
}

static void free_packet(Packet *p) {
    free(p->id); free(p->header); free(p->payload);
    p->id = NULL; p->header = NULL; p->payload = NULL;
}

static void cmd_packet(char *id, size_t hLen, size_t pLen, char *header, char *payload) {
    if (find_packet(id) >= 0) return;
    if (strlen(header) != hLen || strlen(payload) != pLen) return;
    if (grow() < 0) return;
    arr[cnt].id = safe_strdup(id);
    arr[cnt].header = safe_strdup(header);
    arr[cnt].payload = safe_strdup(payload);
    arr[cnt].payload_len = pLen;
    if (!arr[cnt].id || !arr[cnt].header || !arr[cnt].payload) {
        free_packet(&arr[cnt]);
        return;
    }
    cnt++;
}

static void cmd_drop(char *id) {
    int idx = find_packet(id);
    if (idx < 0) return;
    free_packet(&arr[idx]);
    for (size_t i = (size_t)idx; i + 1 < cnt; i++) arr[i] = arr[i + 1];
    cnt--;
}

static void cmd_header(char *id) {
    int idx = find_packet(id);
    if (idx < 0) return;
    printf("%s\n", arr[idx].header);
}

static void cmd_payload(char *id) {
    int idx = find_packet(id);
    if (idx < 0) return;
    printf("%s\n", arr[idx].payload);
}

static void cmd_stats(void) {
    size_t total = 0;
    for (size_t i = 0; i < cnt; i++) {
        if (total > (size_t)-1 - arr[i].payload_len) return;
        total += arr[i].payload_len;
    }
    printf("%zu\n", total);
}

static void cleanup(void) {
    for (size_t i = 0; i < cnt; i++) free_packet(&arr[i]);
    free(arr); arr = NULL; cnt = 0; cap = 0;
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64] = {0}; int n1 = 0;
        if (sscanf(line, "%63s%n", cmd, &n1) < 1) continue;
        char *rest = line + n1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "PACKET") == 0) {
            char id[256] = {0}; size_t hLen = 0, pLen = 0; int n2 = 0, n3 = 0, n4 = 0;
            if (sscanf(rest, "%255s%n %zu%n %zu%n", id, &n2, &hLen, &n3, &pLen, &n4) < 3) continue;
            char *header = rest + n4;
            while (*header == ' ') header++;
            char *payload = header + hLen;
            if (*payload == ' ') payload++;
            else continue;
            cmd_packet(id, hLen, pLen, header, payload);
        } else if (strcmp(cmd, "DROP") == 0 || strcmp(cmd, "HEADER") == 0 || strcmp(cmd, "PAYLOAD") == 0) {
            char id[256] = {0};
            if (sscanf(rest, "%255s", id) < 1) continue;
            if (strcmp(cmd, "DROP") == 0) cmd_drop(id);
            else if (strcmp(cmd, "HEADER") == 0) cmd_header(id);
            else cmd_payload(id);
        } else if (strcmp(cmd, "STATS") == 0) {
            cmd_stats();
        }
    }
    cleanup();
    return 0;
}