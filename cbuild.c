// #define CBUILD_NOECHO			// Disable echo
#define CBUILD_IMPLEMENTATION	// Enable implementations
#include "./cbuild.h"

static void _usage(FILE* stream, const char* const program)
{
	ECHO(stream, "Usage [%s]: \n", program);
	ECHO(stream, "    --help / -h                Print usage to the terminal\n");
	ECHO(stream, "    --tests / -t               Enable tests\n");
	ECHO(stream, "\n");
}

int _main(const char* const program, int argc, char** argv)
{
	fprintf(STDOUT, "%d\n", ISFILE(PATH("examples", "test")));
	fprintf(STDOUT, "%d\n", ISFILE(PATH("examples", "test", "something.txt")));
	fprintf(STDOUT, "%d\n\n", ISFILE(PATH("examples", "test", "main.c")));

	fprintf(STDOUT, "%d\n", ISDIR(PATH("examples", "test")));
	fprintf(STDOUT, "%d\n", ISDIR(PATH("examples", "test", "something.txt")));
	fprintf(STDOUT, "%d\n\n", ISDIR(PATH("examples", "test", "main.c")));

	fprintf(STDOUT, "%d\n", EXISTS(PATH("examples", "test")));
	fprintf(STDOUT, "%d\n", EXISTS(PATH("examples", "test", "something.txt")));
	fprintf(STDOUT, "%d\n\n", EXISTS(PATH("examples", "test", "main.c")));

	FOREACH_ARG_CMD_ARGS(flag, argc, argv,
	{
		if (STREQL(flag, "--help") OR STREQL(flag, "-h"))
		{
			ECHO(STDOUT, CBUILD_INFO" Found flag %s: printing usage!\n", flag);
			_usage(STDOUT, program);
			exit(0);
		}
		else if (STREQL(flag, "--tests")
		 OR STREQL(flag, "-t"))
		{
			ECHO(STDOUT, CBUILD_INFO" Found flag %s: enabling tests!\n", flag);
		}
		else
		{
			ECHO(STDERR, CBUILD_ERROR" Found flag %s: enabling tests!\n", flag);
			_usage(STDERR, program);
			exit(1);
		}
	});

	MKDIR("examples", "test", "build", "bin");
	MKFILE("examples", "test", "build", "bin", "something.txt");

#if _WIN32
	CMD("cl.exe",
		PATH("examples", "test", "main.c"))
#else
	CMD("cc",
		"-o",
		PATH("examples", "test", "build", "bin", "main.c.out"),
		PATH("examples", "test", "main.c"));
	CMD(PATH("examples", "test", "build", "bin", "main.c.out"));
#endif

	RM(PATH("CMakeLists.txt"));

	return 0;
}

int main(int argc, char** argv)
{
	const char* program = _shift(&argc, &argv);
	REBUILD_MYSELF(program);
	int result = _main(program, argc, argv);
	return result;
}
