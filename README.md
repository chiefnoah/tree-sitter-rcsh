# tree-sitter-rcsh

Tree-sitter grammar and Neovim runtime plugin for the Plan 9 rc shell.

The Neovim plugin ships:

- `parser/rcsh.so`, built from `src/parser.c` and `src/scanner.c`
- `queries/rcsh/highlights.scm`
- `queries/rcsh/injections.scm`
- `ftdetect/rcsh.vim` for `*.rcsh`

Heredoc injection is generic: the rcsh parser captures the heredoc marker as
`@injection.language` and the body as `@injection.content`. The plugin scans rcsh
buffers and registers suffix-style markers through Neovim's filetype mapping, so
`<<'js'` injects JavaScript when the JavaScript parser is installed, `<<'py'`
injects Python, and so on.

`*.rc` detection is intentionally off by default because that suffix is shared by
other tools. To opt in:

```lua
vim.api.nvim_create_autocmd({ "BufRead", "BufNewFile" }, {
  pattern = "*.rc",
  command = "set filetype=rcsh",
})
```

## Build

```sh
make
```

Regenerate the parser after changing `grammar.js`:

```sh
tree-sitter generate
make
```
