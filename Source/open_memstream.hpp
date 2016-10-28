#ifdef ANDROID
/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef HAVE_OPEN_MEMSTREAM
/*
 * Implementation of the POSIX open_memstream() function, which Linux has
 * but BSD lacks.
 *
 * Summary:
 * - Works like a file-backed FILE* opened with fopen(name, "w"), but the
 *   backing is a chunk of memory rather than a file.
 * - The buffer expands as you write more data.  Seeking past the end
 *   of the file and then writing to it zero-fills the gap.
 * - The values at "*bufp" and "*sizep" should be considered read-only,
 *   and are only valid immediately after an fflush() or fclose().
 * - A '\0' is maintained just past the end of the file. This is not included
 *   in "*sizep".  (The behavior w.r.t. fseek() is not clearly defined.
 *   The spec says the null byte is written when a write() advances EOF,
 *   but it looks like glibc ensures the null byte is always found at EOF,
 *   even if you just seeked backwards.  The example on the opengroup.org
 *   page suggests that this is the expected behavior.  The null must be
 *   present after a no-op fflush(), which we can't see, so we have to save
 *   and restore it.  Annoying, but allows file truncation.)
 * - After fclose(), the caller must eventually free(*bufp).
 *
 * This is built out of funopen(), which BSD has but Linux lacks.  There is
 * no flush() operator, so we need to keep the user pointers up to date
 * after each operation.
 *
 * I don't think Windows has any of the above, but we don't need to use
 * them there, so we just supply a stub.
 */
//#include <cutils/open_memstream.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

namespace sisu {
//#ifdef HAVE_FUNOPEN
#if 1 //TODO: Actually check if we have funopen (For now assume we do)
/*
 * Definition of a seekable, write-only memory stream.
 */
typedef struct {
    char**      bufp;       /* pointer to buffer pointer */
    size_t*     sizep;      /* pointer to eof */
    size_t      allocSize;  /* size of buffer */
    size_t      eof;        /* furthest point we've written to */
    size_t      offset;     /* current write offset */
    char        saved;      /* required by NUL handling */
} MemStream;
#define kInitialSize    1024
/*
 * Ensure that we have enough storage to write "size" bytes at the
 * current offset.  We also have to take into account the extra '\0'
 * that we maintain just past EOF.
 *
 * Returns 0 on success.
 */
static inline int ensureCapacity(MemStream* stream, int writeSize)
{
    size_t neededSize = stream->offset + writeSize + 1;
    if (neededSize <= stream->allocSize)
        return 0;
    size_t newSize;
    if (stream->allocSize == 0) {
        newSize = kInitialSize;
    } else {
        newSize = stream->allocSize;
        newSize += newSize / 2;             /* expand by 3/2 */
    }
    if (newSize < neededSize)
        newSize = neededSize;
    char* newBuf = (char*) realloc(*stream->bufp, newSize);
    if (newBuf == NULL)
        return -1;
    *stream->bufp = newBuf;
    stream->allocSize = newSize;
    return 0;
}
/*
 * Write data to a memstream, expanding the buffer if necessary.
 *
 * If we previously seeked beyond EOF, zero-fill the gap.
 *
 * Returns the number of bytes written.
 */
static inline int write_memstream(void* cookie, const char* buf, int size)
{
    MemStream* stream = (MemStream*) cookie;
    if (ensureCapacity(stream, size) < 0)
        return -1;
    /* seeked past EOF earlier? */
    if (stream->eof < stream->offset) {
        memset(*stream->bufp + stream->eof, '\0',
            stream->offset - stream->eof);
    }
    /* copy data, advance write pointer */
    memcpy(*stream->bufp + stream->offset, buf, size);
    stream->offset += size;
    if (stream->offset > stream->eof) {
        /* EOF has advanced, update it and append null byte */
        assert(stream->offset < stream->allocSize);
        stream->eof = stream->offset;
    } else {
        /* within previously-written area; save char we're about to stomp */
        stream->saved = *(*stream->bufp + stream->offset);
    }
    *(*stream->bufp + stream->offset) = '\0';
    *stream->sizep = stream->offset;
    return size;
}
/*
 * Seek within a memstream.
 *
 * Returns the new offset, or -1 on failure.
 */
static inline fpos_t seek_memstream(void* cookie, fpos_t offset, int whence)
{
    MemStream* stream = (MemStream*) cookie;
    off_t newPosn = (off_t) offset;
    if (whence == SEEK_CUR) {
        newPosn += stream->offset;
    } else if (whence == SEEK_END) {
        newPosn += stream->eof;
    }
    if (newPosn < 0 || ((fpos_t)((size_t) newPosn)) != newPosn) {
        /* bad offset - negative or huge */
        errno = EINVAL;
        return (fpos_t) -1;
    }
    if (stream->offset < stream->eof) {
        /*
         * We were pointing to an area we'd already written to, which means
         * we stomped on a character and must now restore it.
         */
        *(*stream->bufp + stream->offset) = stream->saved;
    }
    stream->offset = (size_t) newPosn;
    if (stream->offset < stream->eof) {
        /*
         * We're seeked backward into the stream.  Preserve the character
         * at EOF and stomp it with a NUL.
         */
        stream->saved = *(*stream->bufp + stream->offset);
        *(*stream->bufp + stream->offset) = '\0';
        *stream->sizep = stream->offset;
    } else {
        /*
         * We're positioned at, or possibly beyond, the EOF.  We want to
         * publish the current EOF, not the current position.
         */
        *stream->sizep = stream->eof;
    }
    return newPosn;
}
/*
 * Close the memstream.  We free everything but the data buffer.
 */
static inline int close_memstream(void* cookie)
{
    free(cookie);
    return 0;
}
/*
 * Prepare a memstream.
 */
static inline FILE* open_memstream(char** bufp, size_t* sizep)
{
	FILE* fp;

	MemStream* stream;

	if (bufp == NULL || sizep == NULL)
	{
	        errno = EINVAL;
        	return NULL;
	}

	stream = (MemStream*) calloc(1, sizeof(MemStream));

    	if (stream == NULL)
	{
	        return NULL;
	}


    	fp = funopen( stream,  NULL, write_memstream, seek_memstream, close_memstream) ;

	if (fp == NULL)
	{
	        free(stream);
	        return NULL;
	}

	*sizep = 0;
	*bufp = NULL;
        stream->bufp = bufp;
        stream->sizep = sizep;

	return fp;
}
#else /*not HAVE_FUNOPEN*/
static inline FILE* open_memstream(char** bufp, size_t* sizep)
{
#error funopen not found!
    abort();
}
#endif /*HAVE_FUNOPEN*/
} // namespace sisu

#endif /*!HAVE_OPEN_MEMSTREAM*/
#endif // ANDROID
