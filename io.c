#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> 
#include <termios.h>
#include <unistd.h>  /* UNIX Standard Definitions      */ 
#include <errno.h>
#include <string.h>

char *readSerialBytes() {
  int serial_port = open("/dev/ttyACM0", O_RDWR);

  struct termios tty;

  if(tcgetattr(serial_port, &tty) != 0) {
      printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
      return NULL;
  }

  tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
  tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
  tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size 
  tty.c_cflag |= CS8; // 8 bits per byte (most common)
  tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
  tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

  tty.c_lflag &= ~ICANON;
  tty.c_lflag &= ~ECHO; // Disable echo
  tty.c_lflag &= ~ECHOE; // Disable erasure
  tty.c_lflag &= ~ECHONL; // Disable new-line echo
  tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
  tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
  tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

  tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
  tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed

  tty.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
  tty.c_cc[VMIN] = 0;

  // Set in/out baud rate to be 9600
  cfsetispeed(&tty, B115200);
  cfsetospeed(&tty, B115200);

  // Save tty settings, also checking for error
  if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
      printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
      return NULL; 
  }
  // Allocate memory for read buffer, set size according to your needs
  char read_buf[1028];

  memset(&read_buf, '\0', sizeof(read_buf));

  int num_bytes = read(serial_port, &read_buf, sizeof(read_buf));

  if (num_bytes < 0) {
      printf("Error reading: %s", strerror(errno));
      return NULL;
  }
  if(num_bytes == 0) return NULL;
 // printf("Read %i bytes. Received message: %s\n", num_bytes, read_buf);
  close(serial_port);
  char *buffer = malloc(sizeof(read_buf));
  strncpy(buffer, read_buf, strlen(read_buf));
  return buffer;
}

void readSerialData(char *buffer) {
  char *message = readSerialBytes();
  if(message == NULL) return;
 // printf("m->%s\n", message);
  if(buffer[0] == 'b' && message[0] != 'b') strcat(buffer, message);
  else strcpy(buffer, message);
 // printf("buff->%s\n", buffer);
  return;
}
