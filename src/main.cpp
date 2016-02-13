#include <iostream>
#include <terminal_out.h>
#include <auto_vector.hpp>

using namespace std;
using namespace nnv;


#include <unistd.h>

int main() {
  auto_vector<char> v;
  std::vector<FONT_OPTIONS> v2;
  v2.reserve(10);
  auto it = v2.begin();
  v2.insert(it++, FONT_OPTIONS::FG_RED);
  v2.insert(it++, FONT_OPTIONS::BG_BLUE);
  v2.insert(it++, FONT_OPTIONS::FONT_BOLD_OFF);
  v2.insert(it++, FONT_OPTIONS::FONT_BRIGHT_ON);

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

  size_t row, column;
  tout.get_size(row, column);

  for(size_t i = 0; i < column; i++) {
    v.insertAt(((i % 26U) + 'a'), i);
  }

  tout.move_cursor_to(1,1);

  for(size_t i = 0; i < row; i++) {
    if(i == 40) {
      size_t j = 26;
      tout.write_vec(v, 0, j);
      it = v2.begin();
      v2[0] = FONT_OPTIONS::FG_BLUE;
      v2[1] = FONT_OPTIONS::BG_RED;
      v2[2] = FONT_OPTIONS::FONT_BOLD_ON;
      v2[3] = FONT_OPTIONS::FONT_BRIGHT_OFF;
      tout.modify_output(v2);
      tout.write_vec(v, j, v.size());
    } else  {
      size_t ret = tout.write_vec(v, 0, v.size());
    }
  }


  return 0;
}
