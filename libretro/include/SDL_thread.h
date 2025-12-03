//FIXME RETRO HACK REDONE ALL THIS CRAPPY
//SDLTHREAD
#ifndef SDL_THREAD_H
#define SDL_THREAD_H 1

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern long GetTicks(void);
#include <stdint.h>

#define Uint32 uint32_t
#define SDL_GetTicks  GetTicks

#define SDL_KillThread(X)

#define ERR_MAX_STRLEN	128
#define ERR_MAX_ARGS	5
#define SDLCALL
#define SDL_zero(x)	memset(&(x), 0, sizeof((x)))
#define SDL_malloc	malloc
#define SDL_MUTEX_TIMEDOUT	1
#define SDL_MUTEX_MAXWAIT	(~(Uint32)0)
#define SDL_SetError printf
#define SDL_free free

typedef struct SDL_error
{
    int error;
    char key[ERR_MAX_STRLEN];
    int argc;
    union
    {
        void *value_ptr;
        int value_i;
        double value_f;
        char buf[ERR_MAX_STRLEN];
    } args[ERR_MAX_ARGS];
} SDL_error;


#ifdef SF2000
/* SF2000 - stub all threading/semaphore */
#define SDL_Delay(a) /* no-op */

typedef int SDL_sem;
#define SDL_CreateSemaphore(x) ((SDL_sem*)0)
#define SDL_DestroySemaphore(x)
#define SDL_SemTryWait(x) 0
#define SDL_SemWait(x) 0
#define SDL_SemWaitTimeout(x,t) 0
#define SDL_SemPost(x) 0
#define SDL_SemValue(x) 0

typedef unsigned long SDL_threadID;
typedef int SYS_ThreadHandle;

typedef struct SDL_Thread {
    SDL_threadID threadid;
    SYS_ThreadHandle handle;
    int status;
    SDL_error errbuf;
    void *data;
} SDL_Thread;

typedef struct SDL_mutex {
    int recursive;
    SDL_threadID owner;
    SDL_sem *sem;
} SDL_mutex;

#define SDL_CreateMutex() ((SDL_mutex*)0)
#define SDL_DestroyMutex(m)
#define SDL_ThreadID() 0
#define SDL_mutexP(m) 0
#define SDL_mutexV(m) 0
#define SDL_MaskSignals(m)
#define SDL_UnmaskSignals(m)
#define SDL_SYS_SetupThread()
#define SDL_RunThread(d)
#define SDL_SYS_CreateThread(t,a) 0
#define SDL_SYS_WaitThread(t)
#define SDL_CreateThread(f,d) ((SDL_Thread*)0)
#define SDL_GetThreadID(t) 0
#define SDL_SetThreadPriority(p) 0
#define SDL_WaitThread(t,s)

typedef enum {
    SDL_THREAD_PRIORITY_LOW,
    SDL_THREAD_PRIORITY_NORMAL,
    SDL_THREAD_PRIORITY_HIGH
} SDL_ThreadPriority;

typedef int (SDLCALL * SDL_ThreadFunction) (void *data);

typedef struct {
    int (SDLCALL * func) (void *);
    void *data;
    SDL_Thread *info;
    SDL_sem *wait;
} thread_args;

#else /* NOT SF2000 */

#define SDL_Delay(a) usleep((a)*1000)

#include <sys/errno.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>

struct SDL_semaphore
{
    sem_t sem;
};
typedef struct SDL_semaphore SDL_sem;

/* Create a counting semaphore */
extern SDL_sem * SDL_CreateSemaphore(Uint32 initial_value);
extern void SDL_DestroySemaphore(SDL_sem *sem);
extern int SDL_SemTryWait(SDL_sem *sem);
#define SDL_SemWait(sem)
extern int SDL_SemWaitTimeout(SDL_sem *sem, Uint32 timeout);
extern Uint32 SDL_SemValue(SDL_sem *sem);
extern int SDL_SemPost(SDL_sem *sem);

typedef struct SDL_Thread SDL_Thread;
typedef unsigned long SDL_threadID;
typedef pthread_t SYS_ThreadHandle;

typedef enum {
    SDL_THREAD_PRIORITY_LOW,
    SDL_THREAD_PRIORITY_NORMAL,
    SDL_THREAD_PRIORITY_HIGH
} SDL_ThreadPriority;

typedef int (SDLCALL * SDL_ThreadFunction) (void *data);

/* This is the system-independent thread info structure */
struct SDL_Thread
{
    SDL_threadID threadid;
    SYS_ThreadHandle handle;
    int status;
    SDL_error errbuf;
    void *data;
};

struct SDL_mutex
{
    int recursive;
    SDL_threadID owner;
    SDL_sem *sem;
};

typedef struct SDL_mutex SDL_mutex;
extern SDL_mutex *SDL_CreateMutex(void);
extern void SDL_DestroyMutex(SDL_mutex * mutex);
extern Uint32 SDL_ThreadID(void);
extern int SDL_mutexP(SDL_mutex * mutex);
extern int SDL_mutexV(SDL_mutex * mutex);

extern void SDL_MaskSignals(sigset_t * omask);
extern void SDL_UnmaskSignals(sigset_t * omask);

typedef struct {
    int (SDLCALL * func) (void *);
    void *data;
    SDL_Thread *info;
    SDL_sem *wait;
} thread_args;

extern void SDL_SYS_SetupThread(void);
extern void SDL_RunThread(void *data);
extern int SDL_SYS_CreateThread(SDL_Thread *thread, void *args);
extern void SDL_SYS_WaitThread(SDL_Thread *thread);

#define DECLSPEC

#ifdef SDL_PASSED_BEGINTHREAD_ENDTHREAD
#undef SDL_CreateThread
extern DECLSPEC SDL_Thread *SDLCALL
SDL_CreateThread(int (SDLCALL * fn) (void *), void *data,
                 pfnSDL_CurrentBeginThread pfnBeginThread,
                 pfnSDL_CurrentEndThread pfnEndThread);
#else
extern DECLSPEC SDL_Thread *SDLCALL
SDL_CreateThread(int (SDLCALL * fn) (void *), void *data);
#endif

extern SDL_threadID SDL_GetThreadID(SDL_Thread * thread);
extern int SDL_SetThreadPriority(SDL_ThreadPriority priority);
extern void SDL_WaitThread(SDL_Thread * thread, int *status);

#endif /* SF2000 */

#define ARRAY_CHUNKSIZE	32

#endif /* SDL_THREAD_H */
