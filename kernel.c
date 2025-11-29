// A C function that prints a string using a system call (no libraries)
void print(const char *str) {
    // Calculate the string length manually
    int len = 0;
    while (str[len] != '\0') {
        len++;
    }

    // Linux x86-64 syscall: write(fd=1, buf, count)
    // syscall number for write: 1
    // fd=1 -> stdout
    asm(
        "movq $1, %%rax\n"       // syscall number: sys_write
        "movq $1, %%rdi\n"       // file descriptor: stdout
        "movq %0, %%rsi\n"       // buffer to write
        "movq %1, %%rdx\n"       // length of buffer
        "syscall\n"
        :
        : "r"(str), "r"((long)len)
        : "%rax", "%rdi", "%rsi", "%rdx"
    );
}

// A C function that reads input from the user (no libraries)
long read(char *buffer, long max_len) {
    long bytes_read;

    // Linux x86-64 syscall: read(fd=0, buf, count)
    // syscall number for read: 0
    // fd=0 -> stdin
    asm(
        "movq $0, %%rax\n"       // syscall number: sys_read
        "movq $0, %%rdi\n"       // file descriptor: stdin
        "movq %1, %%rsi\n"       // buffer to store input
        "movq %2, %%rdx\n"       // max number of bytes to read
        "syscall"
        "movq %%rax, %0\n"       // store number of bytes read
        : "=r"(bytes_read)
        : "r"(buffer), "r"(max_len)
        : "%rax", "%rdi", "%rsi", "%rdx"
    );

    return bytes_read;  // returns number of bytes read
}


//lets begin

int main(void) {
    char cmd[256];
    while (1) {
        print("root@UbuntuSE:/$ ");
        cmd = read();
        print(cmd);
        //rest will be implimented soon
    }
}
