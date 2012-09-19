#include "StdAfx.h"
#include "Common.h"
/* 
    RawSpeed - RAW file decoder.

    Copyright (C) 2009 Klaus Post

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

    http://www.klauspost.com
*/

#if defined(__unix__) || defined(__APPLE__) || defined(__MINGW32__) 

void* _aligned_malloc(size_t bytes, size_t alignment) {
  void* ret= NULL;
  if (0==posix_memalign(&ret, alignment, bytes))
    return ret;
  else
    return NULL;
}
#include <unistd.h>
int rawspeed_get_number_of_processor_cores()
{
  return sysconf( _SC_NPROCESSORS_ONLN );
}

#endif
