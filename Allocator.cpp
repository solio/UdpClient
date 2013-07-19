#ifndef DEF_ALLOCATOR
#define DEF_ALLOCATOR
#define DEF_ALLOCATION
#endif

#include "DataModel.h"
#include "Allocator.h"

Allocator::Allocator(void)
{
}


Allocator::~Allocator(void)
{
}

/*
 * @program:
 *     write the alloc table file that specific the server's
 *     config.the alloc table file format is following:
 *     [WorkID][SPACE][UdpPort][SPACE][IsAllocted][ENTER]
 */ 
void Allocator::Alloc(const SrvConfig_t* list, int nSize)
{
	FILE *fp = NULL;

	fp = fopen(ALLOCTABLEPATH, "w");
	if(fp == NULL)
	{
		return;
	}

	for(int i = 0; i < nSize; i++)
	{
		fprintf(fp, ALLOCFORMAT, list[i].WorkID,
			list[i].UdpPort, NONALLOCATED);
	}
	fclose(fp);	
}

/*
 * @program:
 *     release the allocation for the specific server's config.
 *     set the [IsAllocated] = NONALLOCATED
 * @param:
 *     tAlloc: the allocation that has been allocated
 */
void Allocator::ReleaseAllocated(const Allocation& tAlloc)
{
	FILE *fp = NULL;
	Allocation tmpVal;

	fp = fopen(ALLOCTABLEPATH, "r+");
	if(fp == NULL)
	{
		return;
	}
		
	while(!feof(fp))
	{				
		fscanf(fp, ALLOCFORMAT, &tmpVal.nWorkID,
			&tmpVal.nUdpPort, &tmpVal.bIsAllocted);
		if(tmpVal.nWorkID == tAlloc.nWorkID ||
			tmpVal.nUdpPort == tAlloc.nUdpPort)
		{
			int nLen = 0;
			char szFormat[64] = "";

			sprintf(szFormat, ALLOCFORMAT, tmpVal.nWorkID,
				tmpVal.nUdpPort, NONALLOCATED);
			nLen = strlen(szFormat);
			nLen = fseek(fp, -1 * (nLen + 1), SEEK_CUR);
			nLen = fprintf(fp, ALLOCFORMAT, tmpVal.nWorkID,
				tmpVal.nUdpPort, NONALLOCATED);
			break;			
		}				
	}
	fclose(fp);
}

/*
 * @program:
 *     read the alloc table file and get an non-allocated config
 *     and give it to a new server for config.
 */
Allocation Allocator::GetNonAllocated()
{
	FILE *fp = NULL;
	Allocation tmpVal;

	tmpVal.bIsAllocted = ALLOCATED;
	fp = fopen(ALLOCTABLEPATH, "r+");
	if(fp == NULL)
	{
		return tmpVal;
	}

	while(!feof(fp))
	{		
		fscanf(fp, ALLOCFORMAT, &tmpVal.nWorkID,
			&tmpVal.nUdpPort, &tmpVal.bIsAllocted);
		if(tmpVal.bIsAllocted == NONALLOCATED)
		{
			int nLen = 0;
			char szFormat[64] = "";

			sprintf(szFormat, ALLOCFORMAT, tmpVal.nWorkID,
				tmpVal.nUdpPort, ALLOCATED);
			nLen = strlen(szFormat);
			nLen = fseek(fp, -1 * (nLen + 1), SEEK_CUR);
			nLen = fprintf(fp, szFormat);
			break;
		}		
	}
	fclose(fp);
	return tmpVal;
}

Allocation Allocator::GetAllocatedById(int nWorkId)
{
	FILE *fp = NULL;
	Allocation tmpVal;

	tmpVal.nWorkID = INVALID_ID;
	fp = fopen(ALLOCTABLEPATH, "r+");
	if(fp == NULL)
	{
		return tmpVal;
	}

	while(!feof(fp))
	{		
		fscanf(fp, ALLOCFORMAT, &tmpVal.nWorkID,
			&tmpVal.nUdpPort, &tmpVal.bIsAllocted);
		if(tmpVal.nWorkID == nWorkId)
		{
			break;
		}		
	}
	fclose(fp);
	return tmpVal;
}