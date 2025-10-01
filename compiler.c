// Usage: ./rv32_min <input.rs> <output.hex>
//
// Requirements:
//   - rustc in PATH
//      - to install go to rustup.rs for instructions
//      - with target riscv32imac-unknown-none-elf
//          - to add target     > rustup target add riscv32imac-unknown-none-elf
//   - binutils-riscv64-unknown-elf and gcc-riscv64-unknown-elf in PATH
//      - to install    > sudo apt install binutils-riscv64-unknown-elf gcc-riscv64-unknown-elf
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
#include <stdint.h>
#include <limits.h>

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

static int bin_to_wordhex(const char *bin_path, const char *hex_path) {
    FILE *in = fopen(bin_path, "rb");
    if (!in) return -1;
    FILE *out = fopen(hex_path, "w");
    if (!out) { fclose(in); return -2; }

    for (;;) {
        uint8_t b[4];
        size_t n = fread(b, sizeof(b[0]), NUM_READ, in);
        if (n == 0) break;

        // pad last partial word with 0x00
        while (n < 4) b[n++] = 0;

        // little-endian -> 32-bit word
        uint32_t w = (uint32_t)b[0]
                   | ((uint32_t)b[1] << 8)
                   | ((uint32_t)b[2] << 16)
                   | ((uint32_t)b[3] << 24);

        fprintf(out, "%08X\n", w);
    }
    fclose(in);
    fclose(out);
    return 0;
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
        // make the image link at 0x00000000:
        "-Clink-arg=-Ttext=0x0",
        "-Clink-arg=-Tdata=0x0",
        "-Clink-arg=-Tbss=0x0",
        "-o", out_elf,
        NULL
    };

    run(rustc_argv);

    // 2) ELF -> Verilog plaintext HEX (GNU objcopy required)
    const char *objcopy = NULL;
    if (access("/usr/bin/llvm-objcopy", X_OK) == 0) {
        objcopy = "/usr/bin/llvm-objcopy";
    } else if (access("/usr/bin/riscv64-unknown-elf-objcopy", X_OK) == 0) {
        objcopy = "/usr/bin/riscv64-unknown-elf-objcopy";
    } else {
        fprintf(stderr, "Need llvm-objcopy or riscv64-unknown-elf-objcopy\n");
        exit(1);
    }

    char out_bin[4096];
    snprintf(out_bin, sizeof(out_bin), "%s.bin", out_hex);

    char *argv_llvm[] = {
        (char*)objcopy,
        "--output-target=binary",
        "-S",
        "-j", ".text", "-j", ".rodata", "-j", ".data",
        (char*)out_elf, (char*)out_bin, NULL
    };
    char *argv_gnu[] = {
        (char*)objcopy,
        "-O", "binary",
        "-S",
        "-j", ".text", "-j", ".rodata", "-j", ".data",
        (char*)out_elf, (char*)out_bin, NULL
    };

    if (strstr(objcopy, "llvm-objcopy")) run(argv_llvm);
    else                                 run(argv_gnu);

    // then convert out_bin -> out_hex (word-per-line, LE)
    if(bin_to_wordhex(out_bin, out_hex) != 0) {
        die("Could not create word per line hex.\n");
    }

    fprintf(stderr, "OK: %s\n", out_hex);
    return 0;
}

