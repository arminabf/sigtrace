# sigtrace
A tool to get the backtrace of a process after receiving a specific signal

## Build

To build the library simply do

```bash
make all
```

## Testing

You can use the included test binary _segfault_ to test the tool with signal SIGSEGV

```bash
LD_PRELOAD=/path/to/sigtrace.so /path/to/segfault
```

or use the tool with any other binary. For example, to test SIGINT run

```bash
LD_PRELOAD=/path/to/sigtrace.so ready y
```
and stop the process with Ctrl-C.


The tool should give an output similar to following

```
[sigtrace] Received signal SIGSEGV (11).
[sigtrace] 0: ./sigtrace.so(handler+0x84) [0xb77348e4]
[sigtrace] 1: [0xb7739410]
[sigtrace] 2: ./segfault() [0x80483fd]
[sigtrace] 3: ./segfault() [0x8048417]
[sigtrace] 4: /lib/i386-linux-gnu/libc.so.6(__libc_start_main+0xf3) [0xb7582a83]
[sigtrace] 5: ./segfault() [0x8048311]
```

## Analysis

For example, use _objdump_ to analyze the addresses obtained by the printed backtrace.

```
objdump -S segfault | grep -C 10 80483fd
```


