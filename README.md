# Problem: 1 
# Drop TCP packets on a port using eBPF (XDP)

Drops TCP packets on a given port (default 4040). The port can be
changed from userspace through an eBPF map.

## Build
clang -O2 -g -target bpf -c drop_port.c -o drop_port.o

## Run
sudo ip link set dev lo xdpgeneric obj drop_port.o sec xdp

## Change the port (example: 8080)
sudo bpftool map show name drop_port
sudo bpftool map update id <MAP_ID> key 0 0 0 0 value 0x90 0x1f

## Stop
sudo ip link set dev lo xdpgeneric off

## Test
nc -l 4040
nc -v -w 3 127.0.0.1 4040
