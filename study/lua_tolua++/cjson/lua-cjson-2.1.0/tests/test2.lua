#!/usr/bin/lua

function walkTable(t, fun)			-- 通过函数fun遍历表
	local func = fun or pairs
	for k, v in func(t) do
		print(k, v)
	end
end

function testCJson()
	local json = require 'cjson'
	local str = '["a", "b", "c"]'	-- json格式的字符串
	local j = json.decode(str)		-- 解码为表
	walkTable(j, ipairs)

	str = '{"A": 1, "B": 2}'
	j = json.decode(str)
	--walkTable(j, pairs)

	walkTable(j)
	j['C'] = 'c'
	local new_str = json.encode(j)
	print(new_str)
end

testCJson()
