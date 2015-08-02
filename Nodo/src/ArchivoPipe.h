#ifndef ARCHIVOPIPE_H_
#define ARCHIVOPIPE_H_

#include "ArchivoLocal.h"
#include "ArchivoPipe.h"

typedef struct ArchivoPipe
{
	ArchivoLocal base;
	int pipes[2];
} ArchivoPipe;


ArchivoPipe *newArchivoPipe();

#endif
