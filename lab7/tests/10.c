/* допустимы и вложенные определения */
struct Token {
  struct Fragment {
    struct Pos {
      int line;
      int col;
    } starting, following;
  } fragment;

  enum { Ident, IntConst, FloatConst } type;

  union {
    char *name;
    int int_value;
    double float_value;
  } info;
};
