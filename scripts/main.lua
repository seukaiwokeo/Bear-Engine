local engine = Bear.init("chrome.exe");
data = engine:getThreads();
for index,value in ipairs(data) do
	print(string.format("Index: %d\tThread ID: 0x%08X", index, value))
end