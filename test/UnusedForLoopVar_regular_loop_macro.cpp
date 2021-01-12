// RUN: clang -cc1 -verify -load %shlibdir/libUnusedForLoopVar%shlibext -plugin UFLV %s 2>&1

#define GENERATE_UNUSED_REGULAR_FOR_LOOP_VAR(size, inc) \
  do {\
    for (int x = 0; x < size; ++x)\
      ++inc;\
  }\
  while (0)

int foo(int arg) {
  // expected-warning@+1 {{(Recursive AST Visitor) regular for-loop variable not used}}
  GENERATE_UNUSED_REGULAR_FOR_LOOP_VAR(3, arg);
  return arg;
}

