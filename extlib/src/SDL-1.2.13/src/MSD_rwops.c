#include <malloc.h>
#include <string.h>
#include "MSD_video.h"

static int stdio_seek(MSD_RWops *context, int offset, int whence)
{
	if (fseek(context->hidden.stdio.fp, offset, whence) == 0) {
		return ftell(context->hidden.stdio.fp);
	} else {
		fprintf(stderr, "Error seeking in datastream\n");
		return -1;
	}
}

static int stdio_read(MSD_RWops *context, void *ptr, int size, int maxnum)
{
	size_t nread;

	nread = fread(ptr, size, maxnum, context->hidden.stdio.fp); 
	if (nread == 0 && ferror(context->hidden.stdio.fp)) {
		fprintf(stderr, "Error reading from datastream\n");
	}
	return nread;
}

static int stdio_write(MSD_RWops *context, const void *ptr, int size, int num)
{
	size_t nwrote;

	nwrote = fwrite(ptr, size, num, context->hidden.stdio.fp);
	if (nwrote == 0 && ferror(context->hidden.stdio.fp)) {
		fprintf(stderr, "Error writing to datastream\n");
	}
	return nwrote;
}

static int stdio_close(MSD_RWops *context)
{
	if (context) {
		if (context->hidden.stdio.autoclose) {
			fclose(context->hidden.stdio.fp);
		}
		MSD_FreeRW(context);
	}
	return 0;
}

static int mem_seek(MSD_RWops *context, int offset, int whence)
{
	uint8_t *newpos;

	switch (whence) {
	case MSD_RW_SEEK_SET:
		newpos = context->hidden.mem.base + offset;
		break;

	case MSD_RW_SEEK_CUR:
		newpos = context->hidden.mem.here + offset;
		break;
	
	case MSD_RW_SEEK_END:
		newpos = context->hidden.mem.stop + offset;
		break;
	
	default:
		fprintf(stderr, "Unknown value for 'whence'\n");
		return -1;
	}
	if (newpos < context->hidden.mem.base) {
		newpos = context->hidden.mem.base;
	}
	if (newpos > context->hidden.mem.stop) {
		newpos = context->hidden.mem.stop;
	}
	context->hidden.mem.here = newpos;
	return context->hidden.mem.here-context->hidden.mem.base;
}

static int mem_read(MSD_RWops *context, void *ptr, int size, int maxnum)
{
	size_t total_bytes;
	size_t mem_available;

	total_bytes = maxnum * size;
	if ((maxnum <= 0) || (size <= 0) || ((total_bytes / maxnum) != (size_t) size)) {
		return 0;
	}

	mem_available = context->hidden.mem.stop - context->hidden.mem.here;
	if (total_bytes > mem_available) {
		total_bytes = mem_available;
	}

	memcpy(ptr, context->hidden.mem.here, total_bytes);
	context->hidden.mem.here += total_bytes;

	return total_bytes / size;
}

static int mem_write(MSD_RWops *context, const void *ptr, int size, int num)
{
	if ((context->hidden.mem.here + (num*size)) > context->hidden.mem.stop) {
		num = (context->hidden.mem.stop - context->hidden.mem.here) / size;
	}
	memcpy(context->hidden.mem.here, ptr, num*size);
	context->hidden.mem.here += num*size;
	return num;
}

static int mem_writeconst(MSD_RWops *context, const void *ptr, int size, int num)
{
	fprintf(stderr, "Can't write to read-only memory\n");
	return -1;
}

static int mem_close(MSD_RWops *context)
{
	if (context) {
		MSD_FreeRW(context);
	}
	return 0;
}

MSD_RWops *MSD_RWFromFile(const char *file, const char *mode)
{
	MSD_RWops *rwops = NULL;
	FILE *fp = NULL;
	if (!file || !*file || !mode || !*mode) {
		fprintf(stderr, "MSD_RWFromFile(): No file or no mode specified\n");
		return NULL;
	}
	fp = fopen(file, mode);
	if (fp == NULL) {
		fprintf(stderr, "Couldn't open %s\n", file);
	} else {
		rwops = MSD_RWFromFP(fp, 1);
	}
	return rwops;
}

MSD_RWops *MSD_RWFromFP(FILE *fp, int autoclose)
{
	MSD_RWops *rwops = NULL;

	rwops = MSD_AllocRW();
	if (rwops != NULL) {
		rwops->seek = stdio_seek;
		rwops->read = stdio_read;
		rwops->write = stdio_write;
		rwops->close = stdio_close;
		rwops->hidden.stdio.fp = fp;
		rwops->hidden.stdio.autoclose = autoclose;
	}
	return rwops;
}

MSD_RWops *MSD_RWFromMem(void *mem, int size)
{
	MSD_RWops *rwops;

	rwops = MSD_AllocRW();
	if (rwops != NULL) {
		rwops->seek = mem_seek;
		rwops->read = mem_read;
		rwops->write = mem_write;
		rwops->close = mem_close;
		rwops->hidden.mem.base = (uint8_t *)mem;
		rwops->hidden.mem.here = rwops->hidden.mem.base;
		rwops->hidden.mem.stop = rwops->hidden.mem.base+size;
	}
	return rwops;
}

MSD_RWops *MSD_RWFromConstMem(const void *mem, int size)
{
	MSD_RWops *rwops;

	rwops = MSD_AllocRW();
	if (rwops != NULL) {
		rwops->seek = mem_seek;
		rwops->read = mem_read;
		rwops->write = mem_writeconst;
		rwops->close = mem_close;
		rwops->hidden.mem.base = (uint8_t *)mem;
		rwops->hidden.mem.here = rwops->hidden.mem.base;
		rwops->hidden.mem.stop = rwops->hidden.mem.base+size;
	}
	return rwops;
}

MSD_RWops *MSD_AllocRW(void)
{
	MSD_RWops *area;

	area = (MSD_RWops *)malloc(sizeof *area);
	if (area == NULL) {
		fprintf(stderr, "Out of memory\n");
	}
	return area;
}

void MSD_FreeRW(MSD_RWops *area)
{
	free(area);
}

uint16_t MSD_ReadLE16(MSD_RWops *src)
{
	uint16_t value;

	MSD_RWread(src, &value, (sizeof value), 1);
	return MSD_SwapLE16(value);
}

uint16_t MSD_ReadBE16(MSD_RWops *src)
{
	uint16_t value;

	MSD_RWread(src, &value, (sizeof value), 1);
	return MSD_SwapBE16(value);
}

uint32_t MSD_ReadLE32(MSD_RWops *src)
{
	uint32_t value;

	MSD_RWread(src, &value, (sizeof value), 1);
	return MSD_SwapLE32(value);
}

uint32_t MSD_ReadBE32(MSD_RWops *src)
{
	uint32_t value;

	MSD_RWread(src, &value, (sizeof value), 1);
	return MSD_SwapBE32(value);
}

int MSD_WriteLE16(MSD_RWops *dst, uint16_t value)
{
	value = MSD_SwapLE16(value);
	return MSD_RWwrite(dst, &value, (sizeof value), 1);
}

int MSD_WriteBE16(MSD_RWops *dst, uint16_t value)
{
	value = MSD_SwapBE16(value);
	return MSD_RWwrite(dst, &value, (sizeof value), 1);
}

int MSD_WriteLE32(MSD_RWops *dst, uint32_t value)
{
	value = MSD_SwapLE32(value);
	return MSD_RWwrite(dst, &value, (sizeof value), 1);
}

int MSD_WriteBE32(MSD_RWops *dst, uint32_t value)
{
	value = MSD_SwapBE32(value);
	return MSD_RWwrite(dst, &value, (sizeof value), 1);
}

uint16_t MSD_Swap16(uint16_t x) {
	return((x<<8)|(x>>8));
}

uint32_t MSD_Swap32(uint32_t x) {
	return((x<<24)|((x<<8)&0x00FF0000)|((x>>8)&0x0000FF00)|(x>>24));
}

int MSD_IsLilEndian() {
	uint32_t i = 1;
	uint8_t *ptr = (uint8_t*) &i;
	return (*ptr);
}
