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

#include <stdint.h>
#include <string.h>
#include <hypnoticos/video-memory.h>

uint8_t VideoMemoryX = 0, VideoMemoryY = 0;
char * const VideoMemoryAddr = (char *) 0xb8000;

/*!
   \brief Initialise video memory functions and clear the screen.
*/
inline void VideoMemoryInit() {
  VideoMemoryClear();
}

/*!
   \brief Clear the screen
*/
void VideoMemoryClear() {
  uint32_t i;

  for(i = 0; i < 80 * 25 * 2; i += 2) {
    *(VideoMemoryAddr + i) = ' ';
    *(VideoMemoryAddr + i + 1) = 0x07;
  }

  VideoMemoryX = 0;
  VideoMemoryY = 0;
}

/*!
   \brief Place a character on the screen at the cursor position
   \param chr The character to be printed
*/
void VideoMemoryPutc(char chr) {
  uint16_t offset;

  if(chr != '\n') {
    offset = (VideoMemoryX + (VideoMemoryY * 80)) * 2;
    VideoMemoryAddr[offset] = chr;
    VideoMemoryAddr[offset + 1] = 0x07;
    VideoMemoryX++;
  } else {
    VideoMemoryX = 0;
    VideoMemoryY++;
  }

  if(VideoMemoryX >= 80) {
    VideoMemoryX = 0;
    VideoMemoryY++;
  }
  if(VideoMemoryY >= 25) {
    VideoMemoryY--;
    memcpy(VideoMemoryAddr, VideoMemoryAddr + (80 * 2), 80 * 24 * 2);
    memset(VideoMemoryAddr + (80 * 24 * 2), 0, 80 * 2);
  }
}
