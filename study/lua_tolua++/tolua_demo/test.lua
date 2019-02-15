studentB=Student:new() --实例化Student全局对象

function Run()
	studentB:Run();
end

function Run2(a)
	studentB:Run2(a);
end

function show()
	local b = {}
	local index

	for index = 1,10,2 do
		print(index)
	end
end

show()

Run()
Run2(20)
