/* MM (U) */
ADDRESS_START = 0x80080E30; /* where code goes in ram */

Bcopy           = 0x800912C0;
DMARomToRam     = 0x80080790;
z_malloc        = 0x80066C60; /* TODO */
osSetThreadPri  = 0x8008FC00; /* TODO confirm working */
dec             = 0x8009BA10; /* external variable */
g_dmadata       = 0x8009F8B0; /* external variable */

/* important offsets
00001E30    ROMOFS   where code is injected into rom
00001B48    ROMJAL   location of jal to update in rom
00002178    MMJUMP   see makefile for details
00001A08    ROMGETFILE   where z_file_load is injected into rom
80080A08    ADDRGETFILE  where z_file_load goes in ram
*/

/* extra compilation flags (one per line)
-DMAJORA=1     DEFINE
*/

/* note: how many bytes can safely be overwritten?
840            MAXBYTES */

/* note: how many bytes does original z_file_load occupy?
380            MAXGETFILE */

INCLUDE ./n64.ld

