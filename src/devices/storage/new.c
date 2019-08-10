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
#include <hypnoticos/devices/storage.h>

void StorageNew(uint32_t device_type, void *device_type_data) {
  uint64_t i;
  StorageDevice_t *ptr;

  // Get number of entries
  for(i = 0, ptr = StorageDevices;
    ptr->device_type != DEVICE_TYPE_END;
    i++, ptr = (StorageDevice_t *) ((uint64_t) ptr + sizeof(StorageDevice_t)));

  // Allocate memory for a new entry
  StorageDevices = realloc(StorageDevices, sizeof(StorageDevice_t) * (i + 2));

  // Add this entry
  StorageDevices[i].device_type = device_type;
  StorageDevices[i].device_type_data = device_type_data;

  // Last entry
  StorageDevices[i + 1].device_type = DEVICE_TYPE_END;
}
