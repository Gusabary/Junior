// RPC stubs for clients to talk to lock_server, and cache the locks
// see lock_client.cache.h for protocol details.

#include "lock_client_cache.h"
#include "rpc.h"
#include <sstream>
#include <iostream>
#include <stdio.h>
#include "tprintf.h"


int lock_client_cache::last_port = 0;

lock_client_cache::lock_client_cache(std::string xdst, 
				     class lock_release_user *_lu)
  : lock_client(xdst), lu(_lu)
{
  srand(time(NULL)^last_port);
  rlock_port = ((rand()%32000) | (0x1 << 10));
  const char *hname;
  // VERIFY(gethostname(hname, 100) == 0);
  hname = "127.0.0.1";
  std::ostringstream host;
  host << hname << ":" << rlock_port;
  id = host.str();
  last_port = rlock_port;
  rpcs *rlsrpc = new rpcs(rlock_port);
  rlsrpc->reg(rlock_protocol::revoke, this, &lock_client_cache::revoke_handler);
  rlsrpc->reg(rlock_protocol::retry, this, &lock_client_cache::retry_handler);
}

/* acquire contains sleep
 */
lock_protocol::status
lock_client_cache::acquire(lock_protocol::lockid_t lid)
{
  pthread_mutex_lock(&mtx);
  if (lock_keeper.find(lid) == lock_keeper.end()) {
    // this type of lock hasn't appeared
    lock_manager.insert(std::pair<lock_protocol::lockid_t, pthread_cond_t>(lid, PTHREAD_COND_INITIALIZER));
    lock_keeper.insert(std::pair<lock_protocol::lockid_t, client_status>(lid, NONE));
    revoke_has_arrived.insert(std::pair<lock_protocol::lockid_t, bool>(lid, false));
  }
  // tprintf("acquire in client %s for %d status: %d\n", id.c_str(), lid, lock_keeper[lid]);

  if (lock_keeper[lid] == NONE) {
    // the client has no idea about the lock, need to acquire from server
    REACQUIRE:
    lock_keeper[lid] = ACQUIRING;
    int r;
    pthread_mutex_unlock(&mtx);  

    lock_protocol::status ret = cl->call(lock_protocol::acquire, lid, id, r);

    pthread_mutex_lock(&mtx);  
    // ret may be OK or RETRY
    if (ret == lock_protocol::RETRY) {
      // tprintf("acquire in client %s for %d NONE -> ACQUIRING\n", id.c_str(), lid);
      while (lock_keeper[lid] != FREE && lock_keeper[lid] != LOCKED) {
        if (lock_keeper[lid] == NONE)
          goto REACQUIRE;
        pthread_cond_wait(&(lock_manager.find(lid)->second), &mtx);
      }
      // tprintf("acquire in client %s for %d FREE -> LOCKED\n", id.c_str(), lid);
      lock_keeper[lid] = LOCKED;
      
      pthread_mutex_unlock(&mtx);
      return lock_protocol::OK;
    }
    // tprintf("acquire in client %s for %d NONE -> LOCKED\n", id.c_str(), lid);
    lock_keeper[lid] = LOCKED;
    
    pthread_mutex_unlock(&mtx);  
    return lock_protocol::OK;
  }
  
  // the client is holding the lock
  while (lock_keeper[lid] != FREE) {
    if (lock_keeper[lid] == NONE)
      goto REACQUIRE;
    pthread_cond_wait(&(lock_manager.find(lid)->second), &mtx);
  }
  // tprintf("acquire in client %s for %d -> LOCKED\n", id.c_str(), lid);
  lock_keeper[lid] = LOCKED;

  pthread_mutex_unlock(&mtx);  
  return lock_protocol::OK;
}

/* release doesn't contain any sleep
 */
lock_protocol::status
lock_client_cache::release(lock_protocol::lockid_t lid)
{
  pthread_mutex_lock(&mtx);
  // tprintf("release in client %s for %d status: %d\n", id.c_str(), lid, lock_keeper[lid]);
  if (lock_keeper[lid] != LOCKED) {
    pthread_mutex_unlock(&mtx);
    // tprintf("STATUSERR!!\n");
    return lock_protocol::STATUSERR;
  }

  if (revoke_has_arrived[lid]) {
    // need to grant lock back to server right now
    // tprintf("release in client %s for %d LOCKED -> RELEASING\n", id.c_str(), lid);
    lock_keeper[lid] = RELEASING;
    int r;
    pthread_mutex_unlock(&mtx);

    int ret = cl->call(lock_protocol::release, lid, id, r);
    
    pthread_mutex_lock(&mtx);
    // ret should be OK
    // tprintf("release in client %s for %d RELEASING -> NONE\n", id.c_str(), lid);
    lock_keeper[lid] = NONE;
    revoke_has_arrived[lid] = false;
    pthread_cond_signal(&(lock_manager.find(lid)->second));

    pthread_mutex_unlock(&mtx);
    return lock_protocol::OK;
  }

  // tprintf("release in client %s for %d LOCKED -> FREE\n", id.c_str(), lid);
  lock_keeper[lid] = FREE;
  pthread_cond_signal(&(lock_manager.find(lid)->second));

  pthread_mutex_unlock(&mtx);
  return lock_protocol::OK;
}

/* revoke handler doesn't contain any sleep
 */
rlock_protocol::status
lock_client_cache::revoke_handler(lock_protocol::lockid_t lid, int &)
{
  pthread_mutex_lock(&mtx);
  // tprintf("revoke in client %s for %d status: %d\n", id.c_str(), lid, lock_keeper[lid]);
  if (lock_keeper[lid] == FREE) {
    // tprintf("revoke in client %s for %d FREE -> RELEASING\n", id.c_str(), lid);
    lock_keeper[lid] = RELEASING;
    int r;
    pthread_mutex_unlock(&mtx);

    int ret = cl->call(lock_protocol::release, lid, id, r);
    
    pthread_mutex_lock(&mtx);
    if (ret == lock_protocol::OK) {
      // tprintf("revoke in client %s for %d RELEASING -> NONE\n", id.c_str(), lid);
      lock_keeper[lid] = NONE;
    }
    pthread_cond_signal(&(lock_manager.find(lid)->second));

    pthread_mutex_unlock(&mtx);
    return rlock_protocol::OK;
  }

  revoke_has_arrived[lid] = true;

  pthread_mutex_unlock(&mtx);
  return rlock_protocol::RETRY;
}

/* retry handler doesn't contain any sleep
 */
rlock_protocol::status
lock_client_cache::retry_handler(lock_protocol::lockid_t lid, int &)
{
  pthread_mutex_lock(&mtx);
  // tprintf("retry in client %s for %d status: %d\n", id.c_str(), lid, lock_keeper[lid]);
  if (lock_keeper[lid] != NONE && lock_keeper[lid] != ACQUIRING) {
    pthread_mutex_unlock(&mtx);
    // tprintf("STATUSERR\n");
    return rlock_protocol::STATUSERR;
  }

  if (lock_keeper[lid] == NONE) {
    pthread_mutex_unlock(&mtx);
    return rlock_protocol::OK;
  }

  if (lock_keeper[lid] == ACQUIRING) {
    // tprintf("retry in client %s for %d ACQUIRING -> FREE\n", id.c_str(), lid);
    lock_keeper[lid] = LOCKED;
    pthread_cond_signal(&(lock_manager.find(lid)->second));
    
    pthread_mutex_unlock(&mtx);
    return rlock_protocol::OK;
  }

  assert(0);
}



