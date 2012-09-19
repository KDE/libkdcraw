#ifndef DLLDEF_H
#define DLLDEF_H

#ifdef WIN32
#ifdef _MSC_VER
#pragma warning( disable: 4251 )
#endif
#ifdef RAWSPEED_NODLL
# define DllDef
#else
# ifdef RAWSPEED_BUILDLIB
#    define DllDef   __declspec( dllexport )
# else
#    define DllDef   __declspec( dllimport )
# endif
#endif
#else
#  define DllDef
#endif
#endif
