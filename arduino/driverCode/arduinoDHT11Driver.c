#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

/*
    Depending on the serial port of the Arduino, adjust this macro
    On MacOS, run: ls /dev/cu.*
    NOTE: Opening the serial port resets most Arduinos
*/
#define PORT_NAME "/dev/cu.usbmodem11301"
#define BAUD B115200

int setup_serial(const char *portname, int baudrate) 
{
    int fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) 
    { 
        perror("open"); 
        return -1; 
    }

    struct termios tty;
    if(tcgetattr(fd, &tty) != 0) 
    { 
        perror("tcgetattr"); 
        close(fd); 
        return -1; 
    }

    // Raw 8N1, no flow control
    cfsetospeed(&tty, baudrate);
    cfsetispeed(&tty, baudrate);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~(PARENB | PARODD | CSTOPB | CRTSCTS);

    tty.c_iflag &= ~(IGNBRK | IXON | IXOFF | IXANY);
    tty.c_lflag = 0; // raw: no echo, no canonical, no signals
    tty.c_oflag = 0;

    // Block until at least 1 byte arrives; no timeout
    tty.c_cc[VMIN]  = 1;
    tty.c_cc[VTIME] = 0;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) { perror("tcsetattr"); close(fd); return -1; }

    // Flush any junk, then wait for Arduino to reboot after DTR toggle
    tcflush(fd, TCIFLUSH);
    // sleep(2);

    return fd;
}

// Read one line (ending with '\n'), stripping '\r'. Returns length (>=0).
int read_line(int fd, char *line, int maxlen) 
{
    int len = 0;
    while (len < maxlen - 1) 
    {
        char ch;
        int n = read(fd, &ch, 1); // read one byte/character at a time
        // Possible values of n:
        // n = 1: Read one byte (what we want)
        // n = 0: Read zero bytes
        // n = -1: Error occurred
        if (n < 0) 
        {
            if (errno == EINTR)
            {
                continue; // interrupted system call, try again
            }
            return -1; // read error
        }
        // With VMIN=1, read() blocks until a byte arrives, so n==0 is rare
        if(n == 0)
        {
            continue;
        }
        if(ch == '\r') 
        {
            continue; // drop CR if encounter it
        }
        if(ch == '\n') // once we encounter the new line character we exit immediately, don't include it in our line char array
        {
            break; // line complete
        }
        line[len++] = ch;
    }

    line[len] = '\0'; // put a null character to signify end of string/line
    return len;
}

int main(void) 
{
    int fd = setup_serial(PORT_NAME, BAUD);
    if (fd < 0) 
    {
        fprintf(stderr, "Failed to open serial port: %s\n", PORT_NAME);
        return 1;
    }

    char line[256];
    int n = read_line(fd, line, sizeof(line));
    if(n > 0) 
    {
        printf("Received:\n%s\n", line);
        fflush(stdout);
    } 
    else if(n < 0) 
    {
        perror("read_line");
    }
    // if n==0, it was an empty line; loop continues

    close(fd);
    return 0;
}