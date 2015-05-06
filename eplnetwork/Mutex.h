#include "pthread.h"

class  Mutex
{
public:
    Mutex();

    ~Mutex();

    inline void Acquire()
    {
        pthread_mutex_lock(&mutex);
    }

    inline void Release()
    {
        pthread_mutex_unlock(&mutex);
    }

    inline bool AttemptAcquire()
    {
        return (pthread_mutex_trylock(&mutex) == 0);
    }

protected:
    static bool attr_initalized;
    static pthread_mutexattr_t attr;

    pthread_mutex_t mutex;
};


