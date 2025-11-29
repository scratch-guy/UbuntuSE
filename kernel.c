// syscall numbers
#define SYS_write 1
#define SYS_read 0
#define SYS_fork 57
#define SYS_execve 59
#define SYS_wait4 61

// write syscall (no libc)
void print(const char *s) {
    long len = 0;
    while (s[len]) len++;

    asm volatile(
        "movq %0, %%rsi\n"
        "movq %1, %%rdx\n"
        "movq $1, %%rdi\n"
        "movq $1, %%rax\n"
        "syscall"
        :
        : "r"(s), "r"(len)
        : "%rax", "%rdi", "%rsi", "%rdx"
    );
}

// read syscall (no libc)
long read_input(char *buf, long max) {
    long ret;
    asm volatile(
        "movq %1, %%rsi\n"
        "movq %2, %%rdx\n"
        "movq $0, %%rdi\n"
        "movq $0, %%rax\n"
        "syscall\n"
        "movq %%rax, %0\n"
        : "=r"(ret)
        : "r"(buf), "r"(max)
        : "%rax", "%rdi", "%rsi", "%rdx"
    );
    return ret;
}

// REAL system() — NO LIBC
int system(const char *cmd) {
    long pid;

    // fork()
    asm volatile(
        "movq $57, %%rax\n"
        "syscall\n"
        "movq %%rax, %0\n"
        : "=r"(pid)
        :
        : "%rax"
    );

    if (pid == 0) {
        // child: execve("/bin/sh", ["sh","-c",cmd], environ)

        const char *sh = "/bin/sh";
        const char *arg0 = "sh";
        const char *arg1 = "-c";

        const char *argv[] = { arg0, arg1, cmd, 0 };
        const char *envp[] = { 0 };

        asm volatile(
            "movq $59, %%rax\n"      // execve
            "movq %0, %%rdi\n"       // filename
            "movq %1, %%rsi\n"       // argv
            "movq %2, %%rdx\n"       // envp
            "syscall"
            :
            : "r"(sh), "r"(argv), "r"(envp)
            : "%rax", "%rdi", "%rsi", "%rdx"
        );

        // if exec fails:
        asm volatile(
            "movq $60, %%rax\n"
            "movq $1, %%rdi\n"
            "syscall"
        );
    }

    // parent: waitpid(pid)
    int status;
    asm volatile(
        "movq %1, %%rdi\n"
        "movq %2, %%rsi\n"
        "movq $0, %%rdx\n"
        "movq $61, %%rax\n"
        "syscall\n"
        : "=r"(status)
        : "r"(pid), "r"(&status)
        : "%rax", "%rdi", "%rsi", "%rdx"
    );

    return status;
}

int main() {
    char cmd[256];

    while (1) {
        print("root@UbuntuSE:/# ");

        long n = read_input(cmd, 255);
        cmd[n] = '\0';

        system(cmd);
    }
}
//>:(
