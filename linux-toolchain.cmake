# Clang Linux cross-compilation from Windows
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Specify Clang as the cross-compiler
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

# Target triple for Linux
set(CMAKE_C_COMPILER_TARGET x86_64-linux-gnu)
set(CMAKE_CXX_COMPILER_TARGET x86_64-linux-gnu)

# Where to find Linux libraries (you'll need to provide these)
#set(CMAKE_SYSROOT "C:/path/to/linux/sysroot")  # See note below
#set(CMAKE_FIND_ROOT_PATH ${CMAKE_SYSROOT})

# Adjust find commands behavior
#set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
#set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
#set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
