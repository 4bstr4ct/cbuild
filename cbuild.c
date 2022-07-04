#define CBUILD_ECHO_LEVEL 1		// Set echo level [0-3] (none to all):
								// 0 - no logging from library, only from chld processes.
								// 1 - default info from library, and chld processes.
								// 2 - additional info about calls with everyhting from 1 and 2.
								// 3 - everything + debuging info with a lot of data (recommended only for debugging purposes).
#define CBUILD_IMPLEMENTATION	// Enable implementations.
#define CBUILD_ENABLE_C_EXTENTION
#include "./cbuild.h"

static void _usage(FILE* stream, const char* const program)
{
	ECHO(stream, "Usage [%s]: \n", program);
	ECHO(stream, "    --help / -h                Print usage to the terminal\n");
	ECHO(stream, "    --compiler / -c            Path to C compiler executable\n");
	ECHO(stream, "    --optimize / -o            Optimize value [0-3]\n");
	ECHO(stream, "\n");
}

int _main(const char* const program, int argc, char** argv)
{
	const char* compiler = NULL;
	const char* optimize = NULL;
	const char* name = "capp";

	FOREACH_ARG_IN_CMD_ARGS(flag, argc, argv,
	{
		if (STREQL(flag, "--help") OR STREQL(flag, "-h"))
		{
			_usage(stdout, program);
			exit(0);
		}
		else if (STREQL(flag, "--compiler") OR STREQL(flag, "-c"))
		{
			compiler = _shift(&argc, &argv);
		}
		else if (STREQL(flag, "--optimize") OR STREQL(flag, "-o"))
		{
			optimize = _shift(&argc, &argv);
		}
		else
		{
			_usage(stderr, program);
			exit(1);
		}
	});

	ECHO(stdout, "=============================================================\n");
	ECHO(stdout, CBUILD_INFO_LABEL" Configuring build options...\n");
	ECHO(stdout, " -- "CBUILD_TRACE_LABEL" Compiler is set to: %s\n", compiler);
	ECHO(stdout, " -- "CBUILD_TRACE_LABEL" Optimization flag is set to: %s\n", optimize);
	ECHO(stdout, "=============================================================\n");

	ECHO(stdout, CBUILD_INFO_LABEL" Building name hierarchy tree...\n", name);
	if (NOT ISDIR(PATH("examples"))) MKDIR("examples");
	if (NOT ISDIR(PATH("examples", name))) MKDIR("examples", name);
	if (NOT ISDIR(PATH("examples", name, "include"))) MKDIR("examples", name, "include");
	if (NOT ISDIR(PATH("examples", name, "source"))) MKDIR("examples", name, "source");
	if (NOT ISDIR(PATH("examples", name, "build"))) MKDIR("examples", name, "build");
	if (NOT ISDIR(PATH("examples", name, "tests"))) MKDIR("examples", name, "tests");
	ECHO(stdout, "=============================================================\n");


	ECHO(stdout, CBUILD_INFO_LABEL" Adding initial files...\n", name);
	if (NOT ISFILE(PATH("examples", name, "readme.md"))) MKFILE("examples", name, "readme.md");
	if (NOT ISFILE(PATH("examples", name, "license"))) MKFILE("examples", name, "license");
	if (NOT ISFILE(PATH("examples", name, "include", "capp.h"))) MKFILE("examples", name, "include", "capp.h");
	if (NOT ISFILE(PATH("examples", name, "source", "capp.c"))) MKFILE("examples", name, "source", "capp.c");
	ECHO(stdout, "=============================================================\n");

	ECHO(stdout, CBUILD_INFO_LABEL" Setting up source files...\n");
	const char* sources = NULL;
	FOREACH_FILE_IN_DIRECTORY(file, PATH("examples", name, "source"),
	{
		IGNORE_DIRECTORY_IF_DOTS(file);
		if (ISFILE(PATH("examples", name, "source", file)))
		{
			ECHO(stdout, " -- "CBUILD_INFO_LABEL" Adding `%s` to sources.\n", PATH("examples", name, "source", file));
			sources = JOIN(" ", PATH("examples", name, "source", file));
		}
	});
	ECHO(stdout, "=============================================================\n");

#ifdef _WIN32
#	error "Windows are not supported yet!"
#else
	const char* const OUTPUT_FLAG = "-o";
	const char* const OUTPUT_PATH = PATH("examples", name, "build", "capp.out");
	CMD(compiler, OUTPUT_FLAG, OUTPUT_PATH, sources);
	ECHO(stdout, CBUILD_INFO_LABEL" "CBUILD_BOLD("Running the built executable:")"\n");
	CMD(OUTPUT_PATH);
#endif

	ECHO(stdout, "=============================================================\n");
	return 0;
}

int main(int argc, char** argv)
{
	const char* program = _shift(&argc, &argv);
	REBUILD_MYSELF(program);
	int result = _main(program, argc, argv);
	return result;
}
