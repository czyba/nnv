#include <iostream>
#include <terminal_out.h>
#include <auto_vector.hpp>

using namespace std;
using namespace nnv;

int main() {
  auto_vector<char> v;
  tout.write_vec(v, 0, v.size());
  v.insertAt('a', 0);
  tout.write_vec(v, 0, v.size());
  v.insertAt('b', 1);
  tout.modify_output(FONT_OPTIONS::FG_RED);
  tout.write_vec(v, 0, v.size());
  v.insertAt('c', 1);
  tout.write_vec(v, 0, v.size());

  v.deleteAt(1);
  tout.write_vec(v, 0, v.size());
  v.deleteAt(1);
  tout.modify_output(FONT_OPTIONS::FG_BLACK);
  tout.write_vec(v, 0, v.size());
  v.deleteAt(0);
  tout.write_vec(v, 0, v.size());
  return 0;
}
