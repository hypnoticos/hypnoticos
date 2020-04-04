//
// HypnoticOS
// Copyright (C) 2019  jk30
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

#include <stdlib.h>
#include <string.h>
#include <hypnoticos/dispatcher.h>

DispatcherCpu_t **DispatcherCpus;

uint8_t DispatcherInit() {
  DispatcherProcesses = malloc(sizeof(DispatcherProcess_t *));
  DispatcherProcesses[0] = NULL;

  DispatcherCpus = malloc(sizeof(DispatcherCpu_t *));
  DispatcherCpus[0] = NULL;

  DispatcherOpenIndices = malloc(sizeof(DispatcherOpenIndex_t **));
  DispatcherOpenIndices[0] = NULL;

  return 1;
}

uint8_t DispatcherInitAddCpu(uint8_t apic_id) {
  uint64_t i;
  DispatcherCpu_t *entry;

  for(i = 0; DispatcherCpus[i] != NULL; i++);

  entry = malloc(sizeof(DispatcherCpu_t));
  memset(entry, 0, sizeof(DispatcherCpu_t));
  entry->apic_id = apic_id;

  DispatcherCpus = realloc(DispatcherCpus, sizeof(DispatcherCpu_t *) * (i + 2));
  DispatcherCpus[i] = entry;
  DispatcherCpus[i + 1] = NULL;

  return 1;
}
