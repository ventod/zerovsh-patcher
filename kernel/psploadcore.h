/*
 * PSP Software Development Kit - http://www.pspdev.org
 * -----------------------------------------------------------------------
 * Licensed under the BSD license, see LICENSE in PSPSDK root for details.
 *
 * psploadcore.h - Interface to LoadCoreForKernel.
 *
 * Copyright (c) 2005 Marcus R. Brown <mrbrown@ocgnet.org>
 * Copyright (c) 2005 James Forshaw <tyranid@gmail.com>
 * Copyright (c) 2005 John Kelley <ps2dev@kelley.ca>
 *
 * $Id: psploadcore.h 1095 2005-09-27 21:02:16Z jim $
 */

#ifndef PSPLOADCORE_H
#define PSPLOADCORE_H

#include <pspkerneltypes.h>

/** @defgroup LoadCore Interface to the LoadCoreForKernel library.
 */

#ifdef __cplusplus
extern "C"
{
#endif

	/** @addtogroup LoadCore Interface to the LoadCoreForKernel library. */
	/*@{*/

	typedef struct SceModule2
	{								 // Offset:
		struct SceModule2 *next;	 // 0x00
		unsigned short attribute;	 // 0x04
		unsigned char version[2];	 // 0x06
		char modname[27];			 // 0x08
		char terminal;				 // 0x23
		char mod_state;				 // 0x24
		char unknown1;				 // 0x25
		char unknown2[2];			 // 0x26
		unsigned int unknown3;		 // 0x28
		SceUID modid;				 // 0x2C
		unsigned int unknown4;		 // 0x30
		SceUID mem_id;				 // 0x34
		unsigned int mpid_text;		 // 0x38
		unsigned int mpid_data;		 // 0x3C
		void *ent_top;				 // 0x40
		unsigned int ent_size;		 // 0x44
		void *stub_top;				 // 0x48
		unsigned int stub_size;		 // 0x4C
		unsigned int unknown5[5];	 // 0x50
		unsigned int entry_addr;	 // 0x64
		unsigned int gp_value;		 // 0x68
		unsigned int text_addr;		 // 0x6C
		unsigned int text_size;		 // 0x70
		unsigned int data_size;		 // 0x74
		unsigned int bss_size;		 // 0x78
		unsigned int nsegment;		 // 0x7C
		unsigned int segmentaddr[4]; // 0x80
		unsigned int segmentsize[4]; // 0x90
	} SceModule2;

	/**
	 * Find a module by it's name.
	 *
	 * @param modname - The name of the module.
	 *
	 * @returns Pointer to the ::SceModule structure if found, otherwise NULL.
	 */
	// SceModule * sceKernelFindModuleByName(const char *modname);
	SceModule2 *sceKernelFindModuleByName(const char *modname);

	int sceKernelProbeExecutableObject(void *data, void *exec_info);
	/*@}*/

#ifdef __cplusplus
}
#endif

#endif