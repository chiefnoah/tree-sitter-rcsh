local function heredoc_marker(line)
	local marker = line:match("<<%b[]%s*'([^']+)'%s*$")
		or line:match("<<%b[]%s*([^%s']+)%s*$")
		or line:match("<<%s*'([^']+)'%s*$")
		or line:match("<<%s*([^%s']+)%s*$")

	if marker == nil or marker:sub(1, 1) == "<" then
		return nil
	end

	return marker
end

local function register_marker(marker)
	local filetype = vim.filetype.match({ filename = "x." .. marker }) or marker
	local language = vim.treesitter.language.get_lang(filetype) or filetype

	if language ~= marker then
		pcall(vim.treesitter.language.register, language, marker)
	end
end

local function register_heredoc_languages(bufnr)
	for _, line in ipairs(vim.api.nvim_buf_get_lines(bufnr, 0, -1, false)) do
		local marker = heredoc_marker(line)
		if marker then
			register_marker(marker)
		end
	end
end

vim.api.nvim_create_autocmd({ "BufReadPost", "BufNewFile", "TextChanged", "TextChangedI" }, {
	group = vim.api.nvim_create_augroup("tree_sitter_rcsh_heredocs", { clear = true }),
	pattern = "*.rcsh",
	callback = function(args)
		register_heredoc_languages(args.buf)
	end,
})
