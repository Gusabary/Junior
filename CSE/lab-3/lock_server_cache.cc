// the caching lock server implementation

#include "lock_server_cache.h"
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "lang/verify.h"
#include "handle.h"
#include "tprintf.h"


lock_server_cache::lock_server_cache()
{
}

/* acquire doesn't contain any sleep
 */
int lock_server_cache::acquire(lock_protocol::lockid_t lid, std::string id, int &)
{  
  pthread_mutex_lock(&mtx); 
  // tprintf("acquire in server %s for %d\n", id.c_str(), lid);

  if (lock_keeper.find(lid) == lock_keeper.end()) {
    // this type of lock hasn't appeared
    std::queue<std::string> clist;
    lock_keeper.insert(std::pair<lock_protocol::lockid_t, std::queue<std::string> >(lid, clist));
  }

  if (lock_keeper[lid].empty()) {
    // lock is free
    lock_keeper[lid].push(id);
    pthread_mutex_unlock(&mtx); 
    return lock_protocol::OK;
  }

  // lock is occupied
  std::string cid_back = lock_keeper[lid].back();  // the last one in queue
  lock_keeper[lid].push(id);

  handle h(cid_back);
  rlock_protocol::status ret;
  if (h.safebind()) {
    int r;
    pthread_mutex_unlock(&mtx); 

    ret = h.safebind()->call(rlock_protocol::revoke, lid, r);

    pthread_mutex_lock(&mtx); 
  }

  pthread_mutex_unlock(&mtx); 
  return ret == rlock_protocol::RETRY ? lock_protocol::RETRY : lock_protocol::OK;
}

/* release doesn't contain any sleep
 */
int lock_server_cache::release(lock_protocol::lockid_t lid, std::string id, int &r)
{
  /* If this method is called, there must be a client waiting for lock, 
   * otherwise the client previously holded the lock won't release it.
   */
  pthread_mutex_lock(&mtx); 
  // tprintf("release in server %s for %d\n", id.c_str(), lid);

  // for (int i = 0; i < lock_keeper[lid].size(); i++) {
  //   std::string cid_front = lock_keeper[lid].front();
  //   std::cout << cid_front << ' ';
  //   lock_keeper[lid].pop();
  //   lock_keeper[lid].push(cid_front);
  // }
  // std::cout << std::endl;

  lock_keeper[lid].pop();
  std::string cid_front = lock_keeper[lid].front();

  handle h(cid_front);
  pthread_mutex_unlock(&mtx); 

  rlock_protocol::status ret = h.safebind()->call(rlock_protocol::retry, lid, r);
  return lock_protocol::OK;
}

lock_protocol::status
lock_server_cache::stat(lock_protocol::lockid_t lid, int &r)
{
  tprintf("stat request\n");
  r = nacquire;
  return lock_protocol::OK;
}

