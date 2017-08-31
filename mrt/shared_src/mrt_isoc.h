
#ifndef _MRT_ISOC_H
#define _MRT_ISOC_H

#ifdef WIN32

/* Header to basically remove Windows compiler warnings that state:
 *   ... see declaration of 'strupr'
 *   ... : warning C4996: 'strupr': The POSIX name for this item is
 *   deprecated. Instead, use the ISO C++ conformant name: _strupr. See
 *   online help for details.  Todo: investiagte a better cross-platform
 *   solution.
 */

#define strdup     _strdup
#define strupr     _strupr
#define strlwr     _strlwr
#define swab       _swab
#define strcasecmp _stricmp

#endif

#endif
