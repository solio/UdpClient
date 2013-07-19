#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include "Config.h"

#define ALLOCTABLEPATH "../Refference/alloctable"
#define ALLOCFORMAT "%d %d %d \n"
#define ALLOCATED 1
#define NONALLOCATED 0

#ifdef DEF_ALLOCATION
typedef struct __declspec(dllexport) Allocation
#else
typedef struct __declspec(dllimport) Allocation
#endif
{
	int nWorkID;
	int nUdpPort;
	int bIsAllocted;
}Allocation;


#ifdef DEF_ALLOCATOR
class __declspec(dllexport) Allocator
#else
class __declspec(dllimport) Allocator
#endif
{
public:
	Allocator(void);
	~Allocator(void);
	void Alloc(const SrvConfig_t* list, int nSize);
	void ReleaseAllocated(const Allocation& tAlloc);
	Allocation GetNonAllocated();
	Allocation GetAllocatedById(int nWorkId);
protected:

};

#endif