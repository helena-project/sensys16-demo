#include <stdlib.h>
#include <stdio.h>

void print_hex(const uint8_t *buf, int len) {
  int i;
  char* buf_str = (char*) malloc (2*len + 1);
  char* buf_ptr = buf_str;
  for (i = 0; i < len; i++) {
      buf_ptr += sprintf(buf_ptr, "%02X", buf[i]);
  }
  sprintf(buf_ptr,"\n");
  *(buf_ptr + 1) = '\0';
  printf("%s\n", buf_str);
  free(buf_str);
}

int main(void) {
}

