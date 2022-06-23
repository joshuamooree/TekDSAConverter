/* 
 * Miscellaneous port info - 
 *   - executable will require ~8k stack space, ~20k RAM.
 *   - the typedef's for INT16, UINT16, and INT32 should set properly
 *     for your configuration - it is critical that INT32 be a 32 bit 
 *     integer, and that INT16, UINT16 be a 16 bit integers.
 */

#define MICROSOFT

#include <c:\include\stdio.h>
#include <c:\include\ctype.h>

#define MAX_LENGTH      1023    /* maximum line length */
#define MWIDTH          (MAX_LENGTH+1)

#define MAX_ZAP         16      /* maximum number of zapfile entries */
#define Y_QUAL          13      /* min height for icon, selector id  */
#define X_QUAL          10      /* min width for icon, selector id   */

#define NUM_COLORS      8

#define MAX_MULT        4       /* 4 x 4 target pattern */
#define MAX_DPAT        16      /* 16 x 16 dither pattern */

#define ENHANCED        1
#define LINE_IMAGE      2

#define DEFAULT_RES    -1

#define FALSE           0       
#define TRUE            1   

/* arbitrary directory entry limit */
#define MAX_ENTRY_COUNT     64
#define TIFF_VERSION        42
#define TIFF_COMPRESSION    32773       /* Macintosh 'Packbits'              */
#define BYTE                1
#define ASCII               2
#define SHORT               3
#define LONG                4
#define RATIONAL            5

static FILE *fo    = 0;
static FILE *fi    = 0;
static FILE *fm    = 0;
static FILE *fz    = 0;

#ifdef MICROSOFT
/* signed, unsigned 16 bit integers                                          */
typedef int            INT16;
typedef unsigned int   UINT16;

/* signed 32 bit integer                                                     */
typedef long           INT32;
#else
/* signed, unsigned 16 bit integers                                          */
typedef short          INT16;
typedef unsigned short UINT16;

/* signed 32 bit integer                                                     */
typedef int            INT32;
#endif

static int  height, width, mult, dp_x, dp_y;
static int  warn, compactOut;

typedef struct {
    int sx, sy, cx, cy;
} rect;

/* define regions to be excluded from the image 
 # enhancement processing (used to reduce selector 
 # background intensities).
 */
static rect ZapList[MAX_ZAP+1];

/* defines the rectangular region to be extracted
 # from the raw data file 
 */
static rect Extract;

/* default mapping [0..7] -> ??? array is used to map the 3bit hardware
 * color index to a grey scale value. For current model 11K instruments,
 * the index values correspond to the following screen entities -
 *  0 - Background
 *  1 - Waveform color 1, selectable text & icons
 *  2 - Waveform color 2
 *  3 - Waveform color 3
 *  4 - Waveform color 4
 *  5 - Window waveforms
 *  6 - Graticules, unselected text & icon backgrounds
 *  7 - Cursors, measurement zones
 */
static int map[NUM_COLORS];
static unsigned lmap[MAX_DPAT][MAX_DPAT];

/* index remap used for greyscaling prints (best with SCREEN dumps)          */
static int enhancedMap[NUM_COLORS] = {0,4,4,4,4,6,3,6};

/* index remap used for monochrome prints (use DRAFT mode)                   */
static int bilevelMap[NUM_COLORS] = {0,7,7,7,7,7,7,7};

/* index remap used for color prints (use SCREEN mode)                       */
static int colorMap[NUM_COLORS] = {0,1,2,3,4,5,6,7};
static int maxColor;

static INT32 currentFileOffset;

/* default color map                                                         */
static struct {
    INT16   red, green, blue;
} tiffRGB[16] = {
    0xffff, 0xffff, 0xffff,    
    0x0000, 0x0000, 0x0000,    
    0xffff, 0x6666, 0x9999, 
    0x6666, 0xffff, 0x0000,
    0x9999, 0x3333, 0xcccc,
    0x3333, 0xffff, 0xcccc,
    0x9999, 0x9999, 0x9999,
    0xffff, 0x0000, 0x0000,
    0, 0, 0,                    /* default index uses the range [0..7]       */
    0, 0, 0,
    0, 0, 0,
    0, 0, 0,
    0, 0, 0,
    0, 0, 0,
    0, 0, 0,
    0, 0, 0
};

/* scratchpad memory */
static char matrix[16][MWIDTH];

#define MAX_STRIP_COUNT 64
static struct {
    INT32   *stripOffset;
    INT16   *stripLength;
    int      stripIndex;
    int      stripRow;
    char     stripData[MAX_STRIP_COUNT * 6];
    INT32    stripDataOffset;
} stripInfo;
static int   stripsPerImage;
static int   rowsPerStrip;

int 
updateStripBlock(nbytes)
int nbytes;
{
    switch ( nbytes ) {
    case -1:    /* initialize */
        stripInfo.stripIndex  = 0;
        stripInfo.stripRow    = 0;
        stripInfo.stripDataOffset = currentFileOffset;
        stripInfo.stripOffset = (INT32 *)&stripInfo.stripData[0];
        stripInfo.stripLength = (INT16 *)(stripInfo.stripOffset + 
                                stripsPerImage);
        break;
    case -2:    /* flush */
        /* seek to appropriate location in the file                          */
        fseek(fo, stripInfo.stripDataOffset , 0);
        fwrite(stripInfo.stripOffset, sizeof(*stripInfo.stripOffset), 
               stripsPerImage, fo);
        fwrite(stripInfo.stripLength, sizeof(*stripInfo.stripLength), 
               stripsPerImage, fo);
        break;
    default:
        if ( !stripInfo.stripRow++ ) {
            stripInfo.stripOffset[stripInfo.stripIndex] = currentFileOffset;
            stripInfo.stripLength[stripInfo.stripIndex] = 0;
        }
        stripInfo.stripLength[stripInfo.stripIndex] += nbytes;
        if ( !(stripInfo.stripRow %= rowsPerStrip) ) {
            stripInfo.stripIndex++;
        }
        currentFileOffset += nbytes;
        break;
    }
}


static int BitsPerPixel = 1;

typedef struct {
    INT16   ByteOrder;
    INT16   Version;
    INT32   IFDOffset;
} tiff_header;

typedef struct {
    INT16   Tag;
    INT16   Type;
    INT32   Length;
    union {
        INT32   ValueOffset;
        INT32   LongValue;
        INT16   ShortValue;
    } u;
} tiff_entry;


/*
* TIFF file header
*/

static tiff_entry  entry_list[MAX_ENTRY_COUNT];
INT32
tiffHeader(title,dpi,line)
char *title;
int dpi, line;
{
    tiff_header header;
    tiff_entry  *entry;
    tiff_entry  *stripOffsets, *stripByteCounts;
    char  *data;
    INT16 datum, itmp;
    int   title_length, offset, ndx;
    int   rows, cols;
    int   bits;
    char  *tagB, *tagBase;
    INT16 *tagS;
    INT32 *tagL;

    /* setup for scratchpad use                                              */
    fseek(fo, currentFileOffset = sizeof(header), 0);
    tagB = tagBase = &matrix[0][0];
    tagS = (INT16 *)tagB;
    tagL = (INT32 *)tagB;

    /* determine byte ordering */
    datum = 0xff;
    if ( *(char *)&datum ) {    /* low, high */
        header.ByteOrder = 0x4949;      /* "II" */
    } else {    /* high, low */
        header.ByteOrder = 0x4D4D;  /* "MM" */
    }
    header.Version   = TIFF_VERSION;

    /* Directory Entries - set defaults                                      */
    for (entry = &entry_list[ndx = 0]; ndx++ < MAX_ENTRY_COUNT; ) {
        entry->Type = SHORT;
        entry->Length = 1;
        (entry++)->u.LongValue = 0;
    }
    entry = &entry_list[0];

    bits = 1;
    rows = Extract.sy - Extract.cy + 1;
    cols = Extract.cx - Extract.sx + 1;
    switch ( line ) {
    case 0:
        bits = 4;
        break;
    case (ENHANCED|LINE_IMAGE):
        rows *= mult;
        cols *= mult;
        break;
    case LINE_IMAGE:
        break;
    }
    /* NOTE: these calculations assume that 8/bits == integer                */
    rowsPerStrip = 8192 / (((cols * bits)+7)/8);
    stripsPerImage = (rows + (rowsPerStrip - 1)) / rowsPerStrip;

    /* SubFileType: full resolution image data                               */
    entry->Tag          = 255;
    entry->u.ShortValue = 1;
    entry++;

    /* ImageWidth:                                                           */
    entry->Tag          = 256;
    entry->u.ShortValue = cols;
    entry++;

    /* ImageLength: mult * height                                            */
    entry->Tag          = 257;
    entry->u.ShortValue = rows;
    entry++;

    /* BitsPerSample: 1 (default) for bilevel images, 4 for color palette    */
    if ( bits != 1 ) {
        entry->Tag          = 258;
        entry->u.ShortValue = bits;
        entry++;
    }

    /* Compression(259): Use default of 1 */
    if ( compactOut && bits < 8 ) {
        entry->Tag          = 259;
        entry->u.ShortValue = TIFF_COMPRESSION;
        entry++;
    }

    /* PhotometricInterpretation: 1 == normal video, 3 == Pallette           */
    entry->Tag          = 262;
    entry->u.ShortValue = (line ? 1 : 3);
    entry++;

    if ( line & ENHANCED ) {
        /* Thresholding(263): not relevant to color data                     */
        entry->Tag          = 263;
        entry->u.ShortValue = 2;
        entry++;

        /* CellWidth(264): use the 'mult' value                              */
        entry->Tag          = 264;
        entry->u.ShortValue = 2;
        entry++;
 
        /* CellLength(265): use the 'mult' value                             */
        entry->Tag          = 265;
        entry->u.ShortValue = 2;
        entry++;
    }

    /* Make: Dats us, folks (Tektronix)                                      */
    strcpy(tagB, "Tektronix");
    entry->Tag           = 271;
    entry->Type          = ASCII;
    entry->Length        = strlen(tagB) + 1;
    entry->u.ValueOffset = currentFileOffset;
    fwrite(tagB, sizeof(*tagB), entry->Length, fo);
    currentFileOffset += (entry->Length * sizeof(*tagB));
    entry++;

    /* Model: first chunk of the title block                                 */
    entry->Tag           = 272;
    entry->Type          = ASCII;
    /* find the end of the first piece of the title                          */
    for (data = title; isalnum(*data) || *data == ' '; data++);
    *data = '\0';   /* overwrite the non-space character termination         */
    entry->Length        = data - title + 1;
    entry->u.ValueOffset = currentFileOffset;
    fwrite(title, sizeof(*tagB), entry->Length, fo); 
    currentFileOffset += (entry->Length * sizeof(*tagB));
    title = data;
    entry++;
 
    /* StripOffsets(273): image data offset                                  */
    entry->Tag           = 273;
    entry->Type          = LONG;
    entry->Length        = stripsPerImage;
    stripOffsets         = entry++;

    /* Orientation(274): portrait vs. landscape                              */
    if ( rows < cols ) {
        entry->Tag          = 274;
        entry->u.ShortValue = 8;
        entry++;
    }

    /* RowsPerStrip(278): Manual recommends ~8k per strip - screw 'em. Problem
     * is that i'm trying to generate a serial stream. Reducing the blocks
     * gives me a bit more flexibility in placing the IFD
     */
    entry->Tag           = 278;
    entry->u.ShortValue  = rowsPerStrip;
    entry++;
 
    /* StripByteCounts: has to be determined _after_ compression             */
    entry->Tag           = 279;
    entry->Length        = stripsPerImage;
    stripByteCounts      = entry++;

    /* MinSampleValue(280): Use default of 0 */

    /* XResolution, YResolution are both 1/dpi                               */
    tagL[0] = dpi;
    tagL[1] = 1;

    entry->Tag           = 282;
    entry->Type          = RATIONAL;
    entry->u.ValueOffset = currentFileOffset;
    entry++;

    /* YResolution */
    entry->Tag           = 283;
    entry->Type          = RATIONAL;
    entry->u.ValueOffset = currentFileOffset;
 
    fwrite(tagL, sizeof(*tagL), entry->Length * 2, fo); 
    currentFileOffset += (entry->Length * 2 * sizeof(*tagL));
    entry++;

    /* DateTime: "YYYY:MM:DD HH:MM:SS", 20 characters, including null    */
    {
    int day, month, year;
    int hour, minute, second;

    while ( !isdigit(*data) ) {
        data++;
    }
    sscanf(data, "%d", &day);
    while ( !isalpha(*data) ) {
        data++;
    }
    switch ( *data ) {
    case 'J':   /* JAN, JUN, JUL                                             */
        month = (data[1] == 'A' ? 1 : (data[2] == 'N' ? 6 : 7));
        break;
    case 'F':   /* FEB                                                       */
        month = 2;
        break;
    case 'M':   /* MAR, MAY                                                  */
        month = (data[2] == 'R' ? 3 : 5);
        break;
    case 'A':   /* APR, AUG                                                  */
        month = (data[1] == 'P' ? 4 : 8);
        break;
    case 'S':   /* SEP                                                       */
        month = 9;
        break;
    case 'O':   /* OCT                                                       */
        month = 10;
        break;
    case 'N':   /* NOV                                                       */
        month = 11;
        break;
    case 'D':   /* DEC                                                       */
        month = 12;
        break;
    }
    data += 4;
    sscanf(data, "%d", &year);
    data += 3;
    while ( !isdigit(*data) ) {
        data++;
    }
    /* can use the timestamp as is, tho' it may lack a leading '0'           */
    if ( data[1] == ':' ) {
        *--data = '0';
    }
    data[8] = '\0';

    entry->Tag         = 306;
    entry->Type        = ASCII;
    entry->Length      = 20;
    entry->u.ValueOffset = currentFileOffset;
    fprintf(fo, "%04d:%02d:%02d %s", year + (year < 50 ? 2000 : 1900),
            month, day, data);
    currentFileOffset += (entry->Length * sizeof(*tagB));
    entry++;
    }

    /* ColorMap: RGB map values values - all 16 reds, then the 16 green, and
     * than the 16 blue. Range is defined to be [0..65535]
     */
    if ( !line ) {
        entry->Tag           = 320;
        entry->Length        = (3 * 16);
        entry->u.ValueOffset = currentFileOffset;
 
        for (ndx = 0; ndx < 16; ndx++) {      
            tagS[ndx]    = tiffRGB[ndx].red;            /* red           */
            tagS[ndx+16] = tiffRGB[ndx].green;          /* green         */
            tagS[ndx+32] = tiffRGB[ndx].blue;           /* blue          */
        }

        fwrite(tagS, sizeof(*tagS), entry->Length, fo); 
        currentFileOffset += (entry->Length * sizeof(*tagS));
 
        entry++;
    }

    /* Initialize the strip info buffer (uses currentFileOffset)             */
    updateStripBlock(-1);

    stripOffsets->u.ValueOffset = currentFileOffset;
    currentFileOffset += (stripOffsets->Length * sizeof(INT32));
    stripByteCounts->u.ValueOffset = currentFileOffset;
    currentFileOffset += (stripOffsets->Length * sizeof(INT16));

    /* MICROSOFT screws up when seeking past the end_of_file                 */
    fwrite(stripInfo.stripData, 1,
           stripsPerImage * (sizeof(INT16)+sizeof(INT32)), fo);

    /* write out the header                                                  */
    header.IFDOffset = currentFileOffset;
    fseek(fo, 0L, 0);
    fwrite(&header, sizeof(header), 1, fo);
    fseek(fo, currentFileOffset, 0);

    /* then the IFD block - entry count, entry list, 0 termination           */
    *tagS = entry - &entry_list[0];
    fwrite(tagS, sizeof(*tagS), 1, fo);
    currentFileOffset += sizeof(*tagS);

    fwrite(&entry_list[0], sizeof(*entry), *tagS, fo);
    currentFileOffset += (*tagS * sizeof(*entry));

    /* offset of next IFD */
    *tagL = 0;
    fwrite(tagL, sizeof(*tagL), 1, fo);
    currentFileOffset += sizeof(*tagL);

    /* number of bytes expected in the source image                          */
    return ( (INT32)height * width );
}


INT32
processHeader(dpi, image)
short dpi, image;
{
    char title[MAX_LENGTH];

    /* skip over header string */
    fgets(title,MAX_LENGTH,fi);

    /* make sure we found the first newline... */
    if (feof(fi)) {
        fprintf(stderr,"Fatal Error: premature end-of-file\n");
        exit(-1);
    }

    if (fscanf(fi,"%d%d",&width,&height) != 2 || feof(fi)) {
        fprintf(stderr,
          "Fatal Error: unable to obtain height, width values\n");
        exit(-1);
    }

    if (width > MAX_LENGTH) {
        fprintf(stderr,
          "Fatal Error: Width %1d exceeds max value allowed\n", width);
        exit(-1);
    }

    if (Extract.cx > width || Extract.cy > height) {
        fprintf(stderr,
          "Fatal Error: Extraction region (%1d,%1d),(%1d,%1d) out_of_bounds\n",
          Extract.sx, Extract.sy, Extract.cx, Extract.cy);
        exit(-1);
    }

    /* region default, invert y coordinates */
    if ( Extract.sx < 0 ) {
        Extract.sx = 0;
        Extract.sy = height - 1;
        Extract.cx = width - 1;
        Extract.cy = 0;
    } else {
        /* requires coordinate inversion                                     */
        Extract.cy = height - Extract.cy - 1;
        Extract.sy = height - Extract.sy - 1;
    }

    return tiffHeader(title, dpi, image);
}


#define MAXINBYTES  127
#define INITIAL     0
#define LITERAL     1
#define UNDECIDED   2

char calcRaw (n, lpIn, lpRawrunbuf)
UINT16  n;
char   *lpIn;
char   *lpRawrunbuf;
{
    char ncounts = 0;
    char thisbyte;
    char cnt = 1;
    char runbyte = *lpIn++;

    while (--n) {
        thisbyte = *lpIn++;
        if (thisbyte == runbyte) {
            cnt++;
        } else { /* write prev raw run, & start a new one */
            *lpRawrunbuf++ = cnt;
            ncounts++;
            cnt = 1;
            runbyte = thisbyte;
        }
    }
    *lpRawrunbuf = cnt;
    return (++ncounts);
}

/* as far as we can tell, this is a Mac ToolBox lookalike,
 * although it seems to be a bit smarter since the compression
 * ratios are sometimes higher
 *
 * n must be <= 127
 */
int 
packBitSub (plpIn, plpOut, n)
char   **plpIn;
char   **plpOut;
UINT16   n;
{
    char   *lpIn = *plpIn;
    char   *lpOut = *plpOut;
    char    runcount;
    char    rawrunbuf[MAXINBYTES];
    char   *lpRaw;
    char    nraw;
    char    state;
    char    rawcount;
    char    twins;
    int     ndx;


    /* initialize a few things
     */
    nraw = calcRaw(n, lpIn, (lpRaw = (char *) rawrunbuf));
    state = INITIAL;

    /* go through the raw run count array
     */
    while (nraw--) {

        rawcount = *lpRaw++;

        if (state == INITIAL) {
            if (rawcount == 1) {
                state = LITERAL;
                runcount = 1;
            } else if (rawcount == 2) {
                state = UNDECIDED;
                runcount = 2;
            } else {    /* rawcount >= 3 */
                /* state = INITIAL; */
                /* write replicate run and update ptrs
                 */
                *lpOut++ = -(rawcount - 1);
                *lpOut++ = *lpIn;
                lpIn += rawcount;
            }

        } else if (state == LITERAL) {
            if (rawcount < 3) {
                runcount += rawcount;
            } else {
                state = INITIAL;
                /* write literal run and update ptrs
                 */
                *lpOut++ = runcount - 1;
                for (ndx = runcount; ndx-- > 0; ) {
                    lpOut[ndx] = lpIn[ndx];
                }
                lpOut += runcount;
                lpIn += runcount;
                /* write replicate run and update ptrs
                 */
                *lpOut++ = -(rawcount - 1);
                *lpOut++ = *lpIn;
                lpIn += rawcount;
            }

        } else {    /* state = UNDECIDED */
            if (rawcount == 1) {
                state = LITERAL;
                runcount++;
            } else if (rawcount == 2) {
                /* state = UNDECIDED */
                runcount += 2;
            } else {    /* rawcount >= 3 */
                state = INITIAL;
                /* write out runcount/2 twin replicate runs */
                for (twins = (runcount>>1); twins--; ) {
                    *lpOut++ = -1;
                    *lpOut++ = *lpIn;
                    lpIn += 2;
                }
                /* write out this replicate run
                 */
                *lpOut++ = -(rawcount - 1);
                *lpOut++ = *lpIn;
                lpIn += rawcount;
            }
        } /* end of UNDECIDED case */

    } /* end of main for loop */

    /* clean up hanging states
     */
    if (state == LITERAL) {
        /* write out literal run
         */
        *lpOut++ = runcount - 1;
        for (ndx = runcount; ndx-- > 0; ) {
            lpOut[ndx] = lpIn[ndx];
        }
        lpOut += runcount;
        lpIn += runcount;
    }
    else if (state == UNDECIDED) {
        /* write out runcount/2 twin replicate runs
         */
        for (twins = (runcount>>1); twins--; ) {
            *lpOut++ = -1;
            *lpOut++ = *lpIn;
            lpIn += 2;
        }
    }

    /* calculate the length of the compressed string, set up return values
     */
    n = (lpOut - *plpOut);

    *plpIn = lpIn;
    *plpOut = lpOut;

    return n;

} /* that's all, folks */


/* if you might have more than 127 input bytes, call this routine instead
 * of the basic PackBits
 */
int 
packBits (plpIn, plpOut, n)
char   *plpIn;
char   *plpOut;
UINT16  n;
{
    UINT16  topack;
    int     total;

    for (total = 0; n; n -= topack) {
        total += packBitSub(&plpIn, &plpOut, 
                            (topack = (n < MAXINBYTES) ? n : MAXINBYTES));
    }
    return total;
}

/*
* TIFF output
*/
static int bit[]  = {0x1,0x2,0x4,0x8,0x10,0x20,0x40};
static int pbit[] = {0x80,0x40,0x20,0x10,0x8,0x4,0x2,0x1};

int
kickit(buf, width, row, line)
char *buf;
int  width, line;
{
    int  ndx, i, scanlength;
    char lbuf[MAX_MULT*MAX_LENGTH];
    char *src, *dst, *tag;
    int  limit = mult * width;
    int  nbytes;

    tag = &lbuf[128];
    nbytes = 0;

    /* if not a enhanced line image, write */
    if ( !(line & LINE_IMAGE) ) 
    {
        if ( BitsPerPixel == 8 ) {
            fwrite(buf, 1, (nbytes = width), fo);
        }
        else    /* need to pack the values [MSB..LSB] */
        {
            for ( i = ndx = 0, dst = tag; ndx < width; ndx++) 
            {
                *dst = (*dst << BitsPerPixel) + buf[ndx];
                if ( (i += BitsPerPixel) >= 8 ) {
                    i = 0;
                    dst++;
                }
            }
            while ( i++ < 8 ) {
                *dst <<= 1;
            }
            if ( compactOut ) {
                nbytes = packBits(tag, lbuf, (dst - tag));
                fwrite(lbuf, 1, nbytes, fo);
            }
            else {
                fwrite(tag, 1, nbytes = (dst - tag), fo);
            }
        }
        updateStripBlock(nbytes);
    }
    else {

        /* length of output line in bytes */
        scanlength = (((width*mult) + 7)/8);

        /* clear the local buffer                                            */
        for (ndx = scanlength; ndx--; tag[ndx] = 0);

        for (row = (((height - row) * mult) - 1) % dp_y, i = mult; 
          i--; 
          row = (row + dp_y - 1) % dp_y)
        {
            register 
            unsigned value = 0;
            register
            int j;
            unsigned *map = &lmap[row][0];
            char pixel;

            for (src = buf, dst = tag, j = 0; j < limit; )
            {
                if ( !(j % mult) ) {
                    pixel = *src++;
                }

                /* turn on the requisite bits */
                value <<= 1;
                if (pixel <= map[j % dp_x]) {
                    value |= 0x1;
                }

                /* finished this byte */
                if ( !(++j & 0x7) ) {
                    *dst++ = (char)value;
                    value  = 0;
                }
            }
            /* take care of leftover bits */
            while ( j++ % 8 ) value <<= 1;
            *dst = (char)value;

            if ( compactOut ) {
                nbytes = packBits(tag, lbuf, scanlength);
                fwrite(lbuf, 1, nbytes, fo);
            }
            else {
                fwrite(tag, 1, (nbytes = scanlength), fo);
            }
            updateStripBlock(nbytes);
        }
    }
}

/*

*/
unsigned
gethex()
{
    int byte;

    while ( !fscanf(fi,"%2x",&byte) && !feof(fi)) {
        if ( warn )
            fprintf(stderr, "Warning: Illegal character <%#02x> encountered in the input stream\n",
          fgetc(fi));
        else fgetc(fi);
    }
    return byte;
}

unsigned
getbin(cp)
char **cp;
{
    return fgetc(fi);
}

/*
 *  Function reads in the data, decompresses & remaps the color values
 *  as necessary, and fills in the matrix
 */

read_raster(matrix, tagbits, row, cmp, inbyte, nbytes)
char matrix[][MWIDTH];
int  *tagbits, cmp;
unsigned (*inbyte)();
INT32  *nbytes;
{
    int   datum, cnt, extra;
    char  *raster = matrix[row & 0xf], *limit;

    if (cmp)            /* compacted data */
    {
        for (limit = raster + width; 
          (*nbytes) > 0 && raster < limit && !feof(fi); )
        {
            raster[0] = map[(datum = (*inbyte)()) & 0x7];
            datum >>= 3;
            raster[1] = map[datum & 0x7];
            datum >>= 3;

            /* extended runlength */
            if ( !datum ) 
            {
                datum = (*inbyte)();
                if ( datum < 4 ) {
                    extra = (*inbyte)();
                    datum = (datum << 8) + extra;
                }
            }
            *nbytes -= (datum *= 2);
            for (raster += 2, datum -= 2; datum--; raster++) {
                raster[0] = raster[-2];
            }
        } 
        if ( raster != limit && warn ) {
            fprintf(stderr, "Warning: row %1d decompaction overflow\n", row);
        }
    }
    else                /* raw pixel stream */
    {
        for (cnt = width; (*nbytes)-- > 0 && cnt-- && !feof(fi); ) {
            *raster++ = map[(*inbyte)()];
        }
    }
    *tagbits |= (0x1 << (row & 0xf));
    *raster   = '\0';
}

disqualify_regions( matrix, row )
char matrix[][MWIDTH];
int  row;
{
    register
    int  ndx;
    int  col;
    char *pt, *base;

    if ( ZapList[0].sx >= 0 ) 
    {
        char *base = &matrix[row & 0xf][0];

        /* move origin */
        row = height - row - 1;

        /* for each region in the ZapList */
        for ( ndx = 0; ZapList[ndx].sx >= 0; ndx++ )
        {
            /* check to see if row is inbounds */
            if ( row < ZapList[ndx].sy || row > ZapList[ndx].cy ) 
                continue;

            /* tag the pixels as already processed */
            for ( pt = base + ZapList[ndx].sx, 
              col = ZapList[ndx].cy - ZapList[ndx].sx + 1;
              col-- > 0; ) *pt++ |= 0x80;
        }
    }
}

filter_matrix(matrix, tagbits, row)
char matrix[][MWIDTH];
int *tagbits, row;
{
    static char mem[MWIDTH];
    char   *cp1, *cp2, *pre, qual, newval;
    int    ndx1, ndx2, cnt, bar, qf, pf, start, limit;
    int    ras = row - Y_QUAL;

    if ( row < Y_QUAL || row < (Extract.cy - Y_QUAL) || 
      row > (Extract.sy + Y_QUAL) ) {
        if ( !row ) {
            /* zero the prequalification markers */
            for ( cnt = 0; cnt < MWIDTH; mem[cnt++] = 0 );
        }
        *tagbits &= ~(0x1 << (row&0xf));
        return;
    }

    *tagbits &= ~(0x1 << (row&0xf));

    /* check for prior qualification */
    for (cnt = 0, cp1 = matrix[(row-1)&0xf], cp2 = matrix[row&0xf]; 
      cnt <= width; ) 
    {
        if ( mem[cnt] ) {
            for (qual = mem[ndx1=cnt]; mem[ndx1] == qual && 
              (cp1[ndx1] >= qual || cp2[ndx1] >= qual); ndx1++);

            if ( (ndx1 - cnt) >= X_QUAL ) {
                /* mark qualified points */
                for ( newval = (qual-1) | 0x80; cnt < ndx1; cnt++ ) {
                    if ( cp1[cnt] == qual )
                        cp1[cnt] = newval;
                    else cp1[cnt] |= 0x80;
                } 
            }
            /* clear the prior qualification */
            while ( cnt < ndx1 ) {
              mem[cnt++] = 0;
            }
            if ( mem[ndx1] == qual ) mem[cnt++] = 0;
        } else cnt++;
    }

    /* identify new regions */
    for ( cnt = 0, limit = width - X_QUAL + 1; cnt < limit; )
    {
        /* minimum qualification is a two pixel diagonal */
        if ( (qual = matrix[ras&0xf][cnt]) <= 0 ||
          (qual > matrix[(ras+1)&0xf][cnt+1]) ) {
            cnt++;
            continue; 
        }

        /* then try a X_QUAL by Y_QUAL region */
        for ( pf = qf = FALSE, bar = ras; bar < row; bar++ )
        {
            if ( qf )
            {
                for (ndx1 = 0, cp1 = matrix[bar&0xf]+cnt;
                  ndx1++ < X_QUAL && *cp1++ >= qual; );
            }
            else
            {
                for (ndx1 = 0, cp1 = matrix[bar&0xf]+cnt;
                  ndx1++ < X_QUAL && *cp1 >= qual; ) {
                    if ( *cp1++ != qual ) qf = TRUE;
                }
            }

            /* premature exit */
            if ( !pf && ndx1 <= X_QUAL ) break;

            /* current row satisfied min requirement */
            pf = ndx1 > X_QUAL;
        }

        /* dropped out of initial X_QUAL * Y_QUAL */
        if ( bar != row ) {
            for ( cp1 = matrix[ras&0xf]; ndx1-- > 0 && 
              cp1[cnt++] >= qual; );
            continue;
        }

        for ( ndx1 = cnt + X_QUAL; ndx1 < width; ndx1++)
        {
            if ( qf ) {
                for (bar = ras; bar < (row-1) &&
                  ( matrix[bar&0xf][ndx1] >= qual || 
                    matrix[(bar+1)&0xf][ndx1] >= qual ); bar++ );
            } else {
                for (bar = ras; bar < (row-1) &&
                  ( matrix[bar&0xf][ndx1] >= qual || 
                    matrix[(bar+1)&0xf][ndx1] >= qual ); bar++ )
                    if ( matrix[bar&0xf][ndx1] > qual ) qf = TRUE;
            }
            if ( bar < (row-1) ) break;
        }

        /* fully qualified region? */
        if ( qf ) {
            newval = (qual-1) | 0x80;
            for (ndx2 = cnt; ndx2 < ndx1; mem[ndx2++] = qual);
        } else qual = 0xff;

        for ( bar = ras; bar < row; bar++)
        {
            for (cp1 = &matrix[bar&0xf][ndx2=cnt]; ndx2++ < ndx1; cp1++) {
                if ( *cp1  == qual ) *cp1 = newval;
                else if (*cp1 > qual) *cp1 = (*cp1 + 1) | 0x80;
                else *cp1 |= 0x80;
            }
        }

        cnt = ndx1;
    }

    *tagbits |= (0x1 << (ras&0xf));

    /* take care of remaining line(s) */
    if ( row == (height-1) || (row == Extract.sy + (Y_QUAL-1)) ) {
        for (ndx1 = width, cp1 = matrix[(row+1)&0xf]; ndx1; cp1[--ndx1] = 0);
        filter_matrix(matrix, tagbits, row+1);
        for (ndx1 = row - (Y_QUAL - 1); ndx1 <= row; 
          *tagbits |= (0x1 << (ndx1++ & 0xf)));
    }
}

write_raster(matrix, tagbits, start, lngth, row, image )
char matrix[][MWIDTH];
int  *tagbits, start, lngth, row, image;
{
    int cnt, mask = (0x1 << (row & 0xf));
    char *cpA, *cpB;

    if ( *tagbits & ~mask )
    {
        for ( row -= 15; *tagbits; row++, *tagbits &= ~mask )
        {
            mask = (0x1 << (row & 0xf));

            if ( (*tagbits & mask) && 
              row <= Extract.sy && row >= Extract.cy )
            {
                cpA = &matrix[row & 0xf][start];

                /* clear the tag bits */
                if ( image & ENHANCED ) {
                    for ( cpB = cpA, cnt = lngth; cnt-- > 0; *cpB++ ) 
                      *cpB &= ~0x80;
                }

                kickit(cpA, lngth, row, image);
            }
        }
    }
    else if ( *tagbits && row <= Extract.sy && row >= Extract.cy )
    {
        cpA = &matrix[row & 0xf][start];

        /* clear the tag bits */
        if ( image & ENHANCED ) {
            for ( cpB = cpA, cnt = lngth; cnt-- > 0; *cpB++ ) 
              *cpB &= ~0x80;
        }

        kickit(cpA, lngth, row, image);
        *tagbits = 0;
    }
}

processData(image, binhex, cmp, nbytes)
int   image, binhex, cmp;
INT32 nbytes;
{
    int  cnt, extra;
    unsigned  (*inbyte)();
    int   swidth = Extract.cx - Extract.sx + 1 ;
    char  *cp;
    int   row;
    int   zapflag;
    int   tagbits = 0;

    /* NOTE: width is the number of columns in the source image,
     # columns is the number of image columns to be extracted...
     */

    /* binary format terminates title, text info with <LF><\0> */ 
    if (!binhex ) {
        fgetc(fi); fgetc(fi);
    }

    /* input routine */
    if ( binhex ) {
        inbyte = gethex;
    }
    else {
        inbyte = getbin;
    }

    /* flag existence of enhance suppression */
    zapflag = ZapList[0].sx >= 0;

    /* zero the scratchpad memory block */
    for (cnt = sizeof(matrix), cp = matrix[0]; cnt--; *cp++ = 0);

    for ( row = 0; row < height; row++)
    {
        read_raster(matrix, &tagbits, row, cmp, inbyte, &nbytes);
        if ( image == (LINE_IMAGE | ENHANCED) ) {
            filter_matrix(matrix, &tagbits, row);
            if ( zapflag ) {
                disqualify_regions( matrix, row );
            }
        }
        write_raster(matrix, &tagbits, Extract.sx, swidth, row, image);
    }

    if (nbytes > 0) {
        fprintf(stderr,"Fatal Error: EOF with %1d bytes remaining\n",nbytes);
    }
    else 
    {
        /* skip newline, EOF character */
        fgetc(fi); fgetc(fi);
        /* who knows what these are? */
        fgetc(fi); fgetc(fi);

        if (!feof(fi)) 
        {
            int ndx = 0;

            while (!feof(fi)) {
                fgetc(fi);
                ndx++;
            }
            fprintf(stderr,
              "Error: end-of-file not found, %1d byte overrun\n",ndx);
        }
        else {
            /* write the completed strip data block to disk                  */
            updateStripBlock(-2);
        }
    }
}


main (argc,argv)
int argc;
char **argv;
{
    int  ndx, itmp, line;
    INT32 nbytes;
    char *infile  = 0;
    char *outfile = 0;
    char *mapfile = 0;
    char *zapfile = 0;
    int  binhex, compactIn, dpi;
    char fname[40], *fp, *cp;
    char *null = "";
    int  flag, *tag;

    binhex     = TRUE;
    compactIn  = TRUE;
    dpi        = DEFAULT_RES;
    line       = (LINE_IMAGE | ENHANCED);
    mult       = 1;
    compactOut = -1;
    maxColor   = 7;

    warn      = FALSE;

    /* mark extraction region */
    Extract.sx  = -1;

    /* process command line arguments */
    while (--argc > 0) 
    {
        char *tag = *++argv;

        switch (*tag++)
        {
        case '-': 
            flag = FALSE; 
            break;
        case '+': 
            flag = TRUE; 
            break;
        default:
            if (argc == 1 && !infile && !outfile) 
            {
                cp = infile = argv[0];
                fp = outfile = fname;
                while ((*fp = *cp++) && *fp++ != '.') ;
                if (fp[-1] == '.') fp--;
                strcpy(fp, ".tif");
                argc--;
                continue;
            }
            else {
                fprintf(stderr,"Fatal Error: flags [+/- imozxcpdelEw]\n");
                exit ( -1 );
            }
        }

        /* check for flag */
        while ( *tag )
        {
            switch ( *tag++ ) 
            {
            case 'd':
                if (*tag) {
                    cp = tag;
                    tag = null;
                }
                else {
                    cp = *++argv;
                    argc--;
                }
                if (sscanf(cp,"%d",&itmp) && itmp > 0 && itmp <= 300) {
                    dpi = itmp;
                }
                else {
                    fprintf(stderr,"Error: bad dpi value %1d",itmp);
                }
                break;
            case 'E':
                if (*tag) {
                    cp = tag;
                    tag = null;
                }
                else {
                    cp = *++argv;
                    argc--;
                }
                if (!(sscanf(cp,"%d",&Extract.sx) && 
                    argc-- && sscanf(*++argv,"%d",&Extract.sy) && 
                    argc-- && sscanf(*++argv,"%d",&Extract.cx) && 
                    argc-- && sscanf(*++argv,"%d",&Extract.cy))) {
                    fprintf(stderr,"Fatal Error: bad extraction spec\n");
                    exit(-1);
                }
                break;
            case 'e':
                if (flag) {
                    line = (LINE_IMAGE | ENHANCED);
                } else {
                    line = LINE_IMAGE;
                }
                break;
            case 'l':
                if ( flag ) {
                    line = LINE_IMAGE;
                } else {
                    line = 0;
                }
                break;
            case 'x':   /* default */
                binhex = flag;
                break;
            case 'c':
                compactIn = flag;
                break;
            case 'p':
                compactOut = flag;
                break;
            case 'o':
                if ( *tag ) {
                    outfile = tag;
                    tag = null;
                }
                else {
                    outfile = *++argv;
                    argc--;
                }
                break;
            case 'm':
                if ( *tag ) {
                    mapfile = tag;
                    tag = null;
                }
                else {
                    mapfile = *++argv;
                    argc--;
                }
                break;
            case 'z':
                if ( *tag ) {
                    zapfile = tag;
                    tag = null;
                }
                else {
                    zapfile = *++argv;
                    argc--;
                }
                break;
            case 'i':
                if ( *tag ) {
                    infile = tag;
                    tag = null;
                }
                else {
                    infile = *++argv;
                    argc--;
                }
                break;
            default:    /* unknown flag */
                fprintf(stderr,"Error: flags [+/- imozxcdelpEw]\n");
                break;
            }   /* switch */
        }
    }

    /* check for 'default' compactOut                                        */
    if ( compactOut < 0 ) {
        /* defaults on for the bilevel images, off for color palette         */
        compactOut = (line != 0);
    }

    /* default resolution is 300 dpi for line images, 150 dpi for contone    */
    if ( dpi == DEFAULT_RES ) {
        dpi = ((line & LINE_IMAGE) ? 300 : 150);
    }

    /* override the requisite defaults                                       */
    switch ( line ) 
    {
    case 0:
        tag = colorMap;
        BitsPerPixel = 4;
        break;
    case LINE_IMAGE:
        dp_x = dp_y = 1;
        lmap[0][0] = 0;
        tag = bilevelMap;
        break;
    case (LINE_IMAGE|ENHANCED):
        mult = 2;
        dp_x = 8;
        dp_y = 4;
        tag = enhancedMap;
        lmap[0][0] = 0; lmap[0][1] = 5; lmap[0][2] = 5; lmap[0][3] = 1;
          lmap[0][4] = 5; lmap[0][5] = 3; lmap[0][6] = 3; lmap[0][7] = 5;
        lmap[1][0] = 5; lmap[1][1] = 3; lmap[1][2] = 3; lmap[1][3] = 5;
          lmap[1][4] = 1; lmap[1][5] = 5; lmap[1][6] = 5; lmap[1][7] = 1;
        lmap[2][0] = 5; lmap[2][1] = 3; lmap[2][2] = 3; lmap[2][3] = 5;
          lmap[2][4] = 0; lmap[2][5] = 5; lmap[2][6] = 5; lmap[2][7] = 1;
        lmap[3][0] = 1; lmap[3][1] = 5; lmap[3][2] = 5; lmap[3][3] = 1;
          lmap[3][4] = 5; lmap[3][5] = 3; lmap[3][6] = 3; lmap[3][7] = 5;
        break;
    }

    /* copy in the default index remap values                                */
    for (ndx = 0; ndx < NUM_COLORS; ndx++) {
        map[ndx] = tag[ndx];
    }

    /* output file specified */
    if ( outfile ) 
    {
#ifdef MICROSOFT
        if (!(fo = fopen(outfile, "w+b")))
#else
        if (!(fo = fopen(outfile, "w+")))
#endif
        {
            fprintf(stderr, "Fatal Error: Unable to open <%s> for output\n",
                    outfile);
            exit (-1);
        }
    }

    /* mapfile specified (supported for (LINE|ENHANCED) mode images          */
    if ( mapfile )
    {
        /* color palette implementation is limited to four bits              */
        int limit = line ? 0x7f : 0xf;

        if (!(fm = fopen(mapfile, "r")))
        {
            fprintf(stderr, "Fatal Error: Unable to find map file <%s>\n",
              mapfile);
            exit (-1);
        }

        /* read in map file values */
        ndx = maxColor = 0;
        do {
            fscanf(fm,"%d",map+ndx);
            if (map[ndx] > maxColor) {
                maxColor = map[ndx];
            }
        } while (map[ndx++] <= limit && ndx < NUM_COLORS);

        if (ndx != NUM_COLORS || map[ndx-1] > limit)
        {
            fprintf(stderr, "Fatal Error: Need 8 remap indices [0..0x%02x]\n",
                    limit);
            exit (-1);
        }

        if ( (line & LINE_IMAGE) )
        {
            int i, max = 1;

            fscanf(fm, "%d%d%d", &mult, &dp_x, &dp_y);
            if ( mult < 1 || mult > MAX_MULT ) {
                fprintf(stderr,"Fatal Error: Mult val range [1..4]\n");
                exit ( -1 );
            }

            for ( ndx = 0; ndx < dp_y && !feof(fm); ndx++ )
            {
                for ( i = 0; i < dp_x && !feof(fm); i++ )
                {
                    fscanf(fm, "%d", &lmap[ndx][i]);
                    if ( lmap[ndx][i] > 7 ) {
                        fprintf(stderr,
                          "Fatal Error: Map val range [0..%1d]\n", max );
                        exit ( -1 );
                    }
                }
            }
            if ( feof(fm) ) {
                fprintf(stderr,"Fatal Error: insufficient map values\n");
                exit ( -1 );
            }
        }
        else if ( !line ) {
            int r, g, b;
            for ( ndx = 0; ndx < maxColor && !feof(fm); ndx++ )
            {
                fscanf(fm, "%d%d%d", &r, &g, &b);
                if ( sizeof(int) != sizeof(INT16) &&
                     ((r & ~0xffff) || (g & ~0xffff) || (b & ~0xffff)) ) {
                    fprintf(stderr, "Fatal Error: RGB val range [0..0xffff]\n");
                    exit ( -1 );
                }
                tiffRGB[ndx].red = r;
                tiffRGB[ndx].green = g;
                tiffRGB[ndx].blue = b;
            }
            if ( feof(fm) ) {
                fprintf(stderr,"Fatal Error: insufficient RGB values\n");
                exit ( -1 );
            }
        }
    }

    /* zapfile specified */
    if ( zapfile && (line & ENHANCED))
    {
        if (!(fz = fopen(zapfile, "r")))
        {
            fprintf(stderr,
              "Fatal Error: Unable to find disqualification file <%s>\n",
              zapfile);
            exit (-1);
        }
    }

    /* input file specified */
    if ( infile )
    {
#ifdef MICROSOFT
        if (!(fi = fopen(infile, "r+b")))
#else
        if (!(fi = fopen(infile, "r+")))
#endif
        {
            fprintf(stderr,"Fatal Error: Unable to find input file <%s>\n",
              infile);
            exit (-1);
        }
    }

    /* process the datablock header */
    nbytes = processHeader(dpi, line);

    if ( (line & ENHANCED) && zapfile )
    {
        for (ndx = 0; fscanf(fz,"%d%d%d%d", &ZapList[ndx].sx, &ZapList[ndx].sy,
          &ZapList[ndx].cx, &ZapList[ndx].cy) == 4  && ndx < MAX_ZAP; ndx++);
        ZapList[ndx].sx = -1;
    }
    else ZapList[0].sx = -1;

    processData(line, binhex, compactIn, nbytes);
    
    exit (0);

}   /* main */
