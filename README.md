# Bear-Engine
A lua compiler to control windows processes based on C++ 11 win32

Developed under x86 Windows based machine.

This project based on C++ 11

**Simple Example**

```lua
-- Create a bear object
-- Process must be defined when initializing
local engine = Bear.init("something.exe")
-- Finding assembly code in the memory
data = engine:findPattern("dinput8.dll", "\x4F\x53")
-- Print the address of that assembly in console screen
print(string.format("Address: 0x%08X", data))
```

**Global Bear Functions**

- msg(string) -- Show a simple MessageBox.
```lua
engine:msg("Hello world!")
```

- kill() -- Kill the handled process.
```lua
engine:kill()
```

- suspend() -- Suspend the handled process.
```lua
engine:suspend()
```

- resume() -- Resume the handled process if it has been suspended.
```lua
engine:resume()
```

- sleep(unsigned integer) -- Sleep the handled process for a while.
```lua
-- sleep the process for 2000 ms (2 seconds)
engine:sleep(2000)
```

- load(string) -- Load other lua script in externally.
```lua
engine:load("scripts\\other.lua")
```

- loadLib(string) -- Inject a .dll to the handled process. [LoadLibrary]
```lua
engine:loadLib("folder\\lib.dll")
```

- getThreads() -- Get a list the threads of handled process.
```lua
data = engine:getThreads()
for index,value in ipairs(data) do
   print(string.format("Index: %d\tThread ID: 0x%08X", index, value))
end
```

- killThread(unsigned integer) -- Kill the thread that given ID
```lua
engine:killThread(0xA4)
```

- suspendThread(unsigned integer) -- Suspend the thread that given ID
```lua
engine:suspendThread(0xA1)
```

- resumeThread(unsigned integer) -- Resume the thread that has been suspended by given ID
```lua
engine:resumeThread(0xA1)
```

- getProcess(string) -- Return process Handle by name.
```lua
local otherProcess = engine:getProcess("something.exe")
print(string.format("The process is: 0x%08X\n", otherProcess))
```

- getModule(string) -- Return module handle by name.
```lua
local hModule = engine:getModule("dinput8.dll")
```

- findPattern(string, unsigned char array) -- Return the address of given assembly code block.
```lua
-- find the code block for example
--   0x‭45C61A‬      DEC     DI
--   0x‭45C61B      PUSH    BX
data = engine:findPattern("something.exe", "\x4F\x53")
-- data == 0x‭45C61A
```

- writeMemory(unsigned integer, integer) -- Write the given value to the memory of the given address in 4 bytes size.
```lua
engine:writeMemory(0x‭45C61B, 5‬)
```
- readMemory(unsigned integer, size of  address block) -- Read the given address from memory by the given size.
```lua
-- Read the 0x‭45C61B address from memory for 4 bytes size
local val = engine:readMemory(0x‭45C61B‬, 4)
-- Read the 0x‭45C61B address from memory for 2 bytes size
val = engine:readMemory(0x‭45C61B‬, 2)
```

- writeString(unsigned integer, string) -- Write the given string to the memory of the given address in based on the string length.
```lua
-- This function doesn't allocate a new memory block, it just write. It can cause a problem if the given string length exceed the size of the memory block.
engine:writeString(0x‭45C61B‬, "hello world")
```

- readString(unsigned integer, unsigned integer) -- Read the given address from memory by the size of the given length as a string.
```lua
local mystring = engine:readString(0x‭45C61B, 10‬)
print(string.format("%s", mystring))
```

- asmPatch(string, unsigned char array, unsigned char array) -- Change the asm codes the given code block.
```lua
--   0x‭45C61A‬      DEC     DI
--   0x‭45C61B      PUSH    BX
--   TO
--   0x‭45C61A‬      NOP
--   0x‭45C61B      NOP
engine:asmPatch("Engine.dll", "\x4F\x53", "\x90\x90") -- 4F 53 -> 90 90
```

- asmPatchEx(string, unsigned integer, unsigned char array) -- Change the asm codes the given address.
```lua
--   0x‭45C61A‬      DEC     DI
--   0x‭45C61B      PUSH    BX
--   TO
--   0x‭45C61A‬      NOP
--   0x‭45C61B      NOP
engine:asmPatchEx("Engine.dll", 0x‭45C61A‬, "\x90\x90") -- 45C61A -> 90 90
```