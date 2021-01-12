// RUN: clang++ -Xclang -verify -Xclang -load -Xclang %shlibdir/libUnusedForLoopVar%shlibext -Xclang -plugin -Xclang UFLV -c %s 2>&1

#define GENERATE_UNUSED_RANGE_FOR_LOOP_VAR(arr, inc) \
  do {\
    for (auto x : arr)\
      ++inc;\
  }\
  while (0)

int foo(int arg) {
  int arr[3] = {1, 2, 3};
  // expected-warning@+1 {{(AST Matcher) range for-loop variable not used}}
  GENERATE_UNUSED_RANGE_FOR_LOOP_VAR(arr, arg);
  return arg;
}

