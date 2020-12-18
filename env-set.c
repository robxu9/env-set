// Clears the environment, then reads argv to set
// environment variables before passing exec()'ing
// onto the next command. Requires explicit '--'
// separator between environment variables, which
// are KEY=VALUE strings, and the command to run
// (and its further arguments.)
#define _GNU_SOURCE

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PARSE_ENV 1
#define PARSE_ARG 2

int main(int argc, char const *argv[])
{
  // set up variables we need
  // we allocate arrays that shouldn't ever have more than argc entries, I hope
  char **passed_argv = malloc(sizeof(char *) * argc);
  char **passed_envp = malloc(sizeof(char *) * argc);
  int passed_argv_idx = 0;
  int passed_envp_idx = 0;

  // process argv to see what we have
  int parseState = PARSE_ENV;
  int argIter = 1;
  while (argv[argIter])
  {
    const char *arg = argv[argIter++];

    size_t len = strlen(arg);
    char *dest = malloc(len + 1);
    if (dest == NULL)
    {
      fprintf(stderr, "failed to allocate %ld bytes\n", len);
      abort();
    }
    strcpy(dest, arg);

    switch (parseState)
    {
    case PARSE_ENV:
      if (!strcmp(arg, "--"))
      {
        parseState = PARSE_ARG;
        free(dest); // we don't actually need this anymore
        break;
      }

      passed_envp[passed_envp_idx++] = dest;

      {
        // attempt to figure out if this is PATH and so should be specially handled
        char *eqSep = strchr(dest, '=');
        if (eqSep == NULL)
        {
          fprintf(stderr, "found environment variable without =?: %s\n", dest);
          return 1;
        }
        size_t size = eqSep - dest; // no +1 because we don't want to take =

        char *envName = malloc(size + 1); // +1 to null terminate later
        strncpy(envName, dest, size);
        *(envName + size + 1) = '\0'; // null terminate

        // envValue is just eqSep + 1 to the end
        char *envValue = malloc(strlen(eqSep + 1) + 1);
        if (envValue == NULL)
        {
          fprintf(stderr, "failed to allocate %ld bytes\n", strlen(eqSep + 1) + 1);
          abort();
        }
        strcpy(envValue, eqSep + 1);

        if (!strcmp(envName, "PATH"))
        {
          // this is PATH, modify the PATH for this binary too
          // so that we can execvpe later and it will look at the current
          // PATH that it's running in
          if (setenv(envName, envValue, 1))
          {
            fprintf(stderr, "error setting PATH: errno %d: %s\n", errno, strerror(errno));
            abort();
          }
        }
        free(envName);
        free(envValue);
      }

      break;

    case PARSE_ARG:
      passed_argv[passed_argv_idx++] = dest;
      break;

    default:
      fprintf(stderr, "shouldn't reach here\n");
      abort();
    }
  }

  if (passed_argv_idx < 1)
  {
    fprintf(stderr, "no command specified for env-set to run\n");
    return 1;
  }

  // null terminate the arrays
  passed_argv[passed_argv_idx] = NULL;
  passed_envp[passed_envp_idx] = NULL;

  // execvpe the command
  execvpe(passed_argv[0], passed_argv, passed_envp);
  fprintf(stderr, "failed to execvpe %s: errno %d: %s\n", passed_argv[0], errno, strerror(errno));
  return 1;
}
