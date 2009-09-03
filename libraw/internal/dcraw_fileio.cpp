/* 
   GENERATED FILE, DO NOT EDIT
   Generated from dcraw/dcraw.c at Wed Sep  2 11:06:07 2009
   Look into original file (probably http://cybercom.net/~dcoffin/dcraw/dcraw.c)
   for copyright information.
*/

#define CLASS LibRaw::
#include "libraw/libraw_types.h"
#define LIBRAW_LIBRARY_BUILD
#include "libraw/libraw.h"
#include "internal/defines.h"
#include "internal/var_defines.h"

/*
   Seach from the current directory up to the root looking for
   a ".badpixels" file, and fix those pixels now.
 */
void CLASS bad_pixels (const char *cfname)
{
  FILE *fp=0;
  char *fname, *cp, line[128];
  int len, time, row, col, r, c, rad, tot, n, fixed=0;

  if (!filters) return;
#ifdef LIBRAW_LIBRARY_BUILD
  RUN_CALLBACK(LIBRAW_PROGRESS_BAD_PIXELS,0,2);
#endif
  if (cfname)
    fp = fopen (cfname, "r");
  if (!fp) 
      {
#ifdef LIBRAW_LIBRARY_BUILD
          imgdata.process_warnings |= LIBRAW_WARN_NO_BADPIXELMAP;
#endif
          return;
      }
  while (fgets (line, 128, fp)) {
    cp = strchr (line, '#');
    if (cp) *cp = 0;
    if (sscanf (line, "%d %d %d", &col, &row, &time) != 3) continue;
    if ((unsigned) col >= width || (unsigned) row >= height) continue;
    if (time > timestamp) continue;
    for (tot=n=0, rad=1; rad < 3 && n==0; rad++)
      for (r = row-rad; r <= row+rad; r++)
	for (c = col-rad; c <= col+rad; c++)
	  if ((unsigned) r < height && (unsigned) c < width &&
		(r != row || c != col) && fc(r,c) == fc(row,col)) {
	    tot += BAYER2(r,c);
	    n++;
	  }
    BAYER2(row,col) = tot/n;
#ifdef DCRAW_VERBOSE
    if (verbose) {
      if (!fixed++)
	fprintf (stderr,_("Fixed dead pixels at:"));
      fprintf (stderr, " %d,%d", col, row);
    }
#endif
  }
#ifdef DCRAW_VERBOSE
  if (fixed) fputc ('\n', stderr);
#endif
  fclose (fp);
#ifdef LIBRAW_LIBRARY_BUILD
  RUN_CALLBACK(LIBRAW_PROGRESS_BAD_PIXELS,1,2);
#endif
}

void CLASS subtract (const char *fname)
{
  FILE *fp;
  int dim[3]={0,0,0}, comment=0, number=0, error=0, nd=0, c, row, col;
  ushort *pixel;
#ifdef LIBRAW_LIBRARY_BUILD
  RUN_CALLBACK(LIBRAW_PROGRESS_DARK_FRAME,0,2);
#endif

  if (!(fp = fopen (fname, "rb"))) {
#ifdef DCRAW_VERBOSE
    perror (fname); 
#endif
#ifdef LIBRAW_LIBRARY_BUILD
    imgdata.process_warnings |= LIBRAW_WARN_BAD_DARKFRAME_FILE;
#endif
    return;
  }
  if (fgetc(fp) != 'P' || fgetc(fp) != '5') error = 1;
  while (!error && nd < 3 && (c = fgetc(fp)) != EOF) {
    if (c == '#')  comment = 1;
    if (c == '\n') comment = 0;
    if (comment) continue;
    if (isdigit(c)) number = 1;
    if (number) {
      if (isdigit(c)) dim[nd] = dim[nd]*10 + c -'0';
      else if (isspace(c)) {
	number = 0;  nd++;
      } else error = 1;
    }
  }
  if (error || nd < 3) {
    fprintf (stderr,_("%s is not a valid PGM file!\n"), fname);
    fclose (fp);  return;
  } else if (dim[0] != width || dim[1] != height || dim[2] != 65535) {
#ifdef DCRAW_VERBOSE
      fprintf (stderr,_("%s has the wrong dimensions!\n"), fname);
#endif
#ifdef LIBRAW_LIBRARY_BUILD
      imgdata.process_warnings |= LIBRAW_WARN_BAD_DARKFRAME_DIM;
#endif
    fclose (fp);  return;
  }
  pixel = (ushort *) calloc (width, sizeof *pixel);
  merror (pixel, "subtract()");
  for (row=0; row < height; row++) {
    fread (pixel, 2, width, fp);
    for (col=0; col < width; col++)
      BAYER(row,col) = MAX (BAYER(row,col) - ntohs(pixel[col]), 0);
  }
  free (pixel);
  black = 0;
#ifdef LIBRAW_LIBRARY_BUILD
  RUN_CALLBACK(LIBRAW_PROGRESS_DARK_FRAME,1,2);
#endif
}

#ifndef NO_LCMS
void CLASS apply_profile (const char *input, const char *output)
{
  char *prof;
  cmsHPROFILE hInProfile=0, hOutProfile=0;
  cmsHTRANSFORM hTransform;
  FILE *fp;
  unsigned size;

  cmsErrorAction (LCMS_ERROR_SHOW);
  if (strcmp (input, "embed"))
    hInProfile = cmsOpenProfileFromFile (input, "r");
  else if (profile_length) {
#ifndef LIBRAW_LIBRARY_BUILD
    prof = (char *) malloc (profile_length);
    merror (prof, "apply_profile()");
    fseek (ifp, profile_offset, SEEK_SET);
    fread (prof, 1, profile_length, ifp);
    hInProfile = cmsOpenProfileFromMem (prof, profile_length);
    free (prof);
#else
    hInProfile = cmsOpenProfileFromMem (imgdata.color.profile, profile_length);
#endif
  } else
      {
#ifdef LIBRAW_LIBRARY_BUILD
          imgdata.process_warnings |= LIBRAW_WARN_NO_EMBEDDED_PROFILE;
#endif
#ifdef DCRAW_VERBOSE
          fprintf (stderr,_("%s has no embedded profile.\n"), ifname);
#endif
      }
  if (!hInProfile)
      {
#ifdef LIBRAW_LIBRARY_BUILD
          imgdata.process_warnings |= LIBRAW_WARN_NO_INPUT_PROFILE;
#endif
          return;
      }
  if (!output)
    hOutProfile = cmsCreate_sRGBProfile();
  else if ((fp = fopen (output, "rb"))) {
    fread (&size, 4, 1, fp);
    fseek (fp, 0, SEEK_SET);
    oprof = (unsigned *) malloc (size = ntohl(size));
    merror (oprof, "apply_profile()");
    fread (oprof, 1, size, fp);
    fclose (fp);
    if (!(hOutProfile = cmsOpenProfileFromMem (oprof, size))) {
      free (oprof);
      oprof = 0;
    }
#ifdef DCRAW_VERBOSE
  } else
    fprintf (stderr,_("Cannot open file %s!\n"), output);
#else
}
#endif
  if (!hOutProfile)
      {
#ifdef LIBRAW_LIBRARY_BUILD
          imgdata.process_warnings |= LIBRAW_WARN_BAD_OUTPUT_PROFILE;
#endif
          goto quit;
      }
#ifdef DCRAW_VERBOSE
  if (verbose)
    fprintf (stderr,_("Applying color profile...\n"));
#endif
#ifdef LIBRAW_LIBRARY_BUILD
  RUN_CALLBACK(LIBRAW_PROGRESS_APPLY_PROFILE,0,2);
#endif
  hTransform = cmsCreateTransform (hInProfile, TYPE_RGBA_16,
	hOutProfile, TYPE_RGBA_16, INTENT_PERCEPTUAL, 0);
  cmsDoTransform (hTransform, image, image, width*height);
  raw_color = 1;		/* Don't use rgb_cam with a profile */
  cmsDeleteTransform (hTransform);
  cmsCloseProfile (hOutProfile);
quit:
  cmsCloseProfile (hInProfile);
#ifdef LIBRAW_LIBRARY_BUILD
  RUN_CALLBACK(LIBRAW_PROGRESS_APPLY_PROFILE,1,2);
#endif
}
#endif
