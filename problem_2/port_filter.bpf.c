// SPDX-License-Identifier: GPL-2.0
#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_endian.h>

#define COMM_LEN 16

struct config {
    __u16 port;
    char  comm[COMM_LEN];
};

struct {
    __uint(type, BPF_MAP_TYPE_ARRAY);
    __uint(max_entries, 1);
    __type(key, __u32);
    __type(value, struct config);
} cfg_map SEC(".maps");

static __always_inline int comm_match(const char *a, const char *b)
{
#pragma unroll
    for (int i = 0; i < COMM_LEN; i++) {
        if (a[i] != b[i])
            return 0;
        if (a[i] == '\0')
            return 1;
    }
    return 1;
}

static __always_inline int check(struct bpf_sock_addr *ctx)
{
    __u32 key = 0;
    struct config *cfg = bpf_map_lookup_elem(&cfg_map, &key);
    if (!cfg)
        return 1;

    char comm[COMM_LEN] = {};
    bpf_get_current_comm(&comm, sizeof(comm));
    if (!comm_match(comm, cfg->comm))
        return 1;                       /* different process: allow */

    if (ctx->type != 1)
        return 0;                       /* non-TCP: block */

    if (bpf_ntohs(ctx->user_port) == cfg->port)
        return 1;                       /* matching port: allow */

    return 0;                           /* everything else: block */
}

SEC("cgroup/connect4")
int block_connect4(struct bpf_sock_addr *ctx) { return check(ctx); }

SEC("cgroup/connect6")
int block_connect6(struct bpf_sock_addr *ctx) { return check(ctx); }

SEC("cgroup/sendmsg4")
int block_sendmsg4(struct bpf_sock_addr *ctx) { return check(ctx); }

SEC("cgroup/sendmsg6")
int block_sendmsg6(struct bpf_sock_addr *ctx) { return check(ctx); }

char LICENSE[] SEC("license") = "GPL";
