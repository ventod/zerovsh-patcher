/*
 * This file is part of ZeroVSH Patcher.

 * ZeroVSH Patcher is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * ZeroVSH Patcher is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ZeroVSH Patcher. If not, see <http://www.gnu.org/licenses/ .
 */

#include <pspintrman.h>
#include <pspsysmem.h>
#include <pspsdk.h>
#include <string.h>
#include <stdio.h>

#include "psploadcore.h"
#include "systemctrl.h"

#include "logger.h"
#include "blacklist.h"
#include "resolver.h"
#include "hook.h"

PSP_MODULE_INFO("ZeroVSH_Patcher_Kernel", 0x1007, 0, 2);
PSP_MAIN_THREAD_ATTR(0);

#define UNUSED __attribute__((unused))
#define MAKE_CALL(a, f) _sw(0x0C000000 | (((unsigned int)(f) >> 2) & 0x03FFFFFF), a);
#define REDIRECT_FUNCTION(a, f)                                 \
	_sw(0x08000000 | (((unsigned int)(f)&0x0FFFFFFC) >> 2), a); \
	_sw(0x00000000, a + 4);
#define MAKE_JUMP(a, f) _sw(0x08000000 | (((unsigned int)(f)&0x0FFFFFFC) >> 2), a);

typedef struct
{
	const char *modname;
	const char *modfile;
} modules;

const modules g_modules_mod[] = {
	{"vsh_module", "vshmain.prx"},
	{"scePaf_Module", "paf.prx"},
	{"sceVshCommonGui_Module", "common_gui.prx"},
};

const char *exts[] = {".rco", ".pmf", ".bmp", ".pgf", ".prx", ".dat"};

int k1;
SceUID path_id;

PspIoDrv *lflash;
PspIoDrv *fatms;
static PspIoDrvArg *ms_drv = NULL;

#define REDIR_PATH "/PSP/VSH"

#ifdef USE_EF0
#define DRV "fatef"
#define DSK "ef0:"
#define DUMMY "ef0:/_dummy.prx"
#else
#define DRV "fatms"
#define DSK "ms0:"
#define DUMMY "ms0:/_dummy.prx"
#endif

int (*msIoOpen)(PspIoDrvFileArg *arg, char *file, int flags, SceMode mode);
int (*msIoGetstat)(PspIoDrvFileArg *arg, const char *file, SceIoStat *stat);

int (*IoOpen)(PspIoDrvFileArg *arg, char *file, int flags, SceMode mode);
int (*IoGetstat)(PspIoDrvFileArg *arg, const char *file, SceIoStat *stat);

void *zeroCtrlAllocUserBuffer(SceUID uid, int size)
{
	void *addr;
	k1 = pspSdkSetK1(0);
	uid = sceKernelAllocPartitionMemory(PSP_MEMORY_PARTITION_USER, "pathBuf", PSP_SMEM_High, size, NULL);
	addr = (uid >= 0) ? sceKernelGetBlockHeadAddr(uid) : NULL;
	pspSdkSetK1(k1);
	return addr;
}

void zeroCtrlFreeUserBuffer(SceUID uid)
{
	if (uid >= 0)
	{
		k1 = pspSdkSetK1(0);
		sceKernelFreePartitionMemory(uid);
		pspSdkSetK1(k1);
	}
}

inline void zeroCtrlIcacheClearAll(void)
{
	__asm__ volatile("\
	 .word 0x40088000; .word 0x24091000; .word 0x7D081240;\
	 .word 0x01094804; .word 0x4080E000; .word 0x4080E800;\
	 .word 0x00004021; .word 0xBD010000; .word 0xBD030000;\
	 .word 0x25080040; .word 0x1509FFFC; .word 0x00000000;\
	 " ::);
}

inline void zeroCtrlDcacheWritebackAll(void)
{
	__asm__ volatile("\
	 .word 0x40088000; .word 0x24090800; .word 0x7D081180;\
	 .word 0x01094804; .word 0x00004021; .word 0xBD140000;\
	 .word 0xBD140000; .word 0x25080040; .word 0x1509FFFC;\
	 .word 0x00000000; .word 0x0000000F; .word 0x00000000;\
	 " ::);
}

void ClearCaches(void)
{
	zeroCtrlIcacheClearAll();
	zeroCtrlDcacheWritebackAll();
}

int zeroCtrlIsValidFileType(const char *file)
{
	int ret = 0;
	const char *ext;
	if (!file)
	{
		// zeroCtrlWriteDebug("--> Is NULL\n");
		return 0;
	}

	// zeroCtrlWriteDebug("file: %s\n", file);
	k1 = pspSdkSetK1(0);
	ext = strrchr(file, '.');
	if (!ext)
	{
		// zeroCtrlWriteDebug("--> No Extension\n");
	}
	else
	{
		for (int i = 0; i < ITEMSOF(exts); i++)
		{
			if (strcmp(ext, exts[i]) == 0)
			{
				// zeroCtrlWriteDebug("Success\n");
				ret = 1;
				break;
			}
		}
	}
	pspSdkSetK1(k1);
	return ret;
}

const char *zeroCtrlGetFileName(const char *file)
{
	char *ret = NULL;

	if (!file)
	{
		// zeroCtrlWriteDebug("--> Is NULL\n");
		return ret;
	}

	// zeroCtrlWriteDebug("file: %s\n", file);
	k1 = pspSdkSetK1(0);
	ret = strrchr(file, '/');
	pspSdkSetK1(k1);

	// blacklist this one
	if (strcmp(file, "/codepage/cptbl.dat") == 0)
	{
		ret = NULL;
	}

	if (!ret)
	{
		// zeroCtrlWriteDebug("--> No path\n");
	}
	else
	{
		// zeroCtrlWriteDebug("Success\n");
	}
	return ret;
}

char *zeroCtrlSwapFile(const char *file)
{
	const char *oldfile;
	char *newfile = zeroCtrlAllocUserBuffer(path_id, 256);

	if (!newfile)
	{
		// zeroCtrlWriteDebug("Cannot allocate 256 bytes of memory, abort\n");
		return NULL;
	}

	k1 = pspSdkSetK1(0);

	*newfile = '\0';
	oldfile = zeroCtrlGetFileName(file);
	if (!oldfile)
	{
		// zeroCtrlWriteDebug("-> File not found, abort\n\n");
		pspSdkSetK1(k1);
		return NULL;
	}

	if (zeroCtrlIsBlacklistedFound())
	{
		if (strcmp(oldfile, "/ltn0.pgf") == 0)
		{
			// zeroCtrlWriteDebug("-> File is blacklisted, abort\n\n");
			pspSdkSetK1(k1);
			return NULL;
		}
	}

	sprintf(newfile, REDIR_PATH "%s", oldfile);
	pspSdkSetK1(k1);

	// zeroCtrlWriteDebug("-> Redirected file: %s\n", newfile);
	return newfile;
}

int zeroCtrlIoGetstatEX(PspIoDrvFileArg *arg, const char *file, SceIoStat *stat)
{
	int ret;
	char *new_path;
	PspIoDrvArg *drv;

	new_path = zeroCtrlSwapFile(file);
	if (!new_path)
	{
		return IoGetstat(arg, file, stat);
	}

	drv = arg->drv;
	arg->drv = ms_drv;
	ret = msIoGetstat(arg, new_path, stat);

	if (ret >= 0)
	{
		// zeroCtrlWriteDebug("--> %s found, using custom file\n\n", new_path);
	}
	else
	{
		// zeroCtrlWriteDebug("--> %s not found, using default file\n\n", file);
		arg->drv = drv;
		ret = IoGetstat(arg, file, stat);
	}

	zeroCtrlFreeUserBuffer(path_id);
	return ret;
}

int zeroCtrlIoOpenEX(PspIoDrvFileArg *arg, char *file, int flags, SceMode mode)
{
	int ret;
	char *new_path;
	PspIoDrvArg *drv;

	if ((new_path = zeroCtrlSwapFile(file)) == NULL)
	{
		return IoOpen(arg, file, flags, mode);
	}

	drv = arg->drv;
	arg->drv = ms_drv;
	ret = msIoOpen(arg, new_path, flags, mode);

	if (ret >= 0)
	{
		// zeroCtrlWriteDebug("--> %s found, using custom file\n\n", new_path);
	}
	else
	{
		// zeroCtrlWriteDebug("--> %s not found, using default file\n\n", file);
		arg->drv = drv;
		ret = IoOpen(arg, file, flags, mode);
	}

	zeroCtrlFreeUserBuffer(path_id);
	return ret;
}

int zeroCtrlMsIoOpen(PspIoDrvFileArg *arg, char *file, int flags, SceMode mode)
{
	// do not add logging in this function to avoid infinite recursion
	ms_drv = arg->drv;
	return msIoOpen(arg, file, flags, mode);
}

int zeroCtrlMsIoGetstat(PspIoDrvFileArg *arg, const char *file, SceIoStat *stat)
{
	// do not use sceIoGetstat in this function to avoid infinite recursion
	return msIoGetstat(arg, file, stat);
}

int zeroCtrlIoOpen(PspIoDrvFileArg *arg, char *file, int flags, SceMode mode)
{
	if (ms_drv && zeroCtrlIsValidFileType(file))
	{
		return zeroCtrlIoOpenEX(arg, file, flags, mode);
	}
	else
	{
		// zeroCtrlWriteDebug("cannot redirect file: %s\n\n", file);
	}
	return IoOpen(arg, file, flags, mode);
}

int zeroCtrlIoGetstat(PspIoDrvFileArg *arg, const char *file, SceIoStat *stat)
{
	if (ms_drv && zeroCtrlIsValidFileType(file))
	{
		return zeroCtrlIoGetstatEX(arg, file, stat);
	}
	else
	{
		// zeroCtrlWriteDebug("cannot redirect file: %s\n\n", file);
	}
	return IoGetstat(arg, file, stat);
}

int zeroCtrlHookDriver(void)
{
	int intr;
	SceUID fd;

	fatms = sctrlHENFindDriver(DRV);
	lflash = sctrlHENFindDriver("flashfat");

	if (!lflash || !fatms)
	{
		// zeroCtrlWriteDebug("failed to hook drivers: lflash: %08X, fatms: %08X\n", (unsigned int)lflash, (unsigned int)fatms);
		return 0;
	}

	msIoOpen = fatms->funcs->IoOpen;
	msIoGetstat = fatms->funcs->IoGetstat;

	IoOpen = lflash->funcs->IoOpen;
	IoGetstat = lflash->funcs->IoGetstat;

	// zeroCtrlWriteDebug("suspending interrupts\n");
	intr = sceKernelCpuSuspendIntr();

	fatms->funcs->IoOpen = zeroCtrlMsIoOpen;
	fatms->funcs->IoGetstat = zeroCtrlMsIoGetstat;

	lflash->funcs->IoOpen = zeroCtrlIoOpen;
	lflash->funcs->IoGetstat = zeroCtrlIoGetstat;

	sceKernelCpuResumeIntr(intr);
	ClearCaches();
	// zeroCtrlWriteDebug("interrupts restored\n");

	fd = sceIoOpen(DUMMY, PSP_O_RDONLY, 0644);

	// just in case that someone has a file like this
	if (fd >= 0)
	{
		sceIoClose(fd);
	}

	// zeroCtrlWriteDebug("ms_drv addr: %08X\n", (unsigned int)ms_drv);

	return 1;
}
// The 2nd arg is a SceLoadCoreExecFileInfo *, but we don't need it for now
int zeroCtrlModuleProbe(void *data, void *exec_info)
{
	char filename[256];
	SceSize size;
	SceUID fd;

	char *modname = (char *)data + (((unsigned int *)data)[0x10] & 0x7FFFFFFF) + 4;

	zeroCtrlSetBlackListItems(modname);

	for (int i = 0; i < ITEMSOF(g_modules_mod); i++)
	{
		if (strcmp(modname, g_modules_mod[i].modname) == 0)
		{
			// zeroCtrlWriteDebug("probing: %s\n", g_modules_mod[i].modfile);
			sprintf(filename, DSK REDIR_PATH "/%s", g_modules_mod[i].modfile);
			fd = sceIoOpen(filename, PSP_O_RDONLY, 0644);
			if (fd >= 0)
			{
				// zeroCtrlWriteDebug("writting %s into buffer\n", filename);
				size = sceIoLseek(fd, 0, PSP_SEEK_END);
				sceIoLseek(fd, 0, PSP_SEEK_SET);
				sceIoRead(fd, data, size);
				sceIoClose(fd);
				ClearCaches();
			}
			else
			{
				// zeroCtrlWriteDebug("%s not found, leaving buffer untouched\n", filename);
			}
			break;
		}
	}
	return sceKernelProbeExecutableObject(data, exec_info);
}

void zeroCtrlHookModule(void)
{
	SceModule2 *module = (SceModule2 *)sceKernelFindModuleByName("sceModuleManager");

	if (!module || hook_import_bynid(module, "LoadCoreForKernel", moduleprobe_nid, zeroCtrlModuleProbe, 0) < 0)
	{
		// zeroCtrlWriteDebug("failed to hook ProbeExecutableObject, nid: %08X\n", moduleprobe_nid);
	}
	else
	{
		// zeroCtrlWriteDebug("ProbeExecutableObject nid: %08X, addr: %08X\n", moduleprobe_nid, (unsigned int)sceKernelProbeExecutableObject);
	}
}

int module_start(SceSize args UNUSED, void *argp UNUSED)
{
	zeroCtrlWriteDebug("ZeroVSH Patcher v0.4\n");
	zeroCtrlWriteDebug("Copyright 2011-2015 (C) NightStar3 and codestation\n");
	zeroCtrlWriteDebug("Lite Mod by Vento\n");
	zeroCtrlWriteDebug("[--- Lite 0.3 ---]\n\n");
	zeroCtrlResolveNids();
	zeroCtrlHookModule();
	zeroCtrlHookDriver();
	return 0;
}

int module_stop(SceSize args UNUSED, void *argp UNUSED)
{
	return 0;
}