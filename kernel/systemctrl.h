#ifndef __SCTRLLIBRARY_H__
#define __SCTRLLIBRARY_H__

#include <pspiofilemgr_kernel.h>

/**
 * Finds a driver
 *
 * @param drvname - The name of the driver (without ":" or numbers)
 *
 * @returns the driver if found, NULL otherwise
 *
 */
PspIoDrv *sctrlHENFindDriver(const char *drvname);

/**
 * Finds a function.
 *
 * @param szMod - The module where to search the function
 * @param szLib - The library name
 * @param nid - The nid of the function
 *
 * @returns - The function address or 0 if not found
 *
 */
unsigned int sctrlHENFindFunction(const char *szMod, const char *szLib, unsigned int nid);

#endif