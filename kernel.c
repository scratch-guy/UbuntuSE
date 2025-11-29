#include <stdint.h>

// print (sys_write)
void print(const char *str) {
    int len = 0;
    while (str[len] != '\0') len++;
    asm(
        "movq $1, %%rax\n"
        "movq $1, %%rdi\n"
        "movq %0, %%rsi\n"
        "movq %1, %%rdx\n"
        "syscall\n"
        :
        : "r"(str), "r"((long)len)
        : "%rax", "%rdi", "%rsi", "%rdx"
    );
}

// read_input (sys_read)
long read_input(char *buffer, long max_len) {
    long bytes_read;
    asm(
        "movq $0, %%rax\n"
        "movq $0, %%rdi\n"
        "movq %1, %%rsi\n"
        "movq %2, %%rdx\n"
        "syscall\n"
        "movq %%rax, %0\n"
        : "=r"(bytes_read)
        : "r"(buffer), "r"(max_len)
        : "%rax", "%rdi", "%rsi", "%rdx"
    );
    return bytes_read;
}

// system() replacement (fork + execve + waitpid)
int system(const char *command) {
    long pid;
    asm("movq $57, %%rax\nsyscall\nmovq %%rax, %0\n" : "=r"(pid) : : "%rax");

    if (pid == 0) {
        const char *argv[3] = {"/bin/sh", "-c", command};
        const char *envp[1] = {0};
        asm(
            "movq $59, %%rax\n"
            "movq %0, %%rdi\n"
            "movq %1, %%rsi\n"
            "movq %2, %%rdx\n"
            "syscall\n"
            :
            : "r"(argv[0]), "r"(argv), "r"(envp)
            : "%rax", "%rdi", "%rsi", "%rdx"
        );
        asm("movq $60, %%rax\nmovq $1, %%rdi\nsyscall\n" : : : "%rax", "%rdi");
    } else {
        long status;
        asm(
            "movq $61, %%rax\n"
            "movq %1, %%rdi\n"
            "movq $0, %%rsi\n"
            "movq $0, %%rdx\n"
            "syscall\n"
            "movq %%rax, %0\n"
            : "=r"(status)
            : "r"(pid)
            : "%rax", "%rdi", "%rsi", "%rdx"
        );
    }
    return 0;
}

// main shell loop
int main(void) {
    char cmd[256];
    while (1) {
        print("root@UbuntuSE:/# ");
        long n = read_input(cmd, 255);
        if (n > 0) {
            cmd[n] = '\0';
            system(cmd);
        }
    }
}
