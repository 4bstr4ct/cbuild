#ifndef BUILD_H
#define BUILD_H

#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#include <process.h>

struct dirent
{
	char d_name[MAX_PATH + 1];
};

struct DIR
{
	HANDLE hFind;
	WIN32_FIND_DATA data;
	struct dirent* dirent;
};

struct DIR* opendir(const char* dirpath)
{
	assert(dirpath);

	char buffer[MAX_PATH];
	snprintf(buffer, MAX_PATH, "%s\\*", dirpath);

	struct DIR* dir = (struct DIR*)malloc(sizeof(struct DIR));
	dir->hFind = FindFirstFile(buffer, &dir->data);

	if (dir->hFind == INVALID_HANDLE_VALUE)
	{
		if (dir)
		{
			free(dir);
		}

		return NULL;
	}
	else
	{
		return dir;
	}
}

struct dirent* readdir(struct DIR* dirp)
{
	assert(dirp);

	if (dirp->dirent == NULL)
	{
		dirp->dirent = (struct dirent*)malloc(sizoef(struct dirent));
	}
	else
	{
		if (!FindNextFile(dirp->hFind, &dirp->data))
		{
			return NULL;
		}
	}

	memset(dirp->dirent->d_name, 0, sizeof(dirp->dirent->d_name));
	strncpy(dirp->dirent->d_name, dirp->data.cFileName, sizeof(dirp->dirent->d_name) - 1);
}

void closedir(struct DIR* dirp)
{
	assert(dirp);

	FindClose(dirp->hFind);

	if (dirp->dirent)
	{
		free(dirp->dirent);
	}

	free(dirp);
}

LPSTR _lastErrorAsString(void)
{
	DWORD errorMessageId = GetLastError();
	assert(errorMessageId != 0);
	LPSTR messageBuffer = NULL;

	DWORD size =
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, // DWORD   dwFlags,
			NULL,
			errorMessageId,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPSTR) &messageBuffer,
			0,
			NULL
		);

	return messageBuffer;
}
#else
#	include <sys/types.h>
#	include <sys/stat.h>
#	include <sys/wait.h>
#	include <unistd.h>
#	include <dirent.h>
#endif

#ifndef STDOUT
#	define STDOUT stdout
#endif

#ifndef STDERR
#	define STDERR stderr
#endif

#ifndef TRACE
#	define TRACE "[TRACE]:"
#endif

#ifndef INFO
#	define INFO "[\033[1;32mINFO\033[0m]:"
#endif

#ifndef WARNING
#	define WARNING "[\033[1;33mWARNING\033[0m]:"
#endif

#ifndef ERROR
#	define ERROR "[\033[1;31mERROR\033[0m]:"
#endif

#ifndef BOLD
#	define BOLD(string) "\033[1m"string"\033[0m"
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

const char* _shift(int* argc, char*** argv);

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
const char* _join(const char* const separator, ...);

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
#	define CONCAT(...) JOIN("", __VA_ARGS__, NULL)
#endif

/**
 * Wraps @ref JOIN macro with predefined @ref PATH_SEPARATOR macro.
 * 
 * @code{.c}
 * 		const char* result = PATH("folder1", "something", "rock.rc");
 * @endcode
 */
#ifndef PATH
#	define PATH(...) JOIN(PATH_SEPARATOR, __VA_ARGS__, NULL)
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
int _exists(const char* const path);

/**
 * Wraps @ref _exists function.
 * 
 * @code{.c}
 * 		int result = EXISTS(PATH("folder1", "file.txt"));
 * @endcode
 */
#ifndef EXISTS
#	define EXISTS(path) \
	( \
		_exists(path) \
	)
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
 * Checks if path is a directory or not. Path must be crrectly formatted,
 * and can be provided with @ref PATH macro.
 * 
 * @code{.c}
 * 		bool is = _isdir(PATH("folder1", "folder2"));
 * @endcode
 */
int _isdir(const char* const path);

/**
 * Wraps @ref _isdir function.
 * 
 * @code{.c}
 * 		bool is = ISDIR(PATH("folder1", "folder2"));
 * @endcode
 */
#ifndef ISDIR
#	define ISDIR(path) \
	( \
		_isdir(path) \
	)
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
void _mv(const char* const source, const char* const destination);

/**
 * Wraps @ref _mv function.
 * 
 * @code{.c}
 * 		MV(PATH("folder1", "file.txt"), PATH("folder2", "file.txt"));
 * @endcode
 */
#ifndef MV
#	define MV(source, destination) \
	{ \
		INTERNAL_ECHO(stdout, "["BOLD("MV")"]: %s -> %s\n", source, destination); \
		_mv(source, destination); \
		INTERNAL_ECHO(stdout, "\n"); \
	}
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
void _mkdir(int ignore, ...);

/**
 * Wraps @ref _mkdir function and handles first and last arguments.
 * 
 * @code{.c}
 * 		MKDIR("first", "second", "third");
 * 		// Creates directories: 'first/', 'first/second/', and 'first/second/third/'
 * @endcode
 */
#ifndef MKDIR
#	define MKDIR(...) \
	{ \
		INTERNAL_ECHO(stdout, "["BOLD("MKDIR")"]: %s\n", JOIN(PATH_SEPARATOR, __VA_ARGS__)); \
		_mkdir(0, __VA_ARGS__, NULL); \
		INTERNAL_ECHO(stdout, "\n"); \
	}
#endif

/**
 * @}
 */



/**
 * @addtogroup RM
 * 
 * @{
 */

/**
 * Removes a file of directory with all it's subdirectories and files.
 * Path must be correctly formatted, and can be provided with @ref PATH
 * macro.
 * 
 * @code{.c}
 * 		_rm(PATH("folder1", "test", "file.txt"));
 * @endcode
 */
void _rm(const char* const path);

/**
 * Wraps @ref _rm function.
 * 
 * @code{.c}
 * 		RM(PATH("folder1", "test", "file.txt"));
 * @endcode
 */
#ifndef RM
#	define RM(path) \
	{ \
		INTERNAL_ECHO(stdout, "["BOLD("RM")"]: %s\n", path); \
		_rm(path); \
		INTERNAL_ECHO(stdout, "\n"); \
	}
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
void _cmd(int ignore, ...);

/**
 * Wraps @ref _cmd function.
 * 
 * @code{.c}
 * 		CMD("ls", "-la");
 * @endcode
 */
#ifndef CMD
#	define CMD(...) \
	{ \
		INTERNAL_ECHO(stdout, "["BOLD("CMD")"]: %s\n", JOIN(" ", __VA_ARGS__)); \
		_cmd(0, __VA_ARGS__, NULL); \
		INTERNAL_ECHO(stdout, "\n"); \
	}
#endif

/**
 * @}
 */



// Undocummented:
// TODO: Document!

/**
 * @addtogroup SELFBUILDER
 * 
 * @{
 */

/**
 * Checks if source file has any changes comparing to current built
 * vesrion of the tool.
 */
int _isCBuildModified(const char* sourcePath, const char* binaryPath);

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
void _rebuildMyself(const char* const sourcePath, const char* const binaryPath);

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



#ifdef CBUILD_IMPLEMENTATION

const char* _shift(int* argc, char*** argv)
{
	assert(*argc > 0);
	char* current = **argv;
	*argc -= 1;
	*argv += 1;
	return current;
}

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

int _exists(const char* const path)
{
#ifdef _WIN32
	DWORD dwAttrib = GetFileAttributes(path);
	return (dwAttrib != INVALID_FILE_ATTRIBUTES);
#else
	struct stat statbuf;

	if (stat(path, &statbuf) < 0)
	{
		if (errno == ENOENT)
		{
			errno = 0;
			return 0;
		}

		INTERNAL_ECHO(stderr, " -- "ERROR" Could not retrieve information about path %s: %s", path, strerror(errno));
		exit(1);
	}

	return 1;
#endif
}

int _isdir(const char* const path)
{
#ifdef _WIN32
	DWORD dwAttrib = GetFileAttributes(path);
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#else
	struct stat statbuf;

	if (stat(path, &statbuf) < 0)
	{
		if (errno == ENOENT)
		{
			errno = 0;
			return 0;
		}

		INTERNAL_ECHO(stderr, " -- "ERROR" Could not retrieve information about file %s: %s", path, strerror(errno));
		exit(1);
	}

	return S_ISDIR(statbuf.st_mode);
#endif
}

void _mv(const char* const source, const char* const destination)
{
#ifdef _WIN32
	if (!MoveFileEx(source, destination, MOVEFILE_REPLACE_EXISTING))
	{
		INTERNAL_ECHO(stderr, " -- "ERROR" Could not move %s to %s: %s\n", source, destination, _lastErrorAsString());
		exit(1);
	}
#else
	if (rename(source, destination) < 0)
	{
		INTERNAL_ECHO(stderr, " -- "ERROR" Could not move %s to %s: %s\n", source, destination, strerror(errno));
		exit(1);
	}
#endif
}

void _mkdir(int ignore, ...)
{
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

		if (mkdir(buffer, 0755) < 0)
		{
			if (errno == EEXIST)
			{
				INTERNAL_ECHO(stderr, " -- "WARNING" Directory %s already exists\n", buffer);
			}
			else
			{
				INTERNAL_ECHO(stderr, " -- "ERROR" Could not create directoy %s: %s\n", buffer, strerror(errno));
				exit(1);
			}
		}
	});

	free(buffer);
}

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
				INTERNAL_ECHO(stderr, " -- "WARNING" Directory %s does not exist!\n", path);
			}
			else
			{
				INTERNAL_ECHO(stderr, " -- "ERROR" Could not remove directory %s: %s\n", path, strerror(errno));
				exit(1);
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
				INTERNAL_ECHO(stderr, " -- "WARNING" File %s does not exist!\n", path);
			}
			else
			{
				INTERNAL_ECHO(stderr, " -- "ERROR" Could not remove file %s: %s", path, strerror(errno));
				exit(1);
			}
		}
	}
}

void _cmd(int ignore, ...)
{
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

#ifdef _WIN32
	intprt_t status = _spawnvp(_P_WAIT, argv[0], (char* const *)argv);

	if (status < 0)
	{
		INTERNAL_ECHO(stderr, " -- "ERROR" Could not execute child process: %s\n", _lastErrorAsString());
		exit(1);
	}

	if (status > 0)
	{
		INTERNAL_ECHO(stderr, " -- "ERROR" Child process exited with %d code\n", status);
		exit(1);
	}
#else
	pid_t childProcessId = fork();

	if (childProcessId == -1)
	{
		INTERNAL_ECHO(stderr, " -- "ERROR" Could not fork child process: %s\n", strerror(errno));
		exit(1);
	}

	if (childProcessId == 0)
	{
		if (execvp(argv[0], (char* const *)argv) < 0)
		{
			INTERNAL_ECHO(stderr, " -- "ERROR" Could not execute child process: %s\n", strerror(errno));
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
					INTERNAL_ECHO(stderr, " -- "ERROR" Child process exited with %d code\n", exitStatus);
					exit(1);
				}

				break;
			}

			if (WIFSIGNALED(status))
			{
				INTERNAL_ECHO(stderr, " -- "ERROR" Child process was terminated by %d signal\n", WTERMSIG(status));
				exit(1);
			}
		}
	}
#endif

	free(argv);
}

int _isCBuildModified(const char* sourcePath, const char* binaryPath)
{
#ifdef _WIN32
	FILETIME path1_time, path2_time;
	Fd path1_fd = fd_open_for_read(sourcePath);

	if (!GetFileTime(path1_fd, NULL, NULL, &path1_time))
	{
		INTERNAL_ECHO(stderr, " -- "ERROR" Could not get time of %s: %s\n", sourcePath, _lastErrorAsString());
		exit(1);
	}

	fd_close(path1_fd);
	Fd path2_fd = fd_open_for_read(binaryPath);

	if (!GetFileTime(path2_fd, NULL, NULL, &path2_time))
	{
		INTERNAL_ECHO(stderr, " -- "ERROR" Could not get time of %s: %s\n", binaryPath, _lastErrorAsString());
		exit(1);
	}

	fd_close(path2_fd);
	return CompareFileTime(&path1_time, &path2_time) == 1;
#else
	struct stat statbuf;

	if (stat(sourcePath, &statbuf) < 0)
	{
		INTERNAL_ECHO(stderr, " -- "ERROR" Could not stat %s: %s\n", sourcePath, strerror(errno));
		exit(1);
	}

	int path1_time = statbuf.st_mtime;

	if (stat(binaryPath, &statbuf) < 0)
	{
		INTERNAL_ECHO(stderr, " -- "ERROR" Could not stat %s: %s\n", binaryPath, strerror(errno));
		exit(1);
	}

	int path2_time = statbuf.st_mtime;
	return path1_time > path2_time;
#endif
}

void _rebuildMyself(const char* const sourcePath, const char* const binaryPath)
{
	if (_isCBuildModified(sourcePath, binaryPath))
	{
		INTERNAL_ECHO(stdout, INFO" Rebuilding CBUILD!\n");
		MV(binaryPath, CONCAT(binaryPath, ".old"));
		BUILD_MYSELF(binaryPath, sourcePath);
		RM(CONCAT(binaryPath, ".old"));
		CMD(binaryPath);
		exit(0); 
	}
}

#endif