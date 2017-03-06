/*****************************************************************************
 * cpu.h: cpu detection
 *****************************************************************************
 * Copyright (C) 2004-2012 x264 project
 *
 * Authors: Loren Merritt <lorenm@u.washington.edu>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111, USA.
 *
 * This program is also available under a commercial proprietary license.
 * For more information, contact us at licensing@x264.com.
 *****************************************************************************/

#ifndef X264_CPU_H
#define X264_CPU_H

uint32_t x264_cpu_detect( void );
int      x264_cpu_num_processors( void );
void     x264_cpu_emms( void );
void     x264_cpu_sfence( void );
#if HAVE_MMX
/* There is no way to forbid the compiler from using float instructions
 * before the emms so miscompilation could theoretically occur in the
 * unlikely event that the compiler reorders emms and float instructions. */
#ifdef _MSC_VER
#define x264_emms _m_empty
#define x264_sfence _mm_sfence
void _m_empty(void);
void _mm_sfence(void);
#elif HAVE_X86_INLINE_ASM
/* Clobbering memory makes the compiler less likely to reorder code. */
#define x264_emms() asm volatile( "emms":::"memory","st","st(1)","st(2)", \
                                  "st(3)","st(4)","st(5)","st(6)","st(7)" )
#define x264_sfence() asm volatile("sfence\n\t")
#else
#define x264_emms() x264_cpu_emms()
#define x264_sfence() x264_cpu_sfence()
#endif // HAVE_X86_INLINE_ASM
#else
#define x264_emms()
#define x264_sfence()
#endif // HAVE_MMX
void     x264_cpu_mask_misalign_sse( void );

/* kluge:
 * gcc can't give variables any greater alignment than the stack frame has.
 * We need 16 byte alignment for SSE2, so here we make sure that the stack is
 * aligned to 16 bytes.
 * gcc 4.2 introduced __attribute__((force_align_arg_pointer)) to fix this
 * problem, but I don't want to require such a new version.
 * This applies only to x86_32, since other architectures that need alignment
 * either have ABIs that ensure aligned stack, or don't support it at all. */
#if ARCH_X86 && HAVE_MMX && !defined(_MSC_VER)
int x264_stack_align( void (*func)(), ... );
#define x264_stack_align(func,...) x264_stack_align((void (*)())func, __VA_ARGS__)
#else
#define x264_stack_align(func,...) func(__VA_ARGS__)
#endif

typedef struct
{
    const char name[16];
    uint32_t flags;
} x264_cpu_name_t;
extern const x264_cpu_name_t x264_cpu_names[];

#endif
