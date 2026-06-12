autocmd BufRead,BufNewFile *.rcsh set filetype=rcsh

" *.rc is common across other tools, so it is intentionally off by default.
" To opt in, add this to your Neovim config:
" vim.api.nvim_create_autocmd({ "BufRead", "BufNewFile" }, {
"   pattern = "*.rc",
"   command = "set filetype=rcsh",
" })
