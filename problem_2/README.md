# Problem 2: Drop packets only for a given process

eBPF program that allows a given process (default: myprocess) to send
traffic only to TCP port 4040. All other ports are blocked for that
process. Other processes are not affected.

- Code: port_filter.bpf.c
