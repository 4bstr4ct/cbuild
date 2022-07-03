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
	CREATE_REGION();
	ADD_TO_REGION("path/to/something1");
	ADD_TO_REGION("path/to/something2");
	ADD_TO_REGION("path/to/something3");
	DESTROY_REGION();

	/*
	MKDIR(JOIN("!", "markas", "yra", "genijus"));
	FOREACH_ARG_CMD_ARGS(flag, argc, argv,
	{
		if (STREQL(flag, "--help") OR STREQL(flag, "-h"))
		{
			ECHO(STDOUT, INFO" Found flag %s: printing usage!\n", flag);
			_usage(STDOUT, program);
			exit(0);
		}
		else if (STREQL(flag, "--tests")
		 OR STREQL(flag, "-t"))
		{
			ECHO(STDOUT, INFO" Found flag %s: enabling tests!\n", flag);
		}
		else
		{
			ECHO(STDERR, ERROR" Found flag %s: enabling tests!\n", flag);
			_usage(STDERR, program);
			exit(1);
		}
	});

	MKDIR("examples", "test", "build", "bin");

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

	RM(PATH("examples", "test", "build", "bin"));
	*/

	return 0;
}

int main(int argc, char** argv)
{
	const char* program = _shift(&argc, &argv);
	REBUILD_MYSELF(program);
	int result = _main(program, argc, argv);
	return result;
}
