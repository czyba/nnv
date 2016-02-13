#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <array>
#include <terminal_out.h>
#include <termios.h>

using namespace std;
using namespace nnv;

namespace nnv {

terminal_out tout;

terminal_out::terminal_out() {
  fd = open("/dev/tty", O_RDWR | O_NONBLOCK);
  if(fd < 0) {
    return;
  }
  struct termios term;

  /*We store the actual properties of the input console and set it as:
    no buffered (~ICANON): avoid blocking
    no echoing (~ECHO): do not display the result on the console*/
  tcgetattr(fd, &initial_term);
  term = initial_term;
  term.c_lflag &= ~ICANON;
  term.c_lflag &= ~ECHO;
  term.c_iflag &= ~ISTRIP;
  term.c_iflag |= IGNCR;
  term.c_oflag &= ~OCRNL;
  term.c_oflag |= ONLRET;
  term.c_cflag &= ~CSIZE;
  term.c_cflag |= CS8;
  tcsetattr(fd, TCSANOW, &term);
}

terminal_out::~terminal_out() {
  if(fd > 0) {
    tcsetattr(fd, TCSANOW, &initial_term);
    close(fd);
  }
}

bool terminal_out::get_position(size_t& row, size_t& column) {
  array<char, 4> a {{(char)0x1B, '[', '6', 'n'}};
  write_chars(a.data(), a.size());
  fd_set readset;
  FD_ZERO(&readset);
  FD_SET(STDIN_FILENO, &readset);
  if (select(fd, &readset, NULL, NULL, NULL) == 1) {
    int ret = scanf("\x1B[%lu;%luR", &row, &column);
    if (ret != 2) {
      return false;
    }
  }
  return true;
}

bool terminal_out::get_size(size_t& row, size_t& column) {
  size_t tmp_row, tmp_column;
  if(!get_position(tmp_row, tmp_column)) {
    return false;
  }
  if(!move_cursor_to(9999,9999)) {
    return false;
  }
  if(!get_position(row, column)) {
    return false;
  }
  if(!move_cursor_to(tmp_row,tmp_column)) {
    return false;
  }
  return true;
}

bool terminal_out::move_cursor_to(size_t row, size_t column) {
  if(row > 9999)
    row = 9999;
  if(!row) 
    row = 1;
  if(column > 9999)
    column = 9999;
  if(!column)
    column = 1;
  array<char, 2 + 4 + 1 + 4 + 1> a;
  size_t toWrite = 2;
  a[0] = '\x1B';
  a[1] = '[';
  int ret = sprintf(a.data() + toWrite, "%lu", row);
  if(ret <= 0) {
    return false;
  }
  toWrite += static_cast<size_t>(ret);
  a[toWrite++] = ';';

  ret = sprintf(a.data() + toWrite, "%lu", column);
  if(ret <= 0) {
    return false;
  }
  toWrite += static_cast<size_t>(ret);
  a[toWrite++] = 'H';
  write_chars(a.data(), toWrite);
  return true;
}

size_t terminal_out::write_chars(char const* buf, size_t const size) {
  if(fd < 0) {
    return 0;
  }
  ssize_t written = write(fd, buf, size);
  return static_cast<size_t>(written);
}

size_t terminal_out::write_vec(auto_vector<char> const& vec, size_t const start, size_t const end) {
  return write_chars(vec.data() + start, end - start);
}

int terminal_out::insert_option_code(char* buf, enum FONT_OPTIONS option) {
  return sprintf(buf, "%hhu", option);
}

bool terminal_out::modify_output(std::vector<enum FONT_OPTIONS> options) {
  if (!options.size() || options.size() > 16) {
    return false;
  }
  std::vector<char> buf;
  buf.reserve(3 + options.size() * 3);
  size_t toWrite = 2;
  buf[0] = 0x1B;
  buf[1] = '[';
  for (auto it = options.begin(); it != options.end(); it++) {
    int ret = insert_option_code(buf.data() + toWrite, *it);
    if (ret < 0) {
      return false;
    }
    toWrite += static_cast<size_t>(ret);
    buf[toWrite++] = ';';
  }
  buf[toWrite - 1] = 'm';
  size_t ret = write_chars(buf.data(), toWrite);
  if (ret != toWrite) {
    return false;
  }
  return true;
}
}
