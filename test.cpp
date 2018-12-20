#include <iostream>
#include <stdio.h>
#include <string>

using namespace std;

int main(int argc, char const *argv[]) {
  int c = 0; // centaines
  int d = 0; // dizaines
  int u = 1; // unités

  char *file_name;
  sprintf(file_name, "\%d%d%d-rgb.png", c,d,u);
  String image_name=string(direct_name)+string(file_name);

  std::cout << image_name << std::endl;
  return 0;
}
