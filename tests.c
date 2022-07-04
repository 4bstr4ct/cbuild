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

#define TEST_BOOL(expression, label)	\
{ \
	fprintf(stdout, "%s\n", label); \
	fprintf(stdout, "%s => %d\n\n", #expression, expression); \
}

#define TEST_VOID(expression, label)	\
{ \
	fprintf(stdout, "%s\n", label); \
	fprintf(stdout, "%s\n\n", #expression); \
	expression; \
}

#define PAUSE { int c = getchar(); }

void tests()
{
	TEST_VOID(RM(PATH("test_cases")), "RM:");

	PAUSE

	TEST_BOOL(ISFILE(PATH("test_cases", "one")), "ISFILE:");
	TEST_BOOL(ISFILE(PATH("test_cases", "one", "something.txt")), "ISFILE:");
	TEST_BOOL(ISFILE(PATH("test_cases", "one", "main.c")), "ISFILE:");

	TEST_BOOL(ISDIR(PATH("test_cases", "one")), "ISDIR:");
	TEST_BOOL(ISDIR(PATH("test_cases", "one", "something.txt")), "ISDIR:");
	TEST_BOOL(ISDIR(PATH("test_cases", "one", "main.c")), "ISDIR:");

	TEST_BOOL(EXISTS(PATH("test_cases", "one")), "EXISTS:");
	TEST_BOOL(EXISTS(PATH("test_cases", "one", "something.txt")), "EXISTS:");
	TEST_BOOL(EXISTS(PATH("test_cases", "one", "main.c")), "EXISTS:");

	PAUSE

	TEST_VOID(MKDIR("test_cases", "one", "inner"), "MKDIR:");
	TEST_VOID(MKFILE(PATH("test_cases", "one", "config.json")), "MKFILE:");
	TEST_VOID(MKFILE("test_cases", "one", "inner", "text.txt"), "MKFILE:");

	PAUSE

	TEST_BOOL(ISFILE(PATH("test_cases", "one")), "ISFILE:");
	TEST_BOOL(ISFILE(PATH("test_cases", "one", "something.txt")), "ISFILE:");
	TEST_BOOL(ISFILE(PATH("test_cases", "one", "main.c")), "ISFILE:");

	TEST_BOOL(ISDIR(PATH("test_cases", "one")), "ISDIR:");
	TEST_BOOL(ISDIR(PATH("test_cases", "one", "something.txt")), "ISDIR:");
	TEST_BOOL(ISDIR(PATH("test_cases", "one", "main.c")), "ISDIR:");

	TEST_BOOL(EXISTS(PATH("test_cases", "one")), "EXISTS:");
	TEST_BOOL(EXISTS(PATH("test_cases", "one", "something.txt")), "EXISTS:");
	TEST_BOOL(EXISTS(PATH("test_cases", "one", "main.c")), "EXISTS:");

	PAUSE

#ifdef _WIN32
	TEST_VOID(CMD("gcc", "-o", PATH("examples", "test", "build", "main.out.exe"), PATH("examples", "test", "main.c")), "CMD:");
	TEST_VOID(CMD(PATH("examples", "test", "build", "main.out.exe"), "hello", "?", "what's the time?"), "CMD:");
#else
	TEST_VOID(CMD("cc", "-o", PATH("examples", "test", "build", "main.out"), PATH("examples", "test", "main.c")), "CMD:");
	TEST_VOID(CMD(PATH("examples", "test", "build", "main.out"), "hello", "?", "what's the time?"), "CMD:");
#endif

	PAUSE

	TEST_VOID(MKFILE(PATH("test_cases", "one", "note1.txt")), "MKFILE:");
	TEST_VOID(MKFILE(PATH("test_cases", "one", "note2.txt")), "MKFILE:");
	TEST_VOID(MKFILE(PATH("test_cases", "one", "note3.txt")), "MKFILE:");
	TEST_VOID(MKFILE(PATH("test_cases", "one", "note4.txt")), "MKFILE:");
	TEST_VOID(MKFILE(PATH("test_cases", "one", "note5.txt")), "MKFILE:");

	PAUSE

	FOREACH_FILE_IN_DIRECTORY(file, PATH("test_cases", "one"),
	{
		IGNORE_DIRECTORY_IF_DOTS(file);

		if (ISFILE(PATH("test_cases", "one", file)))
		{
			ECHO(stdout, CBUILD_INFO_LABEL" File: %s\n", file);
		}
		else if (ISDIR(PATH("test_cases", "one", file)))
		{
			ECHO(stdout, CBUILD_INFO_LABEL" Directory: %s\n", file);
		}
	});

	PAUSE
}

int main()
{
	tests();
	return 0;
}
