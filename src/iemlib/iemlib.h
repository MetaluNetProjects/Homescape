/* For information on usage and redistribution, and for a DISCLAIMER OF ALL
* WARRANTIES, see the file, "LICENSE.txt," in this distribution.

iemlib written by Thomas Musil, Copyright (c) IEM KUG Graz Austria 2000 - 2018 */

#ifndef __IEMLIB_H__
#define __IEMLIB_H__


#define IS_A_NULL(atom,index) ((atom+index)->a_type == A_NULL)
#define IS_A_POINTER(atom,index) ((atom+index)->a_type == A_POINTER)
#define IS_A_FLOAT(atom,index) ((atom+index)->a_type == A_FLOAT)
#define IS_A_SYMBOL(atom,index) ((atom+index)->a_type == A_SYMBOL)
#define IS_A_DOLLAR(atom,index) ((atom+index)->a_type == A_DOLLAR)
#define IS_A_DOLLSYM(atom,index) ((atom+index)->a_type == A_DOLLSYM)
#define IS_A_SEMI(atom,index) ((atom+index)->a_type == A_SEMI)
#define IS_A_COMMA(atom,index) ((atom+index)->a_type == A_COMMA)

#define SETNULL(atom) ((atom)->a_type = A_NULL)

#ifdef MSW
int sys_noloadbang;
//t_symbol *iemgui_key_sym=0;
#include <io.h>
#else
extern int sys_noloadbang;
//extern t_symbol *iemgui_key_sym;
#include <unistd.h>
#endif


//millers m_pd.h beg
/* Microsoft Visual Studio is not C99, it does not provide stdint.h */
#ifdef _MSC_VER
typedef signed __int8     int8_t;
typedef signed __int16    int16_t;
typedef signed __int32    int32_t;
typedef signed __int64    int64_t;
typedef unsigned __int8   uint8_t;
typedef unsigned __int16  uint16_t;
typedef unsigned __int32  uint32_t;
typedef unsigned __int64  uint64_t;
#else
# include <stdint.h>
#endif

#if !defined(PD_FLOATSIZE) /* if compiled pd version < 0.47*/
# define PD_FLOATSIZE 32
#endif

typedef  union
{
    float f;
    unsigned int ui;
}t_iemdenormal_f;

typedef  union
{
    double f;
    unsigned int ui[2];
}t_iemdenormal_d;


#ifndef _MSC_VER /* Micrsoft compiler can't handle "inline" function/macros */
#if defined(__i386__) || defined(__x86_64__) || defined(__arm__) || defined(__aarch64__)
/* a test for NANs and denormals.  Should only be necessary on x86 variants. */
/* on arm, the impact is less noticeable, but...why not? */

static inline int IEM_DENORMAL_F(float f)  /* malformed float 32 */
{
    t_iemdenormal_f pun;
    pun.f = f;
    pun.ui &= 0x7f800000;
    return((pun.ui == 0) | (pun.ui == 0x7f800000));
}

static inline int IEM_DENORMAL_D(double f)  /* malformed double 64 */
{
    t_iemdenormal_d pun;
    pun.f = f;
    pun.ui[1] &= 0x7ff00000;
    return((pun.ui[1] == 0) | (pun.ui[1] == 0x7ff00000));
}

#if PD_FLOATSIZE == 32

typedef  union
{
    t_float f;
    unsigned int ui;
}t_iemdenormal_t32;

static inline int IEM_DENORMAL_T(t_float f)  /* malformed t_float 32 */
{
    t_iemdenormal_t32 pun;
    pun.f = f;
    pun.ui &= 0x7f800000;
    return((pun.ui == 0) | (pun.ui == 0x7f800000));
}

#elif PD_FLOATSIZE == 64

typedef  union
{
    t_float f;
    unsigned int ui[2];
}t_iemdenormal_t64;

static inline int IEM_DENORMAL_T(t_float f)  /* malformed t_float 64 */
{
    t_iemdenormal_t64 pun;
    pun.f = f;
    pun.ui[1] &= 0x7ff00000;
    return((pun.ui[1] == 0) | (pun.ui[1] == 0x7ff00000));
}

#endif /* PD_FLOATSIZE */
#else /* not INTEL or ARM */
#define IEM_DENORMAL_T(f) 0
#define IEM_DENORMAL_D(f) 0
#endif

#else   /* _MSC_VER */

#define IEM_DENORMAL_F(f) ((((*(unsigned int*)&(f))&0x7f800000)==0) || \
    (((*(unsigned int*)&(f))&0x7f800000)==0x7f800000))

static int IEM_DENORMAL_D(double f)  /* malformed t_float 64 */
{
    t_iemdenormal_d pun;
    pun.f = f;
    pun.ui[1] &= 0x7ff00000;
    return((pun.ui[1] == 0) | (pun.ui[1] == 0x7ff00000));
}

#if PD_FLOATSIZE == 32
#define IEM_DENORMAL_T(f) ((((*(unsigned int*)&(f))&0x7f800000)==0) || \
    (((*(unsigned int*)&(f))&0x7f800000)==0x7f800000))
#else   /* 64 bits... don't know what to do here */
//#define IEM_DENORMAL_T(f) (!(((f) >= 0) || ((f) <= 0)))
int IEM_DENORMAL_T(t_float f)  /* malformed t_float 64 */
{
    t_iemdenormal_t64 pun;
    pun.f = f;
    pun.ui[1] &= 0x7ff00000;
    return((pun.ui[1] == 0) | (pun.ui[1] == 0x7ff00000));
}
#endif
#endif /* _MSC_VER */


//millers m_pd.h end


/* on 64bit systems we cannot use garray_getfloatarray... */
#if ((defined PD_MAJOR_VERSION && defined PD_MINOR_VERSION) && (PD_MAJOR_VERSION > 0 || PD_MINOR_VERSION > 40))
# define iemarray_t t_word
# define iemarray_getarray garray_getfloatwords
# define iemarray_getfloat(pointer, index) (pointer[index].w_float)
# define iemarray_setfloat(pointer, index, fvalue) (pointer[index].w_float = fvalue)
#else
# define iemarray_t t_float
# define iemarray_getarray garray_getfloatarray
# define iemarray_getfloat(pointer, index) (pointer[index])
# define iemarray_setfloat(pointer, index, fvalue) (pointer[index] = fvalue)
#endif

#ifndef BUILD_DATE
# define BUILD_DATE "" __DATE__ " : " __TIME__
#endif

#if (defined PD_MAJOR_VERSION && defined PD_MINOR_VERSION) && (PD_MAJOR_VERSION > 0 || PD_MINOR_VERSION > 43)
# define iem_open sys_open
# define iem_close sys_close
# define iem_fopen sys_fopen
# define iem_fclose sys_fclose
#else
# define iem_open open
# define iem_close close
# define iem_fopen fopen
# define iem_fclose fclose
#endif


// millers d_osc.c beg

#define UNITBIT32 1572864.  /* 3*2^19; bit 32 has place value 1 */


#if defined(__FreeBSD__) || defined(__APPLE__) || defined(__FreeBSD_kernel__) \
    || defined(__OpenBSD__)
#include <machine/endian.h>
#endif

#if defined(__linux__) || defined(__CYGWIN__) || defined(__GNU__) || \
    defined(ANDROID)
#include <endian.h>
#endif

#ifdef __MINGW32__
#include <sys/param.h>
#endif

#ifdef _MSC_VER
/* _MSVC lacks BYTE_ORDER and LITTLE_ENDIAN */
#define LITTLE_ENDIAN 0x0001
#define BYTE_ORDER LITTLE_ENDIAN
#endif

#if !defined(BYTE_ORDER) || !defined(LITTLE_ENDIAN)
#error No byte order defined
#endif

#if BYTE_ORDER == LITTLE_ENDIAN
# define HIOFFSET 1
# define LOWOFFSET 0
#else
# define HIOFFSET 0    /* word offset to find MSB */
# define LOWOFFSET 1    /* word offset to find LSB */
#endif

union tabfudge
{
    double tf_d;
    int32_t tf_i[2];
};

// millers d_osc.c end

// millers d_delay.c beg
#define IEMDEFDELVS 64
#define IEMXTRASAMPS 4
#define IEMSAMPBLK 4
// millers d_delay.c end

#endif
