#define CBUILD_ECHO_LEVEL 2		// Set echo level [0-3] (none to all):
								// 0 - no logging from library, only from chld processes.
								// 1 - default info from library, and chld processes.
								// 2 - additional info about calls with everyhting from 1 and 2.
								// 3 - everything + debuging info with a lot of data (recommended only for debugging purposes).
#define CBUILD_IMPLEMENTATION	// Enable implementations.
#include "./cbuild.h"

static void _usage(FILE* stream, const char* const program)
{
	ECHO(stream, "Usage [%s]: \n", program);
	ECHO(stream, "    --help / -h                Print usage to the terminal\n");
	ECHO(stream, "    --project / -p             Project title\n");
	ECHO(stream, "\n");
}

int _main(const char* const program, int argc, char** argv)
{
	const char* project = NULL;

	FOREACH_ARG_CMD_ARGS(flag, argc, argv,
	{
		if (STREQL(flag, "--project") OR STREQL(flag, "-p"))
		{
			project = _shift(&argc, &argv);
		}
		else if (STREQL(flag, "--help") OR STREQL(flag, "-h"))
		{
			_usage(stdout, program);
			exit(0);
		}
		else
		{
			_usage(stderr, program);
			exit(1);
		}
	});

	ECHO(stdout, CBUILD_INFO_LABEL" Building `%s`...\n", project);
	
	if (NOT ISDIR(PATH("examples"))) MKDIR("examples");
	if (NOT ISDIR(PATH("examples", project))) MKDIR("examples", project);
	if (NOT ISDIR(PATH("examples", project, "include"))) MKDIR("examples", project, "include");
	if (NOT ISDIR(PATH("examples", project, "source"))) MKDIR("examples", project, "source");
	if (NOT ISDIR(PATH("examples", project, "build"))) MKDIR("examples", project, "build");
	if (NOT ISDIR(PATH("examples", project, "tests"))) MKDIR("examples", project, "tests");

	ECHO(stdout, CBUILD_INFO_LABEL" Creating project hierarchy tree...\n", project);

	if (NOT ISFILE(PATH("examples", project, "readme.md"))) MKFILE("examples", project, "readme.md");
	if (NOT ISFILE(PATH("examples", project, "license"))) MKFILE("examples", project, "license");
	if (NOT ISFILE(PATH("examples", project, "include", "capp.h"))) MKFILE("examples", project, "include", "capp.h");
	if (NOT ISFILE(PATH("examples", project, "source", "capp.c"))) MKFILE("examples", project, "source", "capp.c");

	ECHO(stdout, CBUILD_INFO_LABEL" Adding initial files...\n", project);

#ifdef _WIN32
#	error "Windows are not supported yet!"
#else
	const char* const COMPILER = "cc";
	const char* const OUTPUT_FLAG = "-o";
	const char* const OUTPUT_PATH = PATH("examples", project, "build", "capp.out");
	const char* const SOURCES = PATH("examples", project, "source", "capp.c");
	CMD(COMPILER, OUTPUT_FLAG, OUTPUT_PATH, SOURCES);
	CMD(OUTPUT_PATH);
#endif

	return 0;
}

int main(int argc, char** argv)
{
	const char* program = _shift(&argc, &argv);
	REBUILD_MYSELF(program);
	int result = _main(program, argc, argv);
	return result;
}
