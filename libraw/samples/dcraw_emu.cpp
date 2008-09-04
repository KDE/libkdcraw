/* -*- C++ -*-
 * File: dcraw_emu.cpp
 * Copyright 2008 Alex Tutubalin <lexa@lexa.ru>
 * Created: Sun Mar 23,   2008
 *
 * LibRaw simple C++ API  (almost complete dcraw emulator)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

#include "libraw/libraw.h"
#ifdef WIN32
#define snprintf _snprintf
#endif


void usage(const char *prog)
{
    printf("dcraw_emu: almost complete dcraw emulator\n");
    printf("Usage:  %s [OPTION]... [FILE]...\n", prog);
    printf(
"-v        Verbose: print progress messages\n"
"-w        Use camera white balance, if possible\n"
"-a        Average the whole image for white balance\n"
"-A <x y w h> Average a grey box for white balance\n"
"-r <r g b g> Set custom white balance\n"
"+M/-M     Use/don't use an embedded color matrix\n"
"-C <r b>  Correct chromatic aberration\n"
"-k <num>  Set the darkness level\n"
"-S <num>  Set the saturation level\n"
"-n <num>  Set threshold for wavelet denoising\n"
"-H [0-9]  Highlight mode (0=clip, 1=unclip, 2=blend, 3+=rebuild)\n"
"-t [0-7]  Flip image (0=none, 3=180, 5=90CCW, 6=90CW)\n"
"-o [0-5]  Output colorspace (raw,sRGB,Adobe,Wide,ProPhoto,XYZ)\n"
"-j        Don't stretch or rotate raw pixels\n"
"-W        Don't automatically brighten the image\n"
"-b <num>  Adjust brightness (default = 1.0)\n"
"-q [0-3]  Set the interpolation quality\n"
"-h        Half-size color image (twice as fast as \"-q 0\")\n"
"-f        Interpolate RGGB as four colors\n"
"-m <num>  Apply a 3x3 median filter to R-G and B-G\n"
"-s [0..N-1] Select one raw image from input file\n"
"-4        Write 16-bit linear instead of 8-bit with gamma\n"
"-T        Write TIFF instead of PPM\n"
        );
    exit(1);
}



int main(int argc, char *argv[])
{
    if(argc==1) usage(argv[0]);

    LibRaw RawProcessor;
    int i,arg,c, verbose=0,ret;
    char opm,opt,*cp,*sp;

#define OUT RawProcessor.imgdata.params
    
  argv[argc] = "";
  for (arg=1; (((opm = argv[arg][0]) - 2) | 2) == '+'; ) 
      {
          opt = argv[arg++][1];
          if ((cp = strchr (sp="nbrkStqmHAC", opt)))
              for (i=0; i < "11411111142"[cp-sp]-'0'; i++)
                  if (!isdigit(argv[arg+i][0])) 
                      {
                          fprintf (stderr,"Non-numeric argument to \"-%c\"\n", opt);
                          return 1;
                      }
          switch (opt) 
              {
              case 'v':  verbose           = 1;  break;
                  
              case 'n':  OUT.threshold   = atof(argv[arg++]);  break;
              case 'b':  OUT.bright      = atof(argv[arg++]);  break;
              case 'r':
                  for(c=0;c<4;c++) 
                      OUT.user_mul[c] = atof(argv[arg++]);  
                  break;
              case 'C':  
                  OUT.aber[0] = 1 / atof(argv[arg++]);
                  OUT.aber[2] = 1 / atof(argv[arg++]);  
                  break;
              case 'k':  OUT.user_black  = atoi(argv[arg++]);  break;
              case 'S':  OUT.user_sat    = atoi(argv[arg++]);  break;
              case 't':  OUT.user_flip   = atoi(argv[arg++]);  break;
              case 'q':  OUT.user_qual   = atoi(argv[arg++]);  break;
              case 'm':  OUT.med_passes  = atoi(argv[arg++]);  break;
              case 'H':  OUT.highlight   = atoi(argv[arg++]);  break;
              case 's':  OUT.shot_select = abs(atoi(argv[arg])); break;
              case 'o':  OUT.output_color = atoi(argv[arg++]); break;
              case 'h':  OUT.half_size         = 1;		
                  // no break:  "-h" implies "-f" 
              case 'f':  
                  OUT.four_color_rgb    = 1;  
                  break;
              case 'A':  for(c=0; c<4;c++) OUT.greybox[c]  = atoi(argv[arg++]);
              case 'a':  OUT.use_auto_wb       = 1;  break;
              case 'w':  OUT.use_camera_wb     = 1;  break;
              case 'M':  OUT.use_camera_matrix = (opm == '+');  break;
              case 'j':  OUT.use_fuji_rotate   = 0;  break;
              case 'W':  OUT.no_auto_bright    = 1;  break;
              case 'T':  OUT.output_tiff       = 1;  break;
              case '4':  OUT.output_bps       = 16;  break;
              default:
                  fprintf (stderr,"Unknown option \"-%c\".\n", opt);
                  return 1;
              }
      }

  putenv ((char*)"TZ=UTC"); // dcraw compatibility, affects TIFF datestamp field

#define P1 RawProcessor.imgdata.idata
#define S RawProcessor.imgdata.sizes
#define C RawProcessor.imgdata.color
#define T RawProcessor.imgdata.thumbnail
#define P2 RawProcessor.imgdata.other


  for ( ; arg < argc; arg++)
        {
            char outfn[1024];

            if(verbose) printf("Processing file %s\n",argv[arg]);
            if( (ret = RawProcessor.open_file(argv[arg])) != LIBRAW_SUCCESS)
                {
                    fprintf(stderr,"Cannot open %s: %s\n",argv[arg],libraw_strerror(ret));
                    continue; // no recycle b/c open_file will recycle itself
                }
            if( (ret = RawProcessor.unpack() ) != LIBRAW_SUCCESS)
                {
                    fprintf(stderr,"Cannot unpack %s: %s\n",argv[arg],libraw_strerror(ret));
                    continue;
                }
            if (LIBRAW_SUCCESS != (ret = RawProcessor.dcraw_process()))
                {
                    fprintf(stderr,"Cannot do postpocessing on %s: %s\n",argv[arg],libraw_strerror(ret));
                    if(LIBRAW_FATAL_ERROR(ret))
                        continue; 
                }
            snprintf(outfn,sizeof(outfn),
                     "%s.%s",
                     argv[arg], OUT.output_tiff ? "tiff" : (P1.colors>1?"ppm":"pgm"));

            if(verbose) printf("Writing file %s\n",outfn);

            if( LIBRAW_SUCCESS != (ret = RawProcessor.dcraw_ppm_tiff_writer(outfn)))
                fprintf(stderr,"Cannot write %s: %s\n",outfn,libraw_strerror(ret));
            
            RawProcessor.recycle(); // just for show this call
        }
    return 0;
}
