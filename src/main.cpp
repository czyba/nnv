#include <iostream>
#include <terminal_out.h>
#include <auto_vector.hpp>

using namespace std;
using namespace nnv;

int main() {
  auto_vector<char> v;
  std::vector<FONT_OPTIONS> v2;
  v2.reserve(10);
  auto it = v2.begin();
  v2.insert(it++, FONT_OPTIONS::FG_RED);
  v2.insert(it++, FONT_OPTIONS::BG_BLUE);
  v2.insert(it++, FONT_OPTIONS::FONT_BOLD_OFF);

  cout << "size = " << v2.size() << endl;
  tout.modify_output(v2);
  tout.write_vec(v, 0, v.size());
  v.insertAt('a', 0);
  tout.write_vec(v, 0, v.size());
  v.insertAt('b', 1);
  tout.write_vec(v, 0, v.size());
  v.insertAt('c', 1);
  tout.write_vec(v, 0, v.size());

  v.deleteAt(1);
  tout.write_vec(v, 0, v.size());
  v.deleteAt(1);
  tout.write_vec(v, 0, v.size());
  v.deleteAt(0);
  tout.write_vec(v, 0, v.size());
  return 0;
}
