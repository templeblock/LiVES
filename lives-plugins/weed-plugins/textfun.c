// textfun.c
// weed plugin
// (c) G. Finch (salsaman) 2005
//
// released under the GNU GPL 3 or later
// see file COPYING or www.gnu.org for details


#include "../../libweed/weed.h"
#include "../../libweed/weed-effects.h"
#include "../../libweed/weed-plugin.h"

///////////////////////////////////////////////////////////////////

static int num_versions=1; // number of different weed api versions supported
static int api_versions[]={100}; // array of weed api versions supported in plugin, in order of preference (most preferred first)

static int package_version=2; // version of this package

//////////////////////////////////////////////////////////////////

#include "../../libweed/weed-utils.h" // optional
#include "../../libweed/weed-plugin-utils.h" // optional

/////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>

typedef struct {
  char *fontname;
  int width;
  int nglyphs;
  uint16_t *fonttable;
} font_table_t;

/////////////////////////////////////////////

static uint8_t onescount[65536];

void makeonescount(void) {
  int i,j;
  for (i=0;i<65536;i++) {
    onescount[i]=0;
    for (j=0;j<16;j++) {
      if (i&(1<<j)) onescount[i]++;
    }
  }
}

static int get_hex_digit (char *d) {
  char c[2];
  c[0]=*d;
  c[1]=0;

  if (!strcmp (c,"a")||!strcmp (c,"A")) return 10;
  if (!strcmp (c,"b")||!strcmp (c,"B")) return 11;
  if (!strcmp (c,"c")||!strcmp (c,"C")) return 12;
  if (!strcmp (c,"d")||!strcmp (c,"D")) return 13;
  if (!strcmp (c,"e")||!strcmp (c,"E")) return 14;
  if (!strcmp (c,"f")||!strcmp (c,"F")) return 15;
  return(atoi (c));
}


#define NFONTMAPS 3

static char *font_maps[NFONTMAPS];
static font_table_t font_tables[NFONTMAPS];


static void make_font_tables(void) {
  // font_map format is "font_name|glyphbitwidth|bitmap of glyphs"
  // glyph height is always 8 bits

  // this can be expanded by increasing NFONTMAPS

  font_maps[0]="ANSI|8|0000183C3C3C18181800181800000000006666662400000000000000000000000000006C6CFE6C6C6CFE6C6C0000000018187CC6C2C07C060686C67C1818000000000000C2C60C183060C686000000000000386C6C3876DCCCCCCC76000000000030303060000000000000000000000000000C18303030303030180C00000000000030180C0C0C0C0C0C1830000000000000000000663CFF3C66000000000000000000000018187E181800000000000000000000000000000018181830000000000000000000007E0000000000000000000000000000000000001818000000000000000002060C183060C0800000000000007CC6C6CEDEF6E6C6C67C0000000000001838781818181818187E0000000000007CC6060C183060C0C6FE0000000000007CC606063C060606C67C0000000000000C1C3C6CCCFE0C0C0C1E000000000000FEC0C0C0FC060606C67C0000000000003860C0C0FCC6C6C6C67C000000000000FEC606060C18303030300000000000007CC6C6C67CC6C6C6C67C0000000000007CC6C6C67E0606060C78000000000000000018180000001818000000000000000000181800000018183000000000000000060C18306030180C060000000000000000007E00007E000000000000000000006030180C060C1830600000000000007CC6C60C1818180018180000000000007CC6C6C6DEDEDEDCC07C00000000000010386CC6C6FEC6C6C6C6000000000000FC6666667C66666666FC0000000000003C66C2C0C0C0C0C2663C000000000000F86C6666666666666CF8000000000000FE6662687868606266FE000000000000FE6662687868606060F00000000000003C66C2C0C0DEC6C6663A000000000000C6C6C6C6FEC6C6C6C6C60000000000003C18181818181818183C0000000000001E0C0C0C0C0CCCCCCC78000000000000E666666C78786C6666E6000000000000F06060606060606266FE000000000000C3E7FFFFDBC3C3C3C3C3000000000000C6E6F6FEDECEC6C6C6C60000000000007CC6C6C6C6C6C6C6C67C000000000000FC6666667C60606060F00000000000007CC6C6C6C6C6C6D6DE7C0C0E00000000FC6666667C6C666666E60000000000007CC6C660380C06C6C67C000000000000FFDB991818181818183C000000000000C6C6C6C6C6C6C6C6C67C000000000000C3C3C3C3C3C3C3663C18000000000000C3C3C3C3C3DBDBFF6666000000000000C3C3663C18183C66C3C3000000000000C3C3C3663C181818183C000000000000FFC3860C183060C1C3FF0000000000003C30303030303030303C0000000000000080C0E070381C0E06020000000000003C0C0C0C0C0C0C0C0C3C0000000010386CC600000000000000000000000000000000000000000000000000FF0000303018000000000000000000000000000000000000780C7CCCCCCC76000000000000E06060786C666666667C0000000000000000007CC6C0C0C0C67C0000000000001C0C0C3C6CCCCCCCCC760000000000000000007CC6FEC0C0C67C000000000000386C6460F060606060F000000000000000000076CCCCCCCCCC7C0CCC78000000E060606C7666666666E60000000000001818003818181818183C0000000000000606000E06060606060666663C000000E06060666C78786C66E60000000000003818181818181818183C000000000000000000E6FFDBDBDBDBDB000000000000000000DC6666666666660000000000000000007CC6C6C6C6C67C000000000000000000DC66666666667C6060F000000000000076CCCCCCCCCC7C0C0C1E000000000000DC7666606060F00000000000000000007CC660380CC67C000000000000103030FC30303030361C000000000000000000CCCCCCCCCCCC76000000000000000000C3C3C3C3663C18000000000000000000C3C3C3DBDBFF66000000000000000000C3663C183C66C3000000000000000000C6C6C6C6C6C67E060CF8000000000000FECC183060C6FE0000000000000E18181870181818180E00000000000018181818001818181818000000000000701818180E181818187000000000000076DC0000000000000000000000000000000010386CC6C6C6FE0000000000";
  

  // hex encoded font map - Hiragana
  font_maps[1]="Hiragana|16|0000000000000000020002E03F00024002400FF01488250822083C100060018000000100010001F83F000120012007F01A4822444144418443043C08003000C0000000000000000000000000300010101008100810041104120414000C00040000000000000060102010200820082004200420042104120414001800080008000000000000000000040003F0000001F026081804000400040008003000C003000000080007F00000000001F04608380400040004000400040008003000C007000000000000000000040003E0000020401FF00080010003C004400840107C000000001000080007C00000002041F03E40008001800280044008401040203E000000000000000000000000061002083FF4020007E00A101208120812080C10006000000800040804E47F02041E040007F00C081404240444044404380800700000000006000200020847C43C44044408420842085A104610822080270040000000000004040212020804107FC80444044408420842105E1042208026804100000000000100008C00F01F80004700F87F2000200010401040F02000180007F0000000000206011801E01E800047007803C03C200020001041F02010180007E00000000000700020004000800300040008000400020001000080006000100008000000000052004900800100020004000800100010000C00030000C00020001000000000206020102010201347FC40104010401044104810502030201040108000000000104220292024202047FF40204020402048205020502030203040118000000000000010000FF80010002000000000000000000000400020021FFC000000000000000200091FF40020004000000000000000000000400020021FFC00000000000001000080004E30700FC000200010001040C84038200010000C0003F80004000002040112008820FC1FC0004000200020401040F0400020001C0003FC00000000300008000800080008000800080010001002100410081010086007800000000010200910088008000800080008001000100110021004100810300FC00000000000E0002000204FFF302001A0026004200420026001A00040008003000C00000001840052004847FE3840004007C0084008C0074000400040008001000E000000002030100810081008FE7F1008100810081008D0083008000FFC0000000000000014104A08200820082008FE7F200820082009A0086008000C0003FC0000000000F01F20004000800100021E0FE014006400040004000400030000FC0000000000FA1F25004000800100021E0FE014006400040004000400030000FC0000000004000400046047803C7E040008000800080010001200120021FE4000000000000414040A046047803C7E040008000800080010001200120021FE40000000000003000100217C1F800200040005FC0E0208021002100200040008007003800000060A020542F83F00040008000BF81C041004200420040008001000E0070000000000000000000000000003F00C081004000400040008001000600380000000000000000001F80604780200020002000200020004000400180060078000000000000A000501F806047802000200020002000200040004001800600780000000000000003C07C07A000400040008000800080008000400020001F80000000000000000003C07C07A14040A040008000800080008000400020001F8000000000000030001000100010801180160038004000800100010001000080007FC000000000600020A02050210023002C007000800100020002000200010000FF8000000000400040007707C1C04100410081008100810101013F01418242443C2000000002000100011FC1000200040004000400040004C005200610021FE20001000000001000100410043F02D08110431022902450242024374448C388C0072000200000C000400047805847E04440408041804180428F4490C490638F9080008000000000007F01908210441044102410241024102420222021C040008003000C00000106010101010101E27F0201040104010401045F05A1C621261E1200000000000106A10151010101E27F0201040104010401045F05A1C621261E120000000000020C620292026203C4FE040208020802080208BE0B438C424C3C2400000000000010041003E08020804080808100C200C20122010201010200FC0000000000000010A41053E08020804080808100C200C20122010201010200FC0000000000000010641093E0E020804080808100C200C20122010201010200FC000000000000002000100008001400200040002000110008804440842104226404180000000000214010A00800140020004000200011000880444084210422640418000000000020C0112008C0140020004000200011000880444084210422640418000000000000000000300048008401020602000100008000400020001000000000000000000000014030A04800840102060200010000800040002000100000000000000000000000C0312048C0840102060200010000800040002000100000000000000002000107C11902010201E47F040104010401049F85214221221E1200000000000400A207527C04040407C8FC080408040804093F0A448444443824000000000004006207927C64040407C8FC080408040804093E0A458444443824000000000000180008000FE7F800080008000FC3F80008000800FE0109810860F0000000000018022801C8000980108010802080FF0321842144422482130400080010000001800041005887E04040204023C00440444044C0434080408041003E000000000018030801080108017E01918290449024502460245024804301800600180000006000200020022001FE0040044043FF40402040204020402040403F800000000000000000000000000801040087805A446243A040288027001000100010000000100108020F8214422421C027002080409F8040004000200020002000100000000000000000000001180108011F81684188420842084229821E001000600000001802040107811C4124224424842504260424442424421F8108003000400000000000000000000000180008000FC0080008000800F8010E0119C0E0000000000018000800080008600F800800080008000801FC020B0208C21021E0100000000060001E018001000100010F01308140418041004200420040008003007C000040408080808090A0A0C04000000030000007811900E2000400080010003F804040802138264420442044C03F00000000018000400047004903F104410041004100C101411241154120C0C040000000000006011A00E400080010002780784080210022002400200040008003003C0000000000000000000000600040004F01F88060404040C041404140804100460000010000800040004000C78158466020C02140224024402440274040C080470000001C006401880010003E00D18120422024402440244E24912311400F80010000013E00C4000800170078808041304148404F80380081C182224424382000400000100010011380FC0020003000487085870E0034004400840080007FE000000000700010002000200040004000800080017801842104220442044404800300000000000000000000003FC1C080010012001C001000100020002000400080000000000000047FE3804011801E001000100010002000200040008001000000000000000000000000001020C00000000000000180008001000100020004000C0014002400C403040004000400040000000000000000000000000010001001FF01110101010201040004000800100020000000100010001001FF811081008100810081010101000200040008003000C0000000000000000000000000000001FF0010001000100010001003FFC0000000000000000000000000000107C0F8001000100010001000100010043FE3C000000000000000000000000000080004000401FFC00C0014002400440184000C0000000000100008000400040407F3FC0004000C00140024004400840104061C00040000003000100010001FC7F040104010401040104020402080488087010006000000001040112010801003FFC01040104010402040204040408881078200040000000040002000100010047FC39000100010001FF7E8000800080008000800000000004000214010A010047FC39000100010001FF7E8000800080008000800000000001000100010003FC040808081010201040200040008001000600180060000000010A0105010003FC04080808101020104020004000800100060018006000000006000200020004000FFE082010202020404000400080010002000400080000000614020A020004000FFE0820102020204040004000800100020004000800000000000000000020001FF800080008000800080008001000107FFC00000000000000000014000A20001FF800080008000800080008001000107FFC0000000000000060182004200420043E7FE00420042004200420044000400080010002000000006A182504200420043E7FE0042004200420042004400040008001000200000000001000080004002002100408080010002000400080110012000C000000000000501028080004002002100408080010002000400080110012000C00000000000000107C0F840008000800100020004000E00110020804041802600000000000000A20F51F100010002000200040008001C00220041008083004C00000000000000008000400040004FC0F087410042004C004000400040007FE0000000000000004080A0404040004FC0F087410042004C004000400040007FE00000000000000000008100808080408041004100020002000400080010002000C0000000000000A0005201010100810082008200040004000800100020004001800000000000100010001FC020404080C101220212040C00080014002000400180060000000020A020503F804080810182024404240818001000280040008003000C00000000000001803E03C800080008003FE7C80008001000100020004001800000000000000003007C279050102010007FCF9000100020002000400080030000000000000000000000000000200110808880488041000100020004000800100060000000000020E21021102108408840808000800100020004000800100060018000000000A0405421822082108111010100020002000400080010002000C003000000010180FE0000000004FFE3080008000800080010001000200040008003000000020301FCA000500009FFC61000100010001000200020004000800100060000003010101010101010101010101010100000314010A01000100010001E00118010401000100010001000100010001000000018000800080008040FE3F80008000800080010001000200040018000000000000000000000000001FF80000000000000000000043FE3C000000000000000000000000001FFC0008000800100610012000C000A001100208040418026000000004000200010000F03F200040008001E003180D0471020102010001000100000000000018000400040008000800100010002000400080010002000C003000000000000040064002200210021004080408080408041002100220024000000000000014004A06400220021002100408040808040804100210022002400000000000000C0052064C02200210021004080408080408041002100220024000000000000000300010001000100C107013801C00100010001000100010041FFC0000000000003014100A1000100C107013801C00100010001000100010041FFC00000000000C3012100C1000100C107013801C00100010001000100010041FFC000000000000000023FC1C04000400080008001000200040008001000200040018000000000A000547F838080008001000100020004000800100020004000800300000000006000947FE3808000800100010002000400080010002000400080030000000000000000200050004800840102060100008000400020002000000000000000000000004001203080480084010206010000800040002000200000000000000000000000C0012030C04800840102060100008000400020002000000000000000003000100010041FE3F00012009100908110811042104410005000300000000000314010A010041FE3F0001200910090811081104210441000500030000000000030C0112010C41FE3F000120091009081108110421044100050003000000000000000000003E43C23C04000800101820064001800080004000400000000000001E0001C00030000C1C0003800060001800041C00030000C00030000800000000020001000100010002000200044004200810100810F82704780200020002000000780008000800100110009000600020005000880104020004001800000000000000000001F81F0001000100010007FE790001000100010001FF0000000000000000000000000000040002000278078879100120014001000100010000000000040004000200023E03C24F043108011001200140008000800080008000000000000000000000000000000000000000000FE000200020002000203FFC000000000000000000000000000000001FF00010001000100020002000207FFE000000000000000000000000000000000FF8000800080FF80010001000101FF0000000000000000000001FFC00040004000400041FF800080008000800087FF8000000000FF800000000000043FC3C0400040008001000200040018006000800100000000020101008100810081008100810081008100810001000200040018006000000030019000900090009000900090109020904090811101120214041800000000000000000100010001001100210041008101010201040118016001800000000000000000000001FF81008100810081008100810081FF810001000000000000000000000000000000010001FF01010101010200020004000400080010002000000000000001FF810081008100810081010001000200020004000800100020000000080008000803FFC08800880088008807FFF008000800080008000800080000000000000000001F83E1000600180010001000100010047FE3800000000000000000000003FF80008000800081FF800080010001000200040008003000C00000000000000300008040404040800080010002000400080010002003C00000000000214010A010001001FF81008100810101010102000400080010002000400000000000000000000000100010001F01F1001100110021002100410086010000000000000000000000000000400040004780FA0104020400080010002000400";

  font_maps[2]="diamond|16|0000000000000000018003C007E00FF01FF83FFC7FFE3FFC1FF80FF007E003C001800000";

 int i,j,k;
 int nglyphs;
 size_t len;

  for (k=0;k<NFONTMAPS;k++) {
    len=strcspn(font_maps[k],"|");
    font_tables[k].fontname=malloc(len+1);
    weed_memcpy(font_tables[k].fontname,font_maps[k],len);
    weed_memset(font_tables[k].fontname+len,0,1);
    font_maps[k]+=len+1;
    font_tables[k].width=atoi(font_maps[k]);
    len=strcspn(font_maps[k],"|");
    font_maps[k]+=len+1;
    nglyphs=strlen(font_maps[k])/4/font_tables[k].width;
    font_tables[k].nglyphs=++nglyphs;
    // we can use malloc here since we are called from setup
    font_tables[k].fonttable=malloc(32*nglyphs);

    for (i=0;i<nglyphs*16;i+=16) {
      for (j=0;j<16;j++) {
	if (i==0) font_tables[k].fonttable[j]=0; // make sure we have a space
	else {
	  if (font_tables[k].width==16) {
	    font_tables[k].fonttable[i+j]=((get_hex_digit(&font_maps[k][(i-16+j)*4])<<12)+(get_hex_digit(&font_maps[k][(i-16+j)*4+1])<<8)+(get_hex_digit(&font_maps[k][(i-16+j)*4+2])<<4)+(get_hex_digit(&font_maps[k][(i-16+j)*4+3])));
	  }
	  else {
	    font_tables[k].fonttable[i+j]=((get_hex_digit(&font_maps[k][(i-16+j)*2])<<4)+(get_hex_digit(&font_maps[k][(i-16+j)*2+1])));
	  }
	}
      }
    }
  }
}


/* precomputed tables */
#define FP_BITS 16

static int Y_R[256];
static int Y_G[256];
static int Y_B[256];

static int Cb_R[256];
static int Cb_G[256];
static int Cb_B[256];
static int Cr_R[256];
static int Cr_G[256];
static int Cr_B[256];

static int myround(double n)
{
  if (n >= 0) 
    return (int)(n + 0.5);
  else
    return (int)(n - 0.5);
}



static void init_RGB_to_YCbCr_tables(void)
{
  int i;

  /*
   * Q_Z[i] =   (coefficient * i
   *             * (Q-excursion) / (Z-excursion) * fixed-pogint-factor)
   *
   * to one of each, add the following:
   *             + (fixed-pogint-factor / 2)         --- for rounding later
   *             + (Q-offset * fixed-pogint-factor)  --- to add the offset
   *             
   */
  for (i = 0; i < 256; i++) {
    Y_R[i] = myround(0.2100 * (double)i 
		     * 219.0 / 255.0 * (double)(1<<FP_BITS));
    Y_G[i] = myround(0.587 * (double)i 
		     * 219.0 / 255.0 * (double)(1<<FP_BITS));
    Y_B[i] = myround((0.114 * (double)i 
		      * 219.0 / 255.0 * (double)(1<<FP_BITS))
		     + (double)(1<<(FP_BITS-1))
		     + (16.0 * (double)(1<<FP_BITS)));

    Cb_R[i] = myround(-0.168736 * (double)i 
		      * 224.0 / 255.0 * (double)(1<<FP_BITS));
    Cb_G[i] = myround(-0.331264 * (double)i 
		      * 224.0 / 255.0 * (double)(1<<FP_BITS));
    Cb_B[i] = myround((0.500 * (double)i 
		       * 224.0 / 255.0 * (double)(1<<FP_BITS))
		      + (double)(1<<(FP_BITS-1))
		      + (128.0 * (double)(1<<FP_BITS)));

    Cr_R[i] = myround(0.500 * (double)i 
		      * 224.0 / 255.0 * (double)(1<<FP_BITS));
    Cr_G[i] = myround(-0.418688 * (double)i 
		      * 224.0 / 255.0 * (double)(1<<FP_BITS));
    Cr_B[i] = myround((-0.081312 * (double)i 
		       * 224.0 / 255.0 * (double)(1<<FP_BITS))
		      + (double)(1<<(FP_BITS-1))
		      + (128.0 * (double)(1<<FP_BITS)));
  }
}


static inline uint8_t 
calc_luma (uint8_t *pixel) {
  return (Y_R[pixel[0]] + Y_G[pixel[1]]+ Y_B[pixel[2]]) >> FP_BITS;
}


static inline uint8_t make_lumbyte (unsigned char *img, uint8_t thresh) {
  // here we map 8 pixels to one byte; if the luma of a pixel is > THRESH we set a 1, otherwise a 0
  uint8_t lumbyte=0;
  register int i;

  for (i=7;i>=0;i--) {
    if (calc_luma(img)>thresh) lumbyte|=(1<<i);
    img+=3;
  }
  return lumbyte;
}


static inline void fill_line(int fontwidth, unsigned char *src,unsigned char *dst, unsigned char *fillval, unsigned short fontrow, int type) {
  register int i;

  for (i=fontwidth-1;i>=0;i--) {
    switch (type) {
    case 0:
      // coloured pixels
      if (fontrow&(1<<i)) weed_memcpy(dst,src,3);
      else weed_memset(dst,0,3);
      src+=3;
      break;
    case 1:
      // monochrome
      if (fontrow&(1<<i)) weed_memset(dst,255,3);
      else weed_memset(dst,0,3);
      break;
    case 2:
      // greyscale
    case 3:
      // colour blocks
      if (fontrow&(1<<i)) weed_memcpy(dst,fillval,3);
      else weed_memset(dst,0,3);
      break;
    }
    dst+=3;
  }
}


static inline void fill_block(int fontnum, unsigned char *src, unsigned char *dst, int drow, int irow, int glyph, int type) {
  // we will fill a 8x16 block of pixels in dst with a map of a character
  register int i;
  unsigned char fillval[3];

  if (type==2) weed_memset(fillval,(src[0]+src[1]+src[2])/3,3);
  else if (type==3) weed_memcpy(fillval,src,3);

  for (i=0;i<16;i++) {
    fill_line(font_tables[fontnum].width,src,dst,fillval,font_tables[fontnum].fonttable[glyph*16+i],type);
    dst+=drow;
    if (type==0) src+=irow;
  }
}

/////////////////////////////////////////////////////////////


int textfun_process (weed_plant_t *inst, weed_timecode_t timestamp) {
  int error;
  unsigned int width3,irow16,orow16;
  unsigned int startx,starty,endx;
  register int j,k,l,m;
  int minones,numones,minchar=0;
  int mode;
  uint8_t thresh;
  int fontnum;
  weed_plant_t *in_channel=weed_get_plantptr_value(inst,"in_channels",&error),*out_channel=weed_get_plantptr_value(inst,"out_channels",&error);
  unsigned char *src=weed_get_voidptr_value(in_channel,"pixel_data",&error);
  unsigned char *dst=weed_get_voidptr_value(out_channel,"pixel_data",&error);
  int width=weed_get_int_value(in_channel,"width",&error);
  int height=weed_get_int_value(in_channel,"height",&error);
  int irowstride=weed_get_int_value(in_channel,"rowstrides",&error);
  int orowstride=weed_get_int_value(out_channel,"rowstrides",&error);
  weed_plant_t **in_params=weed_get_plantptr_array(inst,"in_parameters",&error);
  unsigned char *end;
  int skip,glyph16;

  uint8_t lb[16],lb2[16];

  thresh=weed_get_int_value(in_params[0],"value",&error)&0xff;
  mode=weed_get_int_value(in_params[1],"value",&error);
  fontnum=weed_get_int_value(in_params[2],"value",&error);

  weed_free(in_params); // must weed free because we got an array

  width=(width>>4)<<4;

  width3=width*3;
  irow16=irowstride<<4;
  orow16=orowstride<<4;

  if (font_tables[fontnum].width==8) startx=((width-((width>>3)<<3))>>1)*3;
  else startx=((width-((width>>4)<<4))>>1)*3;
  endx=width3-startx;

  starty=((height-((height>>4)<<4))>>1);
  end=src+(height-15-starty)*irowstride;

  src+=starty*irowstride;
  dst+=starty*orowstride;

  skip=font_tables[fontnum].width*3;
  glyph16=font_tables[fontnum].nglyphs<<4;

  //printf("ok2 %d %d %d\n",startx,endx,skip);


  // get a FONT_WIDTH x 16 block, compare with each character and find the minimum difference
  for (;src<end;src+=irow16) {
    for (j=startx;j<endx;j+=skip) {
      minones=256;

      for (l=0;l<16;l++) lb[l]=make_lumbyte(&src[j+l*irowstride],thresh);
      if (font_tables[fontnum].width==16) for (l=0;l<16;l++) lb2[l]=make_lumbyte(&src[j+l*irowstride+24],thresh);

      for (k=0;k<glyph16;k+=16) {
	numones=m=0;
	// xor'ing our lumbyte with a font row, and then looking this up in onescount gives us the number of different bits
	if (font_tables[fontnum].width==16) {
	  for (l=0;l<16;l++) if ((numones+=onescount[(uint16_t)(((uint16_t)lb[l]<<8)+(uint16_t)lb2[l])^font_tables[fontnum].fonttable[k+(m++)]])>=minones) break;
	}
	else {
	  for (l=0;l<16;l++) if ((numones+=onescount[(lb[l]^(uint8_t)(font_tables[fontnum].fonttable[k+(m++)]))])>=minones) break;
	}
	if (numones<minones) {
	  minones=numones;
	  minchar=k>>4;
	}
      }
      // fill 8x16 block in dest with char
      fill_block(fontnum,&src[j],&dst[j],orowstride,irowstride,minchar,mode);
    }
    dst+=orow16;
  }
  return WEED_NO_ERROR;
}




weed_plant_t *weed_setup (weed_bootstrap_f weed_boot) {
  weed_plant_t *plugin_info=weed_plugin_info_init(weed_boot,num_versions,api_versions);

  if (plugin_info!=NULL) {
    char *modes[]={"colour pixels","monochrome","greyscale","solid colours",NULL};
    int palette_list[]={WEED_PALETTE_BGR24,WEED_PALETTE_RGB24,WEED_PALETTE_END};
    weed_plant_t *in_chantmpls[]={weed_channel_template_init("in channel 0",0,palette_list),NULL};
    weed_plant_t *out_chantmpls[]={weed_channel_template_init("out channel 0",0,palette_list),NULL};
    weed_plant_t *in_params[4];
    weed_plant_t *filter_class;

    char *fonts[NFONTMAPS+1];
    int i;

    make_font_tables();

    for (i=0;i<NFONTMAPS;i++) {
      fonts[i]=font_tables[i].fontname;
    }
    fonts[i]=NULL;

    in_params[0]=weed_integer_init("threshold","Pixel _threshold",128,0,255);
    in_params[1]=weed_string_list_init("mode","Colour _mode",0,modes);
    in_params[2]=weed_string_list_init("font","_Font",0,fonts);
    in_params[3]=NULL;
    
    filter_class=weed_filter_class_init("textfun","salsaman",2,0,NULL,&textfun_process,NULL,in_chantmpls,out_chantmpls,in_params,NULL);

    weed_plugin_info_add_filter_class (plugin_info,filter_class);
    
    weed_set_int_value(plugin_info,"version",package_version);

    init_RGB_to_YCbCr_tables();

    makeonescount();
  }

  return plugin_info;
}



