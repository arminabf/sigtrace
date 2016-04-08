# sigtrace
A tool to get the backtrace of a process after receiving a specific signal

## Build

```bash
make all
```

## Testing

```bash
LD_PRELOAD=/path/to/sigtrace.so /path/to/segfault
```


