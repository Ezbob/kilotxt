#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>

struct termios orig_termios;

void die(const char *msg) {
    perror(msg);
    exit(1);
}

void disableRawMode() {
    if ( tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1 ) { 
        die("tcsetattr failed");
    }
}

void enableRawMode() {
    if ( tcgetattr(STDIN_FILENO, &orig_termios) == -1 ) {
        die("tcgetattr failed");
    }
    atexit(disableRawMode);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN); /* turn off carriage return translation into newline(ICRNL), echo of output (ECHO), line buffering (ICANON) and input signals (ctrl-c (SIGINT)...) ctrl-v (IEXTEN) affects some system*/
    raw.c_oflag &= ~(OPOST); /* turn off all post-processing of output (OPOST). newlines gets translated into \r\n in the output */
    raw.c_cflag |= (CS8);
    raw.c_iflag &= ~(IXON | BRKINT | ICRNL | INPCK | ISTRIP); /* turn off software flow control e.g. ctrl-s ctrl-q wait and resume */

    /* tinker with terminal options */
    raw.c_cc[VMIN] = 0; /* minimum bytes needed to be written to stdin before read happens */
    raw.c_cc[VTIME] = 1; /* max blocking time before read, measured in 1/10 seconds; default behaviour is indefinited */

    if ( tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1 ) {
        die("tcsetattr failed");
    }
}


int main(void) {
    enableRawMode();
    char c;

    while ( 1 ) {
        c = '\0';
        if ( read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN ) {
            die("read failed");
        }

        if ( iscntrl(c) ) {
            printf("%d\r\n", c);
        } else {
            printf("%d ('%c')\r\n", c, c);
        }

        if (c == 'q') break;
    }

    return EXIT_SUCCESS;
}

