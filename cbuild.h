#ifndef BUILD_H
#define BUILD_H

#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#ifdef _WIN32
#	define WIN32_MEAN_AND_LEAN
#	include <windows.h>
#	include <process.h>

typedef HANDLE _ProcessId;
typedef HANDLE _FileHandle;

#	error "Windows is not yet supported!"

#else
#	include <sys/types.h>
#	include <sys/stat.h>
#	include <sys/wait.h>
#	include <unistd.h>
#	include <dirent.h>

typedef pid_t _ProcessId;
typedef int _FileHandle;
#endif



/**
 * @addtogroup RBMM
 * 
 * @{
 */

struct _RBMM_String
{
	char* buffer;
	unsigned long long length;
};

struct _RBMM_String* _RBMM_createString(const char* const buffer)
{
	struct _RBMM_String* string = (struct _RBMM_String*)malloc(sizeof(struct _RBMM_String));
	assert(string != NULL);
	string->length = strlen(buffer);
	string->buffer = (char*)malloc((string->length + 1) * sizeof(char));
	assert(string->buffer != NULL);
	memcpy(string->buffer, buffer, string->length);
	string->buffer[string->length + 1] = '\0';
	return string;
}

void _RBMM_destroyString(struct _RBMM_String* string)
{
	free(string->buffer);
	free(string);
}

struct _RBMM_Node
{
	struct _RBMM_String* string;
	struct _RBMM_Node* next;
};

struct _RBMM_Node* _RBMM_createNode(struct _RBMM_String* string, struct _RBMM_Node* next)
{
	struct _RBMM_Node* node = (struct _RBMM_Node*)malloc(sizeof(struct _RBMM_Node));
	assert(node != NULL);
	node->string = string;
	node->next = next;
	return node;
}

void _RBMM_destroyNode(struct _RBMM_Node* node)
{
	_RBMM_destroyString(node->string);
	free(node);
}

struct _RBMM_Region
{
	struct _RBMM_Node* root;
	struct _RBMM_Node* iterator;
	unsigned long long count;
};

static struct _RBMM_Region* __region = NULL;

void _RBMM_createRegion()
{
	__region = (struct _RBMM_Region*)malloc(sizeof(struct _RBMM_Region));
	assert(__region != NULL);
	__region->root = NULL;
	__region->iterator = NULL;
	__region->count = 0;
}

#ifndef CREATE_REGION
#	define CREATE_REGION() \
	{ \
		_RBMM_createRegion(); \
	}
#endif

void _RBMM_addToRegion(const char* const buffer)
{
	assert(__region != NULL);

	if (__region->root == NULL)
	{
		__region->root = (struct _RBMM_Node*)malloc(sizeof(struct _RBMM_Node));
		assert(__region->root != NULL);
		__region->root = _RBMM_createNode(_RBMM_createString(buffer), NULL);
		__region->iterator = __region->root;
	}
	else if (__region->iterator->next == NULL)
	{
		__region->iterator->next = (struct _RBMM_Node*)malloc(sizeof(struct _RBMM_Node));
		assert(__region->iterator->next != NULL);
		__region->iterator->next = _RBMM_createNode(_RBMM_createString(buffer), NULL);
		__region->iterator = __region->iterator->next;
	}

	++__region->count;
}

#ifndef ADD_TO_REGION
#	define ADD_TO_REGION(buffer) \
	{ \
		_RBMM_addToRegion(buffer); \
	}
#endif

void _RBMM_destroyRegion()
{
	assert(__region != NULL);
	__region->iterator = __region->root;

	while (__region->iterator != NULL)
	{
		struct _RBMM_Node* temp = __region->iterator;
		__region->iterator = __region->iterator->next;
		_RBMM_destroyNode(temp);
		--__region->count;
	}

	free(__region);
}

#ifndef DESTROY_REGION
#	define DESTROY_REGION() \
	{ \
		_RBMM_destroyRegion(); \
	}
#endif

/**
 * @}
 */



#ifndef STDOUT
#	define STDOUT stdout
#endif

#ifndef STDERR
#	define STDERR stderr
#endif

#ifndef CBUILD_TRACE
#	define CBUILD_TRACE "[CBUILD_TRACE]:"
#endif

#ifndef CBUILD_INFO
#	define CBUILD_INFO "[\033[1;32mINFO\033[0m]:"
#endif

#ifndef CBUILD_WARNING
#	define CBUILD_WARNING "[\033[1;33mWARNING\033[0m]:"
#endif

#ifndef CBUILD_ERROR
#	define CBUILD_ERROR "[\033[1;31mERROR\033[0m]:"
#endif

#ifndef CBUILD_BOLD
#	define CBUILD_BOLD(string) "\033[1m"string"\033[0m"
#endif

#ifndef ECHO
#	define ECHO(...) fprintf(__VA_ARGS__);
#endif

#ifndef INTERNAL_ECHO
#	ifdef CBUILD_NOECHO
#		define INTERNAL_ECHO(...)
#	else
#		define INTERNAL_ECHO(...) fprintf(__VA_ARGS__);
#	endif
#endif

#ifndef AND
#	define AND &&
#endif

#ifndef OR
#	define OR ||
#endif

#ifndef STREQL
#	define STREQL(a, b) \
	( \
		strcmp(a, b) == 0 \
	)
#endif

#ifndef PATH_SEPARATOR
#	ifdef _WIN32
#		define PATH_SEPARATOR "\\"
#	else
#		define PATH_SEPARATOR "/"
#	endif
#endif

#ifndef PATH_SEPARATOR_LENGTH
#	define PATH_SEPARATOR_LENGTH (sizeof(PATH_SEPARATOR) - 1)
#endif

const char* _shift(int* argc, char*** argv)
{
	assert(*argc > 0);
	char* current = **argv;
	*argc -= 1;
	*argv += 1;
	return current;
}

#ifndef FOREACH_ARG_CMD_ARGS
#	define FOREACH_ARG_CMD_ARGS(argument, count, arguments, body) \
	{ \
		while (argc > 0) \
		{ \
			const char* argument = _shift(&argc, &argv); \
			body; \
		} \
	}
#endif

#ifndef FOREACH_ARG_IN_VA_ARGS
#	define FOREACH_ARG_IN_VA_ARGS(ignore, type, argument, arguments, body) \
	{ \
		va_start(arguments, ignore); \
		 \
		for (type arg = va_arg(arguments, type); arg != NULL; arg = va_arg(arguments, type)) \
		{ \
			body; \
		} \
		 \
		va_end(arguments); \
	}
#endif

#ifndef FOREACH_ITEM_IN_ARRAY
#	define FOREACH_ITEM_IN_ARRAY(type, item, items, body) \
	{ \
		for (unsigned long long index = 0; index < sizeof(items) / sizeof((items)[0]); ++index) \
		{ \
			type item = items[index]; \
			body; \
		} \
	}
#endif

#ifndef IGNORE_IF_DOTS
#	define IGNORE_IF_DOTS(file) \
	{ \
		if (STREQL(file, ".") OR STREQL(file, "..")) \
		{ \
			continue; \
		} \
	}
#endif

#ifndef FOREACH_FILE_IN_DIRECTORY
#	define FOREACH_FILE_IN_DIRECTORY(file, directory, body) \
	{ \
		struct dirent* dp = NULL; \
		DIR* dir = opendir(directory); \
		 \
		while ((dp = readdir(dir))) \
		{ \
			const char* file = dp->d_name; \
			body; \
		} \
	}
#endif



/**
 * @addtogroup STRUTILS
 * 
 * @{
 */

/**
 * Joins variadic arguments with provided separator into a single
 * string. The variadic list must be a NULL terminated!
 * 
 * @code{.c}
 * 		const char* result = _join("%", "folder1", "something", "rock.rc", NULL);
 * @endcode
 */
const char* _join(const char* const separator, ...)
{
	const unsigned long long separatorLength = strlen(separator);
	signed long long separatorsCount = -1;
	unsigned long long length = 0;
	va_list args;

	FOREACH_ARG_IN_VA_ARGS(separator, const char*, arg, args,
	{
		length += strlen(arg);
		++separatorsCount;
	});

	char* buffer = (char*)malloc((length + separatorsCount * separatorLength + 1) * sizeof(char));
	length = 0;

	FOREACH_ARG_IN_VA_ARGS(separator, const char*, arg, args,
	{
		const unsigned long long argLength = strlen(arg);
		memcpy(buffer + length, arg, argLength);
		length += argLength;

		if (separatorsCount > 0)
		{
			memcpy(buffer + length, separator, separatorLength);
			length += separatorLength;
			--separatorsCount;
		}
	});

	buffer[length] = '\0';
	return buffer;
}

/**
 * Wraps @ref _join function.
 * 
 * @code{.c}
 * 		const char* result = JOIN("%", "folder1", "something", "rock.rc");
 * @endcode
 */
#ifndef JOIN
#	define JOIN(separator, ...) _join(separator, __VA_ARGS__, NULL)
#endif

/**
 * Wraps @ref JOIN macro with predefined separator to an empty string ("").
 * 
 * @code{.c}
 * 		const char* result = CONCAT("folder1", "something", "rock.rc");
 * @endcode
 */
#ifndef CONCAT
#	define CONCAT(...) _join("", __VA_ARGS__, NULL)
#endif

/**
 * Wraps @ref JOIN macro with predefined @ref PATH_SEPARATOR macro.
 * 
 * @code{.c}
 * 		const char* result = PATH("folder1", "something", "rock.rc");
 * @endcode
 */
#ifndef PATH
#	define PATH(...) _join(PATH_SEPARATOR, __VA_ARGS__, NULL)
#endif

/**
 * @}
 */



/**
 * @addtogroup ISFILE
 * 
 * @{
 */

/**
 * Checks if path is a file or not. Path must be correctly formatted,
 * and can be provided with @ref PATH macro.
 * 
 * @code{.c}
 * 		bool is = _isfile(PATH("folder1", "folder2"));
 * @endcode
 */
int _isfile(const char* const path)
{
#ifdef _WIN32
	assert(!"TODO: implement _isfile with Windows WIN32 API!");
#else
	struct stat info;
	const int result = (stat(path, &info) == 0) && (info.st_mode & S_IFREG);
	return result;
#endif
}

/**
 * Wraps @ref _isfile function.
 * 
 * @code{.c}
 * 		bool is = ISFILE(PATH("folder1", "folder2"));
 * @endcode
 */
#ifndef ISFILE
#	define ISFILE(path) _isfile(path)
#endif

/**
 * @}
 */



/**
 * @addtogroup ISDIR
 * 
 * @{
 */

/**
 * Checks if path is a directory or not. Path must be correctly formatted,
 * and can be provided with @ref PATH macro.
 * 
 * @code{.c}
 * 		bool is = _isdir(PATH("folder1", "folder2"));
 * @endcode
 */
int _isdir(const char* const path)
{
#ifdef _WIN32
	assert(!"TODO: implement _isdir with Windows WIN32 API!");
#else
	struct stat info;
	const int result = (stat(path, &info) == 0) && (info.st_mode & S_IFDIR);
	return result;
#endif
}

/**
 * Wraps @ref _isdir function.
 * 
 * @code{.c}
 * 		bool is = ISDIR(PATH("folder1", "folder2"));
 * @endcode
 */
#ifndef ISDIR
#	define ISDIR(path) _isdir(path)
#endif

/**
 * @}
 */



/**
 * @addtogroup EXISTS
 * 
 * @{
 */

/**
 * Chesks if provided path exists ir not. Path must be correctly
 * formatted, and can be provided using @ref PATH macro.
 * 
 * @code{.c}
 * 		int result = _exists(PATH("folder1", "file.txt"));
 * @endcode
 */
int _exists(const char* const path)
{
#ifdef _WIN32
	assert(!"TODO: implement _exists with Windows WIN32 API!");
#else
	const int result = _isfile(path) || _isdir(path);
	return result;
#endif
}

/**
 * Wraps @ref _exists function.
 * 
 * @code{.c}
 * 		int result = EXISTS(PATH("folder1", "file.txt"));
 * @endcode
 */
#ifndef EXISTS
#	define EXISTS(path) _exists(path)
#endif

/**
 * @}
 */



/**
 * @addtogroup MKFILE
 * 
 * @{
 */

/**
 * Creates a file and all subdirectories in the provided path.
 * The path must be a NULL terminated variadic list of strings, not
 * a regular path format!
 * 
 * @code{.c}
 * 		_mkfile(0, "first", "second", "third.txt", NULL);
 * 		// Creates directories: 'first/', 'first/second/', and 'first/second/third.txt'
 * @endcode
 */
void _mkfile(int ignore, ...)
{
#ifdef _WIN32
	assert(!"TODO: implement _mkfile with Windows WIN32 API!");
#else

	signed long long separatorsCount = -1;
	unsigned long long length = 0;
	va_list args;

	FOREACH_ARG_IN_VA_ARGS(ignore, const char*, arg, args,
	{
		length += strlen(arg);
		++separatorsCount;
	});

	char* buffer = (char*)malloc((length + separatorsCount * PATH_SEPARATOR_LENGTH + 1) * sizeof(char));
	length = 0;

	FOREACH_ARG_IN_VA_ARGS(ignore, const char*, arg, args,
	{
		const unsigned long long argLength = strlen(arg);
		memcpy(buffer + length, arg, argLength);
		length += argLength;

		if (separatorsCount > 0)
		{
			memcpy(buffer + length, PATH_SEPARATOR, PATH_SEPARATOR_LENGTH);
			length += PATH_SEPARATOR_LENGTH;
			--separatorsCount;
		}

		buffer[length] = '\0';

		if (fopen(buffer, "w") < 0)
		{
			if (errno == EEXIST)
			{
				INTERNAL_ECHO(stderr, " -- "CBUILD_WARNING" Directory %s already exists\n", buffer);
			}
			else
			{
				INTERNAL_ECHO(stderr, " -- "CBUILD_ERROR" Could not  create a file %s: %s\n", buffer, strerror(errno));
				free(buffer);
				exit(1);
			}
		}
	});

	free(buffer);
#endif
}

/**
 * Wraps @ref _mkfile function and handles first and last arguments.
 * 
 * @code{.c}
 * 		MKFILE("first", "second", "third.txt");
 * 		// Creates directories: 'first/', 'first/second/', and 'first/second/third.txt'
 * @endcode
 */
#ifndef MKFILE
#	define MKFILE(...) _mkfile(0, __VA_ARGS__, NULL)
#endif

/**
 * @}
 */



/**
 * @addtogroup MKDIR
 * 
 * @{
 */

/**
 * Creates a directory and all subdirectories in the provided path.
 * The path must be a NULL terminated variadic list of strings, not
 * a regular path format!
 * 
 * @code{.c}
 * 		_mkdir(0, "first", "second", "third", NULL);
 * 		// Creates directories: 'first/', 'first/second/', and 'first/second/third/'
 * @endcode
 */
void _mkdir(int ignore, ...)
{
#ifdef _WIN32
	assert(!"TODO: implement _mkdir with Windows WIN32 API!");
#else
	signed long long separatorsCount = -1;
	unsigned long long length = 0;
	va_list args;

	FOREACH_ARG_IN_VA_ARGS(ignore, const char*, arg, args,
	{
		length += strlen(arg);
		++separatorsCount;
	});

	char* buffer = (char*)malloc((length + separatorsCount * PATH_SEPARATOR_LENGTH + 1) * sizeof(char));
	length = 0;

	FOREACH_ARG_IN_VA_ARGS(ignore, const char*, arg, args,
	{
		const unsigned long long argLength = strlen(arg);
		memcpy(buffer + length, arg, argLength);
		length += argLength;

		if (separatorsCount > 0)
		{
			memcpy(buffer + length, PATH_SEPARATOR, PATH_SEPARATOR_LENGTH);
			length += PATH_SEPARATOR_LENGTH;
			--separatorsCount;
		}

		buffer[length] = '\0';

		if (mkdir(buffer, 0777) < 0)
		{
			if (errno == EEXIST)
			{
				INTERNAL_ECHO(stderr, " -- "CBUILD_WARNING" Directory %s already exists\n", buffer);
			}
			else
			{
				INTERNAL_ECHO(stderr, " -- "CBUILD_ERROR" Could not create directoy %s: %s\n", buffer, strerror(errno));
				free(buffer);
				exit(1);
			}
		}
	});

	free(buffer);
#endif
}

/**
 * Wraps @ref _mkdir function and handles first and last arguments.
 * 
 * @code{.c}
 * 		MKDIR("first", "second", "third");
 * 		// Creates directories: 'first/', 'first/second/', and 'first/second/third/'
 * @endcode
 */
#ifndef MKDIR
#	define MKDIR(...) _mkdir(0, __VA_ARGS__, NULL)
#endif

/**
 * @}
 */



/**
 * @addtogroup RM
 * 
 * @{
 */

void _rm(const char* const path)
{
	if (_isdir(path))
	{
		FOREACH_FILE_IN_DIRECTORY(file, path,
		{
			IGNORE_IF_DOTS(file);
			_rm(PATH(path, file));
		});

		if (rmdir(path) < 0)
		{
			if (errno == ENOENT)
			{
				errno = 0;
				INTERNAL_ECHO(stdout, CBUILD_WARNING" Directory %s does not exist", path);
			}
			else
			{
				INTERNAL_ECHO(stderr, CBUILD_ERROR" Could not remove directory %s: %s", path, strerror(errno));
			}
		}
	}
	else
	{
		if (unlink(path) < 0)
		{
			if (errno == ENOENT)
			{
				errno = 0;
				INTERNAL_ECHO(stdout, CBUILD_WARNING" File %s does not exist!\n", path);
			}
			else
			{
				INTERNAL_ECHO(stderr, CBUILD_ERROR" Could not remove file %s: %s", path, strerror(errno));
			}
		}
	}
}

/**
 * Wraps @ref _rm function.
 * 
 * @code{.c}
 * 		RM(PATH("folder1", "file.txt"));
 * @endcode
 */
#ifndef RM
#	define RM(path) _rm(path)
#endif

/**
 * @}
 */



/**
 * @addtogroup MV
 * 
 * @{
 */

/**
 * Moves a file to other location. It can effectively rename a file.
 * Source and destination are paths that can be formatted with @ref PATH
 * macro.
 * 
 * @code{.c}
 * 		_mv(PATH("folder1", "file.txt"), PATH("folder2", "file.txt"));
 * @endcode
 */
void _mv(const char* const source, const char* const destination)
{
#ifdef _WIN32
	assert(!"TODO: implement _mv with Windows WIN32 API!");
#else
	if (rename(source, destination) < 0)
	{
		INTERNAL_ECHO(stderr, " -- "CBUILD_ERROR" Could not move %s to %s: %s\n", source, destination, strerror(errno));
		exit(1);
	}
#endif
}

/**
 * Wraps @ref _mv function.
 * 
 * @code{.c}
 * 		MV(PATH("folder1", "file.txt"), PATH("folder2", "file.txt"));
 * @endcode
 */
#ifndef MV
#	define MV(source, destination) _mv(source, destination)
#endif

/**
 * @}
 */



/**
 * @addtogroup CMD
 * 
 * @{
 */

/**
 * Calls a command line command as a child process. It requires the whole
 * command to be provided either separates or not as a variadic arguments.
 * Last parameter must be NULL!
 * 
 * @code{.c}
 * 		_cmd("ls", "-la");
 * 		// or
 * 		_cmd("ls -la");
 * @endcode
 */
void _cmd(int ignore, ...)
{
#ifdef _WIN32
	assert(!"TODO: implement _cmd with Windows WIN32 API!");
#else
	unsigned long long argc = 0;
	va_list args;

	FOREACH_ARG_IN_VA_ARGS(ignore, const char*, arg, args,
	{
		++argc;
	});

	assert(argc >= 1);
	const char** argv = (const char**)malloc((argc + 1) * sizeof(const char*));
	argc = 0;

	FOREACH_ARG_IN_VA_ARGS(ignore, const char*, arg, args,
	{
		argv[argc++] = arg;
	});

	argv[argc] = NULL;
	assert(argc >= 1);
	pid_t childProcessId = fork();

	if (childProcessId == -1)
	{
		INTERNAL_ECHO(stderr, " -- "CBUILD_ERROR" Could not fork child process: %s\n", strerror(errno));
		exit(1);
	}

	if (childProcessId == 0)
	{
		if (execvp(argv[0], (char* const *)argv) < 0)
		{
			INTERNAL_ECHO(stderr, " -- "CBUILD_ERROR" Could not execute child process: %s\n", strerror(errno));
			exit(1);
		}
	}
	else
	{
		for (;;)
		{
			int status = 0;
			wait(&status);

			if (WIFEXITED(status))
			{
				const int exitStatus = WEXITSTATUS(status);

				if (exitStatus != 0)
				{
					INTERNAL_ECHO(stderr, " -- "CBUILD_ERROR" Child process exited with %d code\n", exitStatus);
					exit(1);
				}

				break;
			}

			if (WIFSIGNALED(status))
			{
				INTERNAL_ECHO(stderr, " -- "CBUILD_ERROR" Child process was terminated by %d signal\n", WTERMSIG(status));
				exit(1);
			}
		}
	}

	free(argv);
#endif
}

/**
 * Wraps @ref _cmd function.
 * 
 * @code{.c}
 * 		CMD("ls", "-la");
 * @endcode
 */
#ifndef CMD
#	define CMD(...) _cmd(0, __VA_ARGS__, NULL)
#endif

/**
 * @}
 */



/**
 * @addtogroup SELFBUILDER
 * 
 * @{
 */

/**
 * Checks if source file has any changes comparing to current built
 * vesrion of the tool.
 */
int _isCBuildModified(const char* sourcePath, const char* binaryPath)
{
#ifdef _WIN32
	assert(!"TODO: implement _isCBuildModified with Windows WIN32 API!");
#else
	struct stat info;

	if (stat(sourcePath, &info) < 0)
	{
		INTERNAL_ECHO(stderr, " -- "CBUILD_ERROR" Could not stat %s: %s\n", sourcePath, strerror(errno));
		exit(1);
	}

	int path1Time = info.st_mtime;

	if (stat(binaryPath, &info) < 0)
	{
		INTERNAL_ECHO(stderr, " -- "CBUILD_ERROR" Could not stat %s: %s\n", binaryPath, strerror(errno));
		exit(1);
	}

	int path2Time = info.st_mtime;
	return path1Time > path2Time;
#endif
}

/**
 * Intermediate step - actual building of the new executable.
 */
#ifndef BUILD_MYSELF
#	if _WIN32
#		if defined(__GNUC__)
#			define BUILD_MYSELF(binaryPath, sourcePath) CMD("gcc", "-o", binaryPath, sourcePath)
#		elif defined(__clang__)
#			define BUILD_MYSELF(binaryPath, sourcePath) CMD("clang", "-o", binaryPath, sourcePath)
#		elif defined(_MSC_VER)
#			define BUILD_MYSELF(binaryPath, sourcePath) CMD("cl.exe", sourcePath)
#		endif
# 	else
#		define BUILD_MYSELF(binaryPath, sourcePath) CMD("cc", "-o", binaryPath, sourcePath)
# 	endif
#endif

/**
 * Starts the rebuilding process for the tool.
 */
void _rebuildMyself(const char* const sourcePath, const char* const binaryPath)
{
	if (_isCBuildModified(sourcePath, binaryPath))
	{
		INTERNAL_ECHO(stdout, CBUILD_INFO" Rebuilding CBUILD!\n");
		MV(binaryPath, CONCAT(binaryPath, ".old"));
		BUILD_MYSELF(binaryPath, sourcePath);
		RM(CONCAT(binaryPath, ".old"));
		CMD(binaryPath);
		exit(0); 
	}
}

/**
 * Wraps @ref _rebuildMyself function.
 */
#ifndef REBUILD_MYSELF
#	define REBUILD_MYSELF(program) \
	{ \
		const char* sourcePath = __FILE__; \
		const char* binaryPath = program; \
		_rebuildMyself(sourcePath, binaryPath); \
	}
#endif

/**
 * @}
 */

#endif
