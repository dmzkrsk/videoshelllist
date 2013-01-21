/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 5.01.0164 */
/* at Tue Jan 18 17:34:43 2005
 */
/* Compiler settings for S:\Documents\Программирование\Development\VideoShellList\VideoShellList.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )
#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

const IID IID_IVSLExt = {0x0F2226A1,0x47FC,0x47F2,{0xB0,0x1D,0xC8,0xDD,0xC2,0xCA,0x19,0x49}};


const IID LIBID_VIDEOSHELLLISTLib = {0x98BB44E8,0x4949,0x4E05,{0x8A,0x65,0xB3,0x90,0xF2,0xD1,0x10,0x3B}};


const CLSID CLSID_VSLExt = {0x3CA443C1,0x5F38,0x45D5,{0x9D,0x7E,0x75,0x42,0xAD,0xF9,0xE6,0x12}};


#ifdef __cplusplus
}
#endif

