#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <execinfo.h>

#define BACKTRACE_DEPTH 20

#ifdef DEBUG
  #define debug(fmt, ...) \
    fprintf(stdout, "[sigtrace] DEBUG: " fmt, ##__VA_ARGS__)
#else
  #define debug(fmt, ...)
#endif

#define info(fmt, ...) \
  fprintf(stdout, "[sigtrace] " fmt, ##__VA_ARGS__)

#define error(fmt, ...) \
  fprintf(stdout, "[sigtrace] ERROR: " fmt, ##__VA_ARGS__)


#define N_ELEMS(x)  (sizeof(x) / sizeof(x[0]))

/* list the signals for which a sigtrace should be generated if
 * received by the application. */
#define OVERRIDE_SIGNALS \
  X(SIGABRT) \
  X(SIGUSR1) \
  X(SIGINT) \
  X(SIGQUIT) \
  X(SIGILL) \
  X(SIGFPE) \
  X(SIGSEGV) \
  X(SIGBUS)


/* Just returning from a signal handler might cause an infinit loop 
 * for some signals. If such signals are overriden (i.e. listed above)
 * list them below. After handling such a signal exit(EXIT_FAILURE)
 * will be called to avoid this undesired behaviour. */
#define ABORT_SIGNALS \
  X(SIGINT) \
  X(SIGQUIT) \
  X(SIGILL) \
  X(SIGFPE) \
  X(SIGSEGV) \
  X(SIGBUS)


#define X(sig) sig,
int override_signals[] = { OVERRIDE_SIGNALS };
int abort_signals[] = { ABORT_SIGNALS };
#undef X

#define X(sig) #sig,
char * override_signals_str[] = { OVERRIDE_SIGNALS };
char * abort_signals_str[] = { ABORT_SIGNALS };
#undef X

/*****************************************************************************
 * declaration of functions
 *****************************************************************************/
void init();
void handler(int, siginfo_t *, void *);

/*****************************************************************************
 * functions called on library load and unload
 *****************************************************************************/
static void _sigtrace_initialize() __attribute__((constructor));
static void _sigtrace_initialize()
{
  debug("initializing\n");

#ifdef DEBUG
  unsigned int i;

  fprintf(stderr, "[sigtrace] DEBUG: overriding: ");
  for (i = 0; i < N_ELEMS(override_signals); ++i) {
    fprintf(stderr, "%s (%d)  ", override_signals_str[i], override_signals[i]);
  }
  fprintf(stderr, "\n");

  fprintf(stderr, "[sigtrace] DEBUG: aborting:   ");
  for (i = 0; i < N_ELEMS(abort_signals); ++i) {
    fprintf(stderr, "%s (%d)  ", abort_signals_str[i], abort_signals[i]);
  }
  fprintf(stderr, "\n");
#endif 

  init();

  debug("initializing done\n");
}

static void _sigtrace_finalize() __attribute__((destructor));
static void _sigtrace_finalize()
{
  debug("finalizing\n");
  debug("finalizing done\n");
}


/*****************************************************************************
 * definition of functions
 *****************************************************************************/

void init()
{
  unsigned int i;
  struct sigaction action;

  memset(&action, 0, sizeof(action));
  action.sa_sigaction = handler;
  sigfillset(&action.sa_mask);
  action.sa_flags = SA_SIGINFO | SA_NODEFER;

  int error;

  for (i = 0; i < N_ELEMS(override_signals); ++i) {
    error = sigaction(override_signals[i], &action, NULL);

    if (error == -1) {
      error("Installing signal handler for signal %s (%d) failed: error %d.\n",
          override_signals_str[i], override_signals[i], error);
      exit(EXIT_FAILURE);
    }
  }

  return;
}

void handler(int signal, siginfo_t * info, void * context)
{
  unsigned int i = 0;
  char * name = "unknown";

  for (i = 0; i < N_ELEMS(override_signals); ++i) {
    if (override_signals[i] == signal) {
      name = override_signals_str[i];
      break;
    }
  }

  info("Received signal %s (%d).\n", name, signal);

  int num;
  void * addresses[BACKTRACE_DEPTH];
  char ** symbols = NULL;

  num = backtrace(addresses, BACKTRACE_DEPTH);
  symbols = backtrace_symbols(addresses, num);

  if (symbols == NULL) {
    error("Retrieving symbols failed.\n");
    exit(EXIT_FAILURE);
  }

  for (i = 0; i < num; ++i) {
    info("%d: %s\n", i, symbols[i]);
  }

  for (i = 0; i < N_ELEMS(abort_signals); ++i) {
    if (signal == abort_signals[i]) {
      debug("Calling exit() after handled signal %s (%d).\n",
          abort_signals_str[i], signal);
      exit(EXIT_FAILURE);
    }
  }

  return;
}
