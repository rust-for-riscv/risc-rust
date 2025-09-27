// gcc -O2 -Wall -Wextra -o rv32_min rv32_min.c
// Usage: ./rv32_min <input.rs> <output.bin>
//
// Requirements:
//   - rustc in PATH
//   - llvm-objcopy (or rust-objcopy) in PATH
// Notes:
//   - This assumes a single-file bare-metal Rust program (#![no_std], defines an entry symbol, e.g. _start).
//   - For custom memory layout, pass -Clink-arg=-Tmemory.x via RUSTFLAGS env if needed.

#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

static void die(const char *msg) { perror(msg); exit(1); }

static void run(char *const argv[]) {
    pid_t pid = fork();
    if (pid < 0) die("fork");
    if (pid == 0) { execvp(argv[0], argv); _exit(127); }
    int st = 0;
    if (waitpid(pid, &st, 0) < 0) die("waitpid");
    if (!WIFEXITED(st) || WEXITSTATUS(st) != 0) {
        fprintf(stderr, "command failed: %s\n", argv[0]);
        exit(1);
    }
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input.rs> <output.bin>\n", argv[0]);
        return 1;
    }
    const char *in_rs = argv[1];
    const char *out_hex = argv[2];

    // Use a simple ELF path next to the bin (e.g., <output>.elf)
    char out_elf[1024];
    snprintf(out_elf, sizeof out_elf, "%s.elf", out_hex);

    // 1) rustc → RV32I ELF (optimized for size, abort on panic)
    char *rustc_argv[] = {
        "rustc",
        (char*)in_rs,
        "--target=riscv32i-unknown-none-elf",
        "-Copt-level=s",
        "-Cpanic=abort",
        "--emit=link",
        "-o", out_elf,
        NULL
    };
    run(rustc_argv);

    // 2) objcopy → raw BIN
    const char *objcopy = "llvm-objcopy";  // prefer llvm-objcopy
    if (access("/usr/bin/llvm-objcopy", X_OK) != 0 &&
        access("/usr/local/bin/llvm-objcopy", X_OK) != 0) {
        objcopy = "rust-objcopy";          // fallback
    }
    char *objcopy_argv[] = {
        (char*)objcopy,
        "-O", "ihex",           // options first
        (char*)out_elf,         // input ELF
        (char*)out_hex,         // output HEX
        NULL
    };

    run(objcopy_argv);

    fprintf(stderr, "OK: %s\n", out_hex);
    return 0;
}

