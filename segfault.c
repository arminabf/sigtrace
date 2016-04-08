#include <stdio.h>
#include <string.h>

int die() {
  char *str = NULL;
  strcpy(str, "byebye");
}

int main() {
  die();
  return 0;
}
