

#include "Common.h"
#include "Network.h"
#include "ListenSocket.h"
#include "Singleton.h"
#include <iostream>


initialiseSingleton(SocketEngine);
initialiseSingleton(SocketDeleter);

#ifdef CONFIG_USE_KQUEUE


kqueueEngine::kqueueEngine()
{
    new SocketDeleter();
    kq = kqueue();
    assert(kq != -1);
    memset(this->fds, 0, sizeof(void*) * MAX_DESCRIPTORS);
    m_running = true;
}

kqueueEngine::~kqueueEngine()
{
    close(kq);
}

void kqueueEngine::AddSocket(BaseSocket * s)
{
    assert(fds[s->GetFd()] == 0);
    std::cout<<s->GetFd()<<"\n";
    fds[s->GetFd()] = s;

    struct kevent ev;
    EV_SET(&ev, s->GetFd(), s->Writable() ? EVFILT_WRITE : EVFILT_READ, EV_ADD, 0, 0, NULL);
    if(kevent(kq, &ev, 1, NULL, 0, NULL) < 0)
       {}//DBG("!! could not add kevent for fd %u\n", s->GetFd());
}

void kqueueEngine::RemoveSocket(BaseSocket * s)
{
    int a, b;
    std::cout<<"remove socke from socketEngine: "<<s->GetFd()<<"\n";
    assert(fds[s->GetFd()] == s);
    fds[s->GetFd()] = 0;

    struct kevent ke, k2;
    EV_SET(&ke, s->GetFd(), EVFILT_READ, EV_DELETE, 0, 0, NULL);
    EV_SET(&k2, s->GetFd(), EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
    if((kevent(kq, &ke, 1, NULL, 0, NULL) < 0) && (kevent(kq, &k2, 1, NULL, 0, NULL) < 0))
       {}//DBG("!! both kevent removals returned -1 for fd %u\n", s->GetFd());
}

void kqueueEngine::WantWrite(BaseSocket * s)
{
    struct kevent ev;
    EV_SET(&ev, s->GetFd(), EVFILT_WRITE, EV_ADD | EV_ONESHOT, 0, 0, NULL);
    if(kevent(kq, &ev, 1, NULL, 0, NULL) < 0)
       std::cout<<"!! could not modify kevent for fd  "<<s->GetFd()<<"\n";
}

void kqueueEngine::RunLoop()
{
    const static int maxevents = MAX_DESCRIPTORS;
    timespec timeout;
    timeout.tv_sec = 1;
    timeout.tv_nsec = 0;
    struct kevent events[MAX_DESCRIPTORS];
    struct kevent ev;
    int nfds, i;
    BaseSocket * s;
    while(m_running)
    {
        //usleep(10);
        nfds = kevent(kq, 0, 0, events, maxevents, &timeout);
        for(i = 0; i < nfds; ++i)
        {
            s = fds[events[i].ident];
            if(s == 0)
            {
               {}//DBG("kqueue returned invalid fd %u\n", events[i].ident);
                continue;
            }

            if(events[i].flags & EV_EOF || events[i].flags & EV_ERROR)
            {
                s->OnError(events[i].fflags);
                continue;
            }

            if(events[i].filter == EVFILT_READ)
            {
                s->OnRead(0);
                if(s->Writable() && !s->m_writeLock)
                {
                    ++s->m_writeLock;
                    WantWrite(s);
                }
            }
            else if(events[i].filter == EVFILT_WRITE)
            {
                s->OnWrite(0);
                if(!s->Writable())
                {
                    --s->m_writeLock;
                    EV_SET(&ev, s->GetFd(), EVFILT_READ, EV_ADD, 0, 0, NULL);
                    if(kevent(kq, &ev, 1, NULL, 0, NULL) < 0)
                       {}//DBG("!! could not modify kevent (to read) for fd %u\n", s->GetFd());
                }
                else
                {
                    EV_SET(&ev, s->GetFd(), EVFILT_WRITE, EV_ADD | EV_ONESHOT, 0, 0, NULL);
                    if(kevent(kq, &ev, 1, NULL, 0, NULL) < 0)
                       {}//DBG("!! could not modify kevent (to write) for fd %u\n", s->GetFd());
                }

            }
            else
            {
               {}//DBG("Unknwon filter: %u Fflags: %u, fd: %u, flags: %u\n", events[i].filter, events[i].fflags, events[i].ident, events[i].flags);
            }
        }

//        for(i = 0; i < MAX_DESCRIPTORS; ++i)
//        {
//         s = fds[i];
//         if(s != 0 &&  s->m_WillDisconnet)
//         {
//           s->Disconnect();
//          // break;
//         }
//        }
    }
}

void kqueueEngine::Shutdown()
{
    m_running = false;
    for(int i = 0; i < MAX_DESCRIPTORS; ++i)
    {
        if(fds[i] != 0)
        {
            fds[i]->Delete();
        }
    }

    sSocketDeleter.Kill();
    delete SocketDeleter::getSingletonPtr();
    delete this;
}

void kqueueEngine::SpawnThreads()
{
    uint32 count = 1;
    for(uint32 i = 0; i < count; ++i)
        ThreadPool.ExecuteTask(new SocketEngineThread(this));
}

#endif

#ifdef CONFIG_USE_EPOLL

epollEngine::epollEngine()
{
    /**
     *Constuctor: create new epoll_fd, and init a empty array of socket
     */
    new SocketDeleter();
    epoll_fd = epoll_create(MAX_DESCRIPTORS);
    assert(epoll_fd != -1);
    memset(this->fds, 0, sizeof(void*) * MAX_DESCRIPTORS);
    m_running = true;
}

epollEngine::~epollEngine()
{
    close(epoll_fd);
}

void epollEngine::AddSocket(BaseSocket * s)
{
    /**
     * add a socket fd to epoll_fd
     * create new epoll event struct, set an init event if this socket is writeable
     */
    assert(fds[s->GetFd()] == 0);
    fds[s->GetFd()] = s;

    struct epoll_event ev;
    memset(&ev, 0, sizeof(epoll_event));
    ev.data.fd = s->GetFd();
    ev.events = (s->Writable()) ? EPOLLOUT : EPOLLIN;
   // ev.events |= EPOLLET;

    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, s->GetFd(), &ev);
}

void epollEngine::RemoveSocket(BaseSocket * s)
{
    assert(fds[s->GetFd()] == s);
    fds[s->GetFd()] = 0;

    struct epoll_event ev;
    memset(&ev, 0, sizeof(epoll_event));
    ev.data.fd = s->GetFd();
    ev.events = (s->Writable()) ? EPOLLOUT : EPOLLIN;
    //ev.events |= EPOLLET;  //if we not use no-blocking socket. generally, we don't need EPOLLET mod

    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, s->GetFd(), &ev);
}

void epollEngine::WantWrite(BaseSocket * s)
{
    /**
     * tell EPL you want to write your data
     * EPL will set a OUT event
     * next Loop will catch this event and it will call your socket.Write method
     */
    struct epoll_event ev;
    memset(&ev, 0, sizeof(epoll_event));
    ev.data.fd = s->GetFd();
    ev.events = EPOLLOUT;// | EPOLLET;

    epoll_ctl(epoll_fd, EPOLL_CTL_MOD, s->GetFd(), &ev);
}

void epollEngine::RunLoop()
{
    /**
     * this loop will called by ThreadPool's threading.
     * use SocketEngineThread class pack this, and SocketEngineThread support method: run() for Threading call it
     */
    const static int maxevents = MAX_DESCRIPTORS;
    struct epoll_event events[MAX_DESCRIPTORS];
    int nfds, i;
    BaseSocket * s;
    while(m_running)
    {
       // usleep(10);
        /**
         * use blocking mod for epoll_wait and set timeout 1s
         */
        nfds = epoll_wait(epoll_fd, events, maxevents, 1000);
        for(i = 0; i < nfds; ++i)
        {
            s = fds[events[i].data.fd];
            if(s == 0)
            {
               {}//DBG("epoll returned invalid fd %u\n", events[i].data.fd);
                continue;
            }

            if(events[i].events & EPOLLHUP || events[i].events & EPOLLERR)
            {
                s->OnError(errno);
            }
            else if(events[i].events & EPOLLIN)
            {
                //get a IN event, notify socket to read data
                s->OnRead(0);
                if(s->Writable() && !s->m_writeLock)
                {
                    ++s->m_writeLock;
                    WantWrite(s);
                }
            }
            else if(events[i].events & EPOLLOUT)
            {
                //get a IN event, notify socket to write data
                //this event triggered by WantWrite method
                s->OnWrite(0);
                if(!s->Writable())
                {
                    /* change back to read state */
                    struct epoll_event ev;
                    memset(&ev, 0, sizeof(epoll_event));
                    ev.data.fd = s->GetFd();
                    ev.events = EPOLLIN ;//| EPOLLET;

                    epoll_ctl(epoll_fd, EPOLL_CTL_MOD, s->GetFd(), &ev);
                    /* unlock the lock in order to let next socket.Write to continue */
                    --s->m_writeLock;
                }
            }
        }
/**
        // if necessary, close socket when MAX DESCRIPTORS cout
        for(i = 0; i < MAX_DESCRIPTORS; ++i)
        {
         s = fds[i];
         if(s != 0 &&  s->m_WillDisconnet)
         {
           s->Disconnect();
           break;
         }
        }
*/

    }
}

void epollEngine::Shutdown()
{
    m_running = false;
    for(int i = 0; i < MAX_DESCRIPTORS; ++i)
    {
        if(fds[i] != 0)
        {
            fds[i]->Delete();
        }
    }

    sSocketDeleter.Kill();
    delete SocketDeleter::getSingletonPtr();
    delete this;
}

void epollEngine::SpawnThreads()
{
    uint32 count = 1;
    for(uint32 i = 0; i < count; ++i)
        ThreadPool.ExecuteTask(new SocketEngineThread(this));
}



#endif

