// A C function that prints a string using a system call (no libraries)
void print(const char *str) {
    int len = 0;
    while (str[len] != '\0')
        len++;

    asm(
        "movq $1, %%rax\n"   // sys_write
        "movq $1, %%rdi\n"   // stdout
        "movq %0, %%rsi\n"   // buffer
        "movq %1, %%rdx\n"   // length
        "syscall\n"
        :
        : "r"(str), "r"((long)len)
        : "%rax", "%rdi", "%rsi", "%rdx"
    );
}


// A C function that reads input from the user (no libraries)
long read_input(char *buffer, long max_len) {
    long bytes_read;

    asm(
        "movq $0, %%rax\n"       // sys_read
        "movq $0, %%rdi\n"       // stdin
        "movq %1, %%rsi\n"       // buffer
        "movq %2, %%rdx\n"       // count
        "syscall\n"
        "movq %%rax, %0\n"       // return value
        : "=r"(bytes_read)
        : "r"(buffer), "r"(max_len)
        : "%rax", "%rdi", "%rsi", "%rdx"
    );

    return bytes_read;
}



int main(void) {
    char cmd[256];

    while (1) {
        print("root@UbuntuSE:/# ");

        long n = read_input(cmd, 255);   // read up to 255 bytes
        if (n > 0) {
            cmd[n] = '\0';               // null‑terminate
            print(cmd);                  // echo back
        }
    }
}
