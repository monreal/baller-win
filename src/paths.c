/*
 * Ballerburg - paths.c
 *
 * Set up the various path strings.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * (http://www.gnu.org/licenses/) for more details.
 */
const char Paths_fileid[] = "Ballerburg paths.c : " __DATE__ " " __TIME__;

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "config.h"
#include "paths.h"

#ifdef WIN32
#define PATHSEP '\\'
#define realpath(N,R) _fullpath((R),(N),_MAX_PATH)
#else
#define PATHSEP '/'
#endif


static char sWorkingDir[FILENAME_MAX];    /* Working directory */
static char sDataDir[FILENAME_MAX];       /* Directory where data files can be found */
static char sLocaleDir[FILENAME_MAX];     /* Directory where locale files can be found */

/**
 * Return pointer to current working directory string
 */
const char *Paths_GetWorkingDir(void)
{
	return sWorkingDir;
}

/**
 * Return pointer to data directory string
 */
const char *Paths_GetDataDir(void)
{
	return sDataDir;
}

/**
 * Return pointer to data directory string
 */
const char *Paths_GetLocaleDir(void)
{
	return sLocaleDir;
}

/**
 * Return TRUE if file exists, is readable or writable at least and is not
 * a directory.
 */
static bool Paths_FileExists(const char *filename)
{
	struct stat buf;
	if (stat(filename, &buf) == 0 &&
	    (buf.st_mode & (S_IRUSR|S_IWUSR)) && !(buf.st_mode & S_IFDIR))
	{
		/* file points to user readable regular file */
		return true;
	}
	return false;
}


/**
 * Explore the PATH environment variable to see where our executable is
 * installed.
 */
static void Paths_GetExecDirFromPATH(const char *argv0, char *pExecDir, int nMaxLen)
{
	char *pPathEnv;
	char *pAct;
	char *pTmpName;
	const char *pToken;

	/* Get the PATH environment string */
	pPathEnv = getenv("PATH");
	if (!pPathEnv)
		return;
	/* Duplicate the string because strtok destroys it later */
	pPathEnv = strdup(pPathEnv);
	if (!pPathEnv)
		return;

	pTmpName = malloc(FILENAME_MAX);
	if (!pTmpName)
	{
		free(pPathEnv);
		return;
	}

	/* If there is a semicolon in the PATH, we assume it is the PATH
	 * separator token (like on Windows), otherwise we use a colon. */
	if (strchr((pPathEnv), ';'))
		pToken = ";";
	else
		pToken = ":";

	pAct = strtok (pPathEnv, pToken);
	while (pAct)
	{
		snprintf(pTmpName, FILENAME_MAX, "%s%c%s",
		         pAct, PATHSEP, argv0);
		if (Paths_FileExists(pTmpName))
		{
			/* Found the executable - so use the corresponding path: */
			strncpy(pExecDir, pAct, nMaxLen);
			pExecDir[nMaxLen-1] = 0;
			break;
		}
		pAct = strtok (0, pToken);
  	}

	free(pPathEnv);
	free(pTmpName);
}


/**
 * Locate the directory where the executable resides
 */
static char *Paths_InitExecDir(const char *argv0)
{
	char *psExecDir;  /* Path string where the executable can be found */

	/* Allocate memory for storing the path string of the executable */
	psExecDir = malloc(FILENAME_MAX);
	if (!psExecDir)
	{
		fprintf(stderr, "Out of memory (Paths_Init)\n");
		exit(-1);
	}

	/* Determine the bindir...
	 * Start with empty string, then try to use OS specific functions,
	 * and finally analyze the PATH variable if it has not been found yet. */
	psExecDir[0] = '\0';

#if defined(__linux__)
	{
		int i;
		/* On Linux, we can analyze the symlink /proc/self/exe */
		i = readlink("/proc/self/exe", psExecDir, FILENAME_MAX);
		if (i > 0)
		{
			char *p;
			psExecDir[i] = '\0';
			p = strrchr(psExecDir, '/');    /* Search last slash */
			if (p)
				*p = 0;                     /* Strip file name from path */
		}
	}
//#elif defined(WIN32) || defined(__CEGCC__)
//	/* On Windows we can use GetModuleFileName for getting the exe path */
//	GetModuleFileName(NULL, psExecDir, FILENAME_MAX);
#endif

	/* If we do not have the execdir yet, analyze argv[0] and the PATH: */
	if (psExecDir[0] == 0)
	{
		if (strchr(argv0, PATHSEP) == 0)
		{
			/* No separator in argv[0], we have to explore PATH... */
			Paths_GetExecDirFromPATH(argv0, psExecDir, FILENAME_MAX);
		}
		else
		{
			/* There was a path separator in argv[0], so let's assume a
			 * relative or absolute path to the current directory in argv[0] */
			char *p;
			strncpy(psExecDir, argv0, FILENAME_MAX);
			psExecDir[FILENAME_MAX-1] = 0;
			p = strrchr(psExecDir, PATHSEP);  /* Search last slash */
			if (p)
				*p = 0;                       /* Strip file name from path */
		}
	}

	return psExecDir;
}


/**
 * Initialize the data directory string
 */
static void Paths_InitPackageDir(char *psPkgDir, const char *psRelPath,
                                 const char *psExecDir)
{
	char *pTempName;

	if (psExecDir && strlen(psExecDir) > 0)
	{
		snprintf(psPkgDir, FILENAME_MAX, "%s%c%s",
		         psExecDir, PATHSEP, psRelPath);
	}
	else
	{
		/* bindir could not be determined, let's assume the destination dir is
		 * relative to current working directory... */
		strcpy(psPkgDir, psRelPath);
	}

	pTempName = malloc(FILENAME_MAX);
	if (!pTempName)
	{
		perror("Init data dir malloc");
		return;
	}

	if (realpath(psPkgDir, pTempName) != NULL)
	{
		strncpy(psPkgDir, pTempName, FILENAME_MAX);
	}

	free(pTempName);
}


/**
 * Initialize directory names
 *
 * The datadir will be initialized relative to the bindir (where the executable
 * has been installed to). This means a lot of additional effort since we first
 * have to find out where the executable is. But thanks to this effort, we get
 * a relocatable package (we don't have any absolute path names in the program)!
 */
void Paths_Init(const char *argv0)
{
	char *psExecDir;  /* Path string where the executable can be found */

	/* Init working directory string */
	if (getcwd(sWorkingDir, FILENAME_MAX) == NULL)
	{
		/* This should never happen... just in case... */
		strcpy(sWorkingDir, ".");
	}

	/* Get the directory where the executable resides */
	psExecDir = Paths_InitExecDir(argv0);

	/* Now create the package path names from the bindir path name: */
	Paths_InitPackageDir(sDataDir, BIN2DATADIR, psExecDir);
	Paths_InitPackageDir(sLocaleDir, BIN2LOCALEDIR, psExecDir);

	free(psExecDir);

	/*fprintf(stderr, " WorkingDir = %s\n DataDir = %s\n UserHomeDir = %s\n ProgHomeDir = %s\n",
	        sWorkingDir, sDataDir, sUserHomeDir, sProgHomeDir);*/
}
