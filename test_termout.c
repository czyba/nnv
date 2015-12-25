#include "termout_impl.h"
#include "characters.h"
#include <stdio.h>

#define T_OUT(x, y)                    \
  if (argc > 1) {                      \
    printf(x ", expecting : " y "\n"); \
  }
#define RESULT_OUT(x, y)                 \
  if (x) {                               \
    printf("\033[1;32m[OK]\033[0m " y);  \
  } else {                               \
    printf("\033[1;31m[NOK]\033[0m " y); \
  }

void test_alloc_free(int argc);
void test_inserting_CSI(int argc);
void test_insert_font_option(int argc);

int main(int argc, char** argv) {
  int ret = 0;
  (void) argv;
  test_alloc_free(argc);
  test_inserting_CSI(argc);
  test_insert_font_option(argc);
  return ret;
}

void test_alloc_free(int argc) {
  printf("Starting Malloc/Free tests\n");
  int error = 0;
  T_OUT("Test allocating a queue of size -1", "NULL");
  tcq_t* q = alloc_command_queue(-1);
  if (q != NULL) {
    printf("Allocating a queue of size -1 failed. It returned %p\n", q);
    free_command_queue(q);
    error = 1;
  }
  T_OUT("Test allocating a queue of size 0", "NULL");
  q = alloc_command_queue(0);
  if (q != NULL) {
    printf("Allocating a queue of size 0 failed. It returned %p\n", q);
    free_command_queue(q);
    error = 1;
  }
  T_OUT("Test allocating a queue of size 10", "An allocated object");
  q = alloc_command_queue(10);
  if (q == NULL) {
    printf("Allocating a queue of size 10 failed. It returned %p\n", q);
    error = 1;
  }

  int ret = free_command_queue(q);
  if (ret) {
    printf("Freeing the allocated command queue failed. It returned %d\n", ret);
    error = 1;
  }

  T_OUT("Test freeing a NULL ptr", "no crash")
  free_command_queue(NULL);
  RESULT_OUT(!error, "Alloc/Free tests\n");
}

void test_inserting_CSI(int argc) {
  printf("Starting insert_CSI tests\n");
  int error = 0;
  T_OUT("Test inserting a CSI into a NULL queue", "returning -1")
  int ret = append_CSI(NULL);
  if (ret >= 0) {
    printf("Inserted CSI into a NULL queue.\n");
    error = 1;
  }
  T_OUT("Test inserting a CSI when the queue does not have enough space", "returning -1")
  tcq_t* q = alloc_command_queue(1);
  if (q == NULL) {
    printf("Allocating a command queue of size 1 failed\n");
    error = 1;
    goto CSI_end;
  }
  ret = append_CSI(q);
  if (ret >= 0) {
    printf("Inserted CSI into a queue that was too small.\n");
    error = 1;
  }
  free_command_queue(q);
  T_OUT("Test inserting a CSI into a queue with enough space", "returning 2 and the queue having the correct values")
  q = alloc_command_queue(2);
  if (q == NULL) {
    printf("Allocating a command queue of size 2 failed\n");
    error = 1;
    goto CSI_end;
  }
  ret = append_CSI(q);
  if (ret != 2 || q->buf[0] != ESC || q->buf[1] != '[') {
    printf("Inserted CSI into a queue with enough space failed.\n");
    error = 1;
  }
  free_command_queue(q);
CSI_end:
  RESULT_OUT(!error, "insert_CSI tests\n")
}

#define TEST_ONE_OPTION(m, on, off, y, e)                                                                          \
  {                                                                                                                \
    int ret = 0;                                                                                                   \
    T_OUT("Test m option on NULL queue", "returning -1")                                                           \
    ret = append_font_option(NULL, on, m, off, y);                                                                 \
    if (ret >= 0) {                                                                                                \
      printf("Setting m on NULL worked.\n");                                                                       \
      e = 1;                                                                                                       \
    }                                                                                                              \
    T_OUT("Test off option on small queue", "returning -1")                                                        \
    tcq_t* q = alloc_command_queue(1);                                                                             \
    ret = append_font_option(q, off, m, off, y);                                                                   \
    if (ret >= 0) {                                                                                                \
      printf("Setting off option on a too small queue worked. ret = %d\n", ret);                                   \
      e = 1;                                                                                                       \
    }                                                                                                              \
    free_command_queue(q);                                                                                         \
    T_OUT("Test on option on queue", "returning 1 and the respective byte set")                                    \
    q = alloc_command_queue(1);                                                                                    \
    ret = append_font_option(q, on, m, off, y);                                                                    \
    if (ret != 1 || q->buf[0] != y) {                                                                              \
      printf("Could not set on option in queue, ret = %d\n", ret);                                                 \
      e = 1;                                                                                                       \
    }                                                                                                              \
    free_command_queue(q);                                                                                         \
    T_OUT("Test off option on queue", "returning 2 and the respective bytes set")                                  \
    q = alloc_command_queue(2);                                                                                    \
    ret = append_font_option(q, off, m, off, y);                                                                   \
    if (ret != 2 || q->buf[0] != '2' || q->buf[1] != y) {                                                          \
      printf("Could not set on option in queue, ret = %d, buf[0] = %d, buf[1] = %d\n", ret, q->buf[0], q->buf[1]); \
      e = 1;                                                                                                       \
    }                                                                                                              \
    free_command_queue(q);                                                                                         \
    T_OUT("Test setting both options", "returning -1")                                                             \
    q = alloc_command_queue(2);                                                                                    \
    ret = append_font_option(q, m, m, off, y);                                                                     \
    if (ret != -1) {                                                                                               \
      printf("Set both options which is not possible, ret = %d\n", ret);                                           \
      e = 1;                                                                                                       \
    }                                                                                                              \
    free_command_queue(q);                                                                                         \
  }

void test_insert_font_option(int argc) {
  printf("Start testing single font options\n");
  int error = 0;
  TEST_ONE_OPTION(FONT_BOLD_MASK, FONT_BOLD_ON, FONT_BOLD_OFF, '1', error)
  TEST_ONE_OPTION(FONT_BRIGHT_MASK, FONT_BRIGHT_ON, FONT_BRIGHT_OFF, '2', error)
  TEST_ONE_OPTION(FONT_UNDERSCORE_MASK, FONT_UNDERSCORE_ON, FONT_UNDERSCORE_OFF, '4', error)
  TEST_ONE_OPTION(FONT_BLINK_MASK, FONT_BLINK_ON, FONT_BLINK_OFF, '5', error)
  RESULT_OUT(!error, "append_font_option\n")
}
