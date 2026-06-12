module.exports = grammar({
  name: "rcsh",

  externals: $ => [
    $.heredoc_marker,
    $.heredoc_body,
  ],

  extras: $ => [
    /[ \t\r]+/,
  ],

  rules: {
    source_file: $ => repeat(choice(
      $.comment,
      $.quoted_string,
      $.heredoc_redirect,
      $.redirect,
      $.variable,
      $.keyword,
      $.operator,
      $.word,
      $.terminator,
      $.delimiter,
    )),

    comment: _ => token(seq("#", /.*/)),

    quoted_string: _ => token(seq(
      "'",
      repeat(choice(/[^']/, "''")),
      "'",
    )),

    heredoc_redirect: $ => seq(
      $.heredoc_operator,
      optional($.heredoc_quote),
      $.heredoc_marker,
      optional($.heredoc_quote),
      $.heredoc_body,
    ),

    heredoc_operator: _ => token(seq(
      "<<",
      optional(seq(
        "[",
        /[0-9]+/,
        optional(seq("=", optional(/[0-9]+/))),
        "]",
      )),
    )),

    heredoc_quote: _ => token("'"),

    redirect: _ => token(seq(
      choice("<<<", ">>", "<", ">"),
      optional(seq(
        "[",
        /[0-9]+/,
        optional(seq("=", optional(/[0-9]+/))),
        "]",
      )),
    )),

    variable: _ => token(seq(
      "$",
      optional(choice("#", "^", "\"")),
      choice(/[A-Za-z0-9_*]+/, "'"),
    )),

    keyword: _ => token(choice(
      "case",
      "else",
      "fn",
      "for",
      "if",
      "in",
      "switch",
      "while",
    )),

    operator: _ => token(choice(
      "&&",
      "||",
      "|",
      "&",
      "^",
      "=",
      "~",
      "!",
      "@",
      "$",
      "`",
    )),

    terminator: _ => token(choice("\n", ";")),

    delimiter: _ => token(choice("(", ")", "{", "}")),

    word: _ => token(repeat1(choice(
      /[^ \t\r\n#;&|^$=~`'{}@!()<>\\]+/,
      /\\[^\n]/,
      /\\\n/,
    ))),
  },
});
