# cbuild

Header-only, self-hosting library for creating build configurations for any type of projects. It is a [stb-style](https://github.com/nothings/stb/blob/master/docs/stb_howto.txt) single header library, which means, it does not include implementations by default.

The library provides utilities and functions to manipulate files, directories, and commands to create customized project building configurations.

## Builds situation

Currently tool:
1. Passed on [Fedora Silverblue 36](https://getfedora.org/en/silverblue/) GCC
2. Failed on Windows 10 MSVC
3. Was not tested on anything else

## How to

In order to use the library, you must define CBUILD_IMPLEMENTATION macro before including the header file:
```c
#define CBUILD_IMPLEMENTATION
#include "cbuild.h"
```

By default cbuild will log many building steps to the console. This can be disabled by defining CBUILD_NOECHO macro before the include of the cbuild header:
```c
#define CBUILD_IMPLEMENTATION
#define CBUILD_NOECHO
#include "cbuild.h"
```

For self-hosting, you must use REBUILD_MYSELF(program) macro. In [cbuild.c](./cbuild.c) file there is an example of how this macro is used. Program here is a path to the tool executable. Here is a snip of the example:
```c
int main(int argc, char** argv)
{
	REBUILD_MYSELF(argv[0]);
	// Do the build configuration logic here...
	return 0;
}
```

The initial run will require to build the tool, but after that, the tool will rebuild itself, if any changes are made in the .c file.
The steps of workign with the tool are:
1. Create a cbuild.c file and implement your build configuration in it using cbuild.h utilities and functions.
2. Build the tool using any C compiler you have.
3. Run the tool executable.
4. Change your configuration in .c file.
5. Run the tool executable without rebuilding it (it is an automatic process).

### Possible way to run on Windows:
```console
> cl.exe .\cbuild.c
> .\cbuild.exe
```

### And on Linux:
```console
> cc.exe ./cbuild.c -o ./cbuild.out
> ./cbuild.out
```

You can check out the (cbuild.c)[./cbuild.c] file to get the general idea of how cbuil.h is used.

## Warning
This is yet an experimental tool and has a very basic set of utilities. It currently does not provide functions for including directories for a target, linking etc.
Also, this whole tool is a giant memory leak! Memory managment will be added in the next patch.

## Thanks note
Thanks to [Tsoding](https://www.youtube.com/c/TsodingDaily) for inspiration and sources for such project. Especially for explaining the Windows side of the implementation in one of his videos!
