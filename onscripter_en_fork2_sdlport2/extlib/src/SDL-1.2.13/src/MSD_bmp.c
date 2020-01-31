#include <string.h>
#include <windows.h>
#include "MSD_video.h"

#undef BI_RGB
#undef BI_RLE8
#undef BI_RLE4
#undef BI_BITFIELDS

#define BI_RGB 0
#define BI_RLE8	1
#define BI_RLE4	2
#define BI_BITFIELDS 3

MSD_Surface * MSD_LoadBMP_RW(MSD_RWops *src, int freesrc)
{
	int was_error;
	long fp_offset;
	int bmpPitch;
	int i, pad;
	MSD_Surface *surface;
	uint32_t Rmask;
	uint32_t Gmask;
	uint32_t Bmask;
	MSD_Palette *palette;
	uint8_t *bits;
	int ExpandBMP;

	char magic[2];
	uint32_t bfSize;
	uint16_t bfReserved1;
	uint16_t bfReserved2;
	uint32_t bfOffBits;

	uint32_t biSize;
	int32_t biWidth;
	int32_t biHeight;
	uint16_t biPlanes;
	uint16_t biBitCount;
	uint32_t biCompression;
	uint32_t biSizeImage;
	int32_t biXPelsPerMeter;
	int32_t biYPelsPerMeter;
	uint32_t biClrUsed;
	uint32_t biClrImportant;

	surface = NULL;
	was_error = 0;
	if ( src == NULL ) {
		was_error = 1;
		goto done;
	}

	fp_offset = MSD_RWtell(src);
	if (MSD_RWread(src, magic, 1, 2) != 2) {
		fprintf(stderr, "Error reading from datastream\n");
		was_error = 1;
		goto done;
	}
	if (strncmp(magic, "BM", 2) != 0) {
		fprintf(stderr, "File is not a Windows BMP file\n");
		was_error = 1;
		goto done;
	}
	bfSize		= MSD_ReadLE32(src);
	bfReserved1	= MSD_ReadLE16(src);
	bfReserved2	= MSD_ReadLE16(src);
	bfOffBits	= MSD_ReadLE32(src);

	biSize		= MSD_ReadLE32(src);
	if ( biSize == 12 ) {
		biWidth	= (uint32_t)MSD_ReadLE16(src);
		biHeight = (uint32_t)MSD_ReadLE16(src);
		biPlanes = MSD_ReadLE16(src);
		biBitCount = MSD_ReadLE16(src);
		biCompression = BI_RGB;
		biSizeImage	= 0;
		biXPelsPerMeter	= 0;
		biYPelsPerMeter	= 0;
		biClrUsed = 0;
		biClrImportant = 0;
	} else {
		biWidth	= MSD_ReadLE32(src);
		biHeight = MSD_ReadLE32(src);
		biPlanes = MSD_ReadLE16(src);
		biBitCount = MSD_ReadLE16(src);
		biCompression = MSD_ReadLE32(src);
		biSizeImage	= MSD_ReadLE32(src);
		biXPelsPerMeter	= MSD_ReadLE32(src);
		biYPelsPerMeter	= MSD_ReadLE32(src);
		biClrUsed = MSD_ReadLE32(src);
		biClrImportant = MSD_ReadLE32(src);
	}

	switch (biBitCount) {
	case 1:
	case 4:
		ExpandBMP = biBitCount;
		biBitCount = 8;
		break;

	default:
		ExpandBMP = 0;
		break;
	}

	Rmask = Gmask = Bmask = 0;
	switch (biCompression) {
	case BI_RGB:
		if (bfOffBits == (14 + biSize)) {
			switch (biBitCount) {
			case 15:
			case 16:
				Rmask = 0x7C00;
				Gmask = 0x03E0;
				Bmask = 0x001F;
				break;

			case 24:
				if (!MSD_IsLilEndian()) {
					Rmask = 0x000000FF;
					Gmask = 0x0000FF00;
					Bmask = 0x00FF0000;
					break;
				}
				/*pass?*/

			case 32:
				Rmask = 0x00FF0000;
				Gmask = 0x0000FF00;
				Bmask = 0x000000FF;
				break;
			
			default:
				break;
			}
			break;
		}
		/* Fall through -- read the RGB masks */

	case BI_BITFIELDS:
		switch (biBitCount) {
		case 15:
		case 16:
		case 32:
			Rmask = MSD_ReadLE32(src);
			Gmask = MSD_ReadLE32(src);
			Bmask = MSD_ReadLE32(src);
			break;

		default:
			break;
		}
		break;

	default:
		fprintf(stderr, "Compressed BMP files not supported\n");
		was_error = 1;
		goto done;
	}

	surface = MSD_CreateRGBSurface(MSD_SWSURFACE, biWidth, biHeight, biBitCount, Rmask, Gmask, Bmask, 0);
	if ( surface == NULL ) {
		was_error = 1;
		goto done;
	}

	palette = (surface->format)->palette;
	if (palette) {
		if (biClrUsed == 0) {
			biClrUsed = 1 << biBitCount;
		}
		if (biSize == 12) {
			for (i = 0; i < (int)biClrUsed; ++i) {
				MSD_RWread(src, &palette->colors[i].b, 1, 1);
				MSD_RWread(src, &palette->colors[i].g, 1, 1);
				MSD_RWread(src, &palette->colors[i].r, 1, 1);
				palette->colors[i].unused = 0;
			}	
		} else {
			for (i = 0; i < (int)biClrUsed; ++i) {
				MSD_RWread(src, &palette->colors[i].b, 1, 1);
				MSD_RWread(src, &palette->colors[i].g, 1, 1);
				MSD_RWread(src, &palette->colors[i].r, 1, 1);
				MSD_RWread(src, &palette->colors[i].unused, 1, 1);
			}	
		}
		palette->ncolors = biClrUsed;
	}

	if (MSD_RWseek(src, fp_offset + bfOffBits, MSD_RW_SEEK_SET) < 0) {
		fprintf(stderr, "Error seeking in datastream\n");
		was_error = 1;
		goto done;
	}

	bits = (uint8_t *)surface->pixels + (surface->h * surface->pitch);
	switch (ExpandBMP) {
	case 1:
		bmpPitch = (biWidth + 7) >> 3;
		pad  = (((bmpPitch) % 4) ? (4 - ((bmpPitch) % 4)) : 0);
		break;

	case 4:
		bmpPitch = (biWidth + 1) >> 1;
		pad  = (((bmpPitch) % 4) ? (4 - ((bmpPitch) % 4)) : 0);
		break;
	
	default:
		pad  = ((surface->pitch % 4) ? (4 - (surface->pitch % 4)) : 0);
		break;
	}
	while (bits > (uint8_t *)surface->pixels) {
		bits -= surface->pitch;
		switch (ExpandBMP) {
		case 1:
		case 4: {
				uint8_t pixel = 0;
				int shift = 8 - ExpandBMP;
				for (i = 0; i < surface->w; ++i) {
					if (i % (8/ExpandBMP) == 0) {
						if (!MSD_RWread(src, &pixel, 1, 1)) {
							fprintf(stderr, "Error reading from BMP\n");
							was_error = 1;
							goto done;
						}
					}
					*(bits+i) = (pixel>>shift);
					pixel <<= ExpandBMP;
				}	
			}
			break;

		default:
			if (MSD_RWread(src, bits, 1, surface->pitch) != surface->pitch ) {
				fprintf(stderr, "Error reading from datastream\n");
				was_error = 1;
				goto done;
			}
			if (!MSD_IsLilEndian()) {
				switch (biBitCount) {
					case 15:
					case 16: {
							uint16_t *pix = (uint16_t *)bits;
						for(i = 0; i < surface->w; i++)
								pix[i] = MSD_Swap16(pix[i]);
						break;
					}

					case 32: {
							uint32_t *pix = (uint32_t *)bits;
						for(i = 0; i < surface->w; i++)
								pix[i] = MSD_Swap32(pix[i]);
						break;
					}
				}
			}
			break;
		}
		if (pad) {
			uint8_t padbyte;
			for (i = 0; i < pad; ++i) {
				MSD_RWread(src, &padbyte, 1, 1);
			}
		}
	}
done:
	if (was_error) {
		if (src) {
			MSD_RWseek(src, fp_offset, MSD_RW_SEEK_SET);
		}
		if (surface) {
			MSD_FreeSurface(surface);
		}
		surface = NULL;
	}
	if (freesrc && src) {
		MSD_RWclose(src);
	}
	return surface;
}

int MSD_SaveBMP_RW (MSD_Surface *saveme, MSD_RWops *dst, int freedst)
{
	int is_no_error = 1;
	long fp_offset;
	int i, pad;
	MSD_Surface *surface;
	uint8_t *bits;

	char magic[2] = { 'B', 'M' };
	uint32_t bfSize;
	uint16_t bfReserved1;
	uint16_t bfReserved2;
	uint32_t bfOffBits;

	uint32_t biSize;
	int32_t biWidth;
	int32_t biHeight;
	uint16_t biPlanes;
	uint16_t biBitCount;
	uint32_t biCompression;
	uint32_t biSizeImage;
	int32_t biXPelsPerMeter;
	int32_t biYPelsPerMeter;
	uint32_t biClrUsed;
	uint32_t biClrImportant;

	surface = NULL;
	if (dst) {
		if (saveme->format->palette) {
			if ( saveme->format->BitsPerPixel == 8 ) {
				surface = saveme;
			} else {
				fprintf(stderr, "%d bpp BMP files not supported\n",
						saveme->format->BitsPerPixel);
				is_no_error = 0;
			}
		}
		else if ( (saveme->format->BitsPerPixel == 24) &&
				(MSD_IsLilEndian() ?
					((saveme->format->Rmask == 0x00FF0000) &&
					 (saveme->format->Gmask == 0x0000FF00) &&
					 (saveme->format->Bmask == 0x000000FF))
					:
					((saveme->format->Rmask == 0x000000FF) &&
					 (saveme->format->Gmask == 0x0000FF00) &&
					 (saveme->format->Bmask == 0x00FF0000)))
			  ) {
			surface = saveme;
		} else {
			MSD_Rect bounds;
			if (MSD_IsLilEndian()) {
				surface = MSD_CreateRGBSurface(MSD_SWSURFACE,
						saveme->w, saveme->h, 24,
						0x00FF0000, 0x0000FF00, 0x000000FF,
						0);
			} else {
				surface = MSD_CreateRGBSurface(MSD_SWSURFACE,
						saveme->w, saveme->h, 24,
						0x000000FF, 0x0000FF00, 0x00FF0000,
						0);			
			}
			if ( surface != NULL ) {
				bounds.x = 0;
				bounds.y = 0;
				bounds.w = saveme->w;
				bounds.h = saveme->h;
				if ( MSD_LowerBlit(saveme, &bounds, surface,
							&bounds) < 0 ) {
					MSD_FreeSurface(surface);
					fprintf(stderr, 
						"Couldn't convert image to 24 bpp\n");
					OutputDebugString("!!!!!!!!!!Couldn't convert image to 24 bpp\n");
					surface = NULL;
					is_no_error = 0;
				}
			}
		}
	}

	if (surface && (MSD_LockSurface(surface) == 0)) {
		const int bw = surface->w*surface->format->BytesPerPixel;

		bfSize = 0;
		bfReserved1 = 0;
		bfReserved2 = 0;
		bfOffBits = 0;

		fp_offset = MSD_RWtell(dst);
		MSD_RWwrite(dst, magic, 2, 1);
		MSD_WriteLE32(dst, bfSize);
		MSD_WriteLE16(dst, bfReserved1);
		MSD_WriteLE16(dst, bfReserved2);
		MSD_WriteLE32(dst, bfOffBits);

		biSize = 40;
		biWidth = surface->w;
		biHeight = surface->h;
		biPlanes = 1;
		biBitCount = surface->format->BitsPerPixel;
		biCompression = BI_RGB;
		biSizeImage = surface->h*surface->pitch;
		biXPelsPerMeter = 0;
		biYPelsPerMeter = 0;
		if (surface->format->palette) {
			biClrUsed = surface->format->palette->ncolors;
		} else {
			biClrUsed = 0;
		}
		biClrImportant = 0;

		MSD_WriteLE32(dst, biSize);
		MSD_WriteLE32(dst, biWidth);
		MSD_WriteLE32(dst, biHeight);
		MSD_WriteLE16(dst, biPlanes);
		MSD_WriteLE16(dst, biBitCount);
		MSD_WriteLE32(dst, biCompression);
		MSD_WriteLE32(dst, biSizeImage);
		MSD_WriteLE32(dst, biXPelsPerMeter);
		MSD_WriteLE32(dst, biYPelsPerMeter);
		MSD_WriteLE32(dst, biClrUsed);
		MSD_WriteLE32(dst, biClrImportant);

		if (surface->format->palette) {
			MSD_Color *colors;
			int ncolors;

			colors = surface->format->palette->colors;
			ncolors = surface->format->palette->ncolors;
			for (i = 0; i < ncolors; ++i) {
				MSD_RWwrite(dst, &colors[i].b, 1, 1);
				MSD_RWwrite(dst, &colors[i].g, 1, 1);
				MSD_RWwrite(dst, &colors[i].r, 1, 1);
				MSD_RWwrite(dst, &colors[i].unused, 1, 1);
			}
		}

		bfOffBits = MSD_RWtell(dst) - fp_offset;
		if (MSD_RWseek(dst, fp_offset+10, MSD_RW_SEEK_SET) < 0) {
			fprintf(stderr, "Error seeking in datastream\n");
			is_no_error = 0;
		}
		MSD_WriteLE32(dst, bfOffBits);
		if (MSD_RWseek(dst, fp_offset+bfOffBits, MSD_RW_SEEK_SET) < 0) {
			fprintf(stderr, "Error seeking in datastream\n");
			is_no_error = 0;
		}

		bits = (uint8_t *)surface->pixels + (surface->h * surface->pitch);
		pad  = ((bw % 4) ? (4 - (bw % 4)) : 0);
		while (bits > (uint8_t *)surface->pixels) {
			bits -= surface->pitch;
			if (MSD_RWwrite(dst, bits, 1, bw) != bw) {
				fprintf(stderr, "Error writing to datastream\n");
				break;
			}
			if (pad) {
				const uint8_t padbyte = 0;
				for (i = 0; i < pad; ++i) {
					MSD_RWwrite(dst, &padbyte, 1, 1);
				}
			}
		}
		bfSize = MSD_RWtell(dst) - fp_offset;
		if (MSD_RWseek(dst, fp_offset+2, MSD_RW_SEEK_SET) < 0) {
			fprintf(stderr, "Error seeking in datastream\n");
			is_no_error = 0;
		}
		MSD_WriteLE32(dst, bfSize);
		if (MSD_RWseek(dst, fp_offset + bfSize, MSD_RW_SEEK_SET) < 0) {
			fprintf(stderr, "Error seeking in datastream\n");
			is_no_error = 0;
		}
		MSD_UnlockSurface(surface);
		if (surface != saveme) {
			MSD_FreeSurface(surface);
		}
	}

	if (freedst && dst) {
		MSD_RWclose(dst);
	}

	return is_no_error ? 0 : -1;
}
