//
// HypnoticOS
// Copyright (C) 2020  jk30
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "common.h"
#include <hypnoticos/dispatcher.h>

int main(int argc, char *argv[])
{
  DispatcherProcess_t *p, *p2;

  /////////
  // DispatcherInit
  /////////
  if(!DispatcherInit())
    ERROR();

  /////////
  // DispatcherProcessNew
  /////////
  if((p = DispatcherProcessNew("test process")) == NULL) {
    ERROR();
    return 1;
  }

  if(p->pid != 1) {
    ERROR();
    return 1;
  }

  if((p2 = DispatcherProcessNew("test process 2")) == NULL) {
    ERROR();
    return 1;
  }

  if(p2->pid != 2) {
    ERROR();
    return 1;
  }

  /////////
  // DispatcherProcessRun
  /////////
  if(p->run != 0) {
    ERROR();
    return 1;
  }

  DispatcherProcessRun(p);

  if(p->run != 1) {
    ERROR();
    return 1;
  }

  /////////
  // DispatcherIndexLockAttempt
  // DispatcherIndexLockDone
  // DispatcherIndexLockRetrieve
  /////////
  DispatcherOpenIndex_t *index_lock[4];

  if((index_lock[0] = DispatcherIndexLockAttempt(p, "/bin/test")) == NULL) {
    ERROR();
    return 1;
  }
  if(index_lock[0]->open_index_id != 1 || strcmp(index_lock[0]->path, "/bin/test") != 0 || index_lock[0]->process != p) {
    ERROR();
    return 1;
  }

  if((index_lock[1] = DispatcherIndexLockAttempt(p, "/bin")) == NULL) {
    ERROR();
    return 1;
  }

  DispatcherIndexLockDone(index_lock[1]);

  if((index_lock[1] = DispatcherIndexLockAttempt(p, "/bin")) == NULL) {
    ERROR();
    return 1;
  }

  if((index_lock[2] = DispatcherIndexLockAttempt(p, "/bin")) != NULL) {
    ERROR();
    return 1;
  }

  DispatcherIndexLockDone(index_lock[0]);

  if((index_lock[2] = DispatcherIndexLockAttempt(p, "/bin/test")) == NULL) {
    ERROR();
    return 1;
  }

  if((index_lock[3] = DispatcherIndexLockAttempt(p, "/bin")) != NULL) {
    ERROR();
    return 1;
  }


  if((index_lock[0] = DispatcherIndexLockRetrieve(p, index_lock[2]->open_index_id)) == NULL) {
    ERROR();
    return 1;
  }
  if(index_lock[0] != index_lock[2]) {
    ERROR();
    return 1;
  }

  return 0;
}
