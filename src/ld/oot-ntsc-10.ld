/* OoT NTSC 1.0 */
ADDRESS_START   = 0x80000F60; /* where code goes in ram */

Bcopy           = 0x80004DC0;
DMARomToRam     = 0x8000085C;
z_malloc        = 0x80066C60;
osSetThreadPri  = 0x80004480;
dec             = 0x80008490; /* external variable */
g_dmadata       = 0x8000B140; /* external variable */

/* important offsets
00001B60    ROMOFS   where code is injected into rom
00001820    ROMJAL   location of jal to update in rom
00000000    MMJUMP   unused in OoT
0000170C    ROMGETFILE   where z_file_load is injected into rom
80000B0C    ADDRGETFILE  where z_file_load goes in ram
*/

/* extra compilation flags (one per line)
-DMAJORA=0     DEFINE
*/

/* note: how many bytes can safely be overwritten?
832            MAXBYTES */

/* note: how many bytes does original z_file_load occupy?
412            MAXGETFILE */

INCLUDE ./n64.ld

