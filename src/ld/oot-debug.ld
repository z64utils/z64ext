/* OoT debug */
ADDRESS_START   = 0x80001B30; /* where code goes in ram */
//ADDRESS_START = 0x8000B280; /* this one overwrites file name list */

Bcopy           = 0x80006F10;
DMARomToRam     = 0x80000BFC;
z_malloc        = 0x8007AEE0;
osSetThreadPri  = 0x80006430;
dec             = 0x800140E0; /* external variable */
g_dmadata       = 0x80016DA0; /* external variable */

z_dma_req1      = 0x80001aa0;
z_dma_req2      = 0x80001a5c;
z_scene_load    = 0x800C0008;
z_scene_init    = 0x800BFF0C;
z_file_load_tuple = 0x800BFE5C;
z_scene_room    = 0x80096FE8;
z_bcopy         = 0x80006F10;
z_bzero         = 0x80004450;
z_object_spawn  = 0x80097c00;
z_object_id_to_slot = 0x8009812c;
z_actor_type_push = 0x80031d1c;
z_actor_type_pop = 0x80031d60;
z_actor_init    = 0x8002d678;
z_malloc        = 0x8007ae90; /* ZeldaArena_Malloc */
z_malloc_r      = 0x8007af3c; /* ZeldaArena_MallocR */
z_free          = 0x8007b0ac;
z_actor_load_ovl = 0x800fc620; /* Overlay_Load */
z_actor_free_ovl = 0x80031e2c;
z_flag_get_roomclear = 0x8002ccdc;
z_matrixf_copy  = 0x800d1ba8;
z_actor_table_init = 0x80038590;
z_actor_spawn_entry = 0x80032628;
z_actor_titlecard_clear = 0x8002CDE4;
z_actor_targetctx_init = 0x8002c0c0;
z_actor_magic_wind_test = 0x8002fa60;
z_actor_destroy = 0x8002d788;
z_player_clear_something = 0x8008EDF0;
z_gameplay_get_camera = 0x800c04a4;
z_gameplay_get_active_camera_id = 0x800c030c;
z_cutscene_script_init = 0x800693D8;
z_if_viewport_init = 0x8008A994;
z_matrix_translate_3f = 0x800D0984;
z_matrix_scale_3f = 0x800D0A8C;
z_matrix_alloc = 0x800D1A88;

osCreateMesgQueue = 0x800061d0;
osRecvMesg = 0x80003280;

func_800F89E8 = 0x800F89E8;
func_8005A444 = 0x8005A444;

g_routetable    = 0x801281C0;
g_objtable      = 0x80127528;
g_scenetable    = 0x80129A10;
g_segtable      = 0x80166FA8;
g_actortable    = 0x801162A0;
g_playerivar    = 0x8012AF90;        /* player ivars in code (actor/0/conf.txt) */
g_playerobjdep  = (g_objtable - 8);  /* player object dependency table */
g_savectx       = 0x8015E660;
g_staticctx     = 0x8015FA90;

gMtxFClear = 0x8012db60;

/* end of scene table (XXX careful, these parentheses are necessary) */
g_scenetable_end = (g_scenetable + (20 * 110)); /* 8012A2A8 */

/* 32 bytes for storing scene entry */
g_sceneentry     = (g_scenetable_end - 32); /* 8012A288 */

/* using free space for this function */
zx_dma_udata_tempP = (g_sceneentry - 8); /* 8012A280 */

/* important offsets
00002730    ROMOFS       where code is injected into rom
                           NOTE change to 0000BE80 to overwrite
                                file name table instead; great
                                for testing functions that don't
                                fit right away
00002148    ROMJAL       location of jal to update in rom
00000000    MMJUMP       unused in OoT
00001FFC    ROMGETFILE   where z_file_load is injected into rom
800013FC    ADDRGETFILE  where z_file_load goes in ram
*/

/* extra compilation flags (one per line)
-DMAJORA=0     DEFINE
-DOOTDEBUG     DEFINE
*/

/* note: how many bytes can safely be overwritten?
816            MAXBYTES */

/* note: how many bytes does original z_file_load occupy?
460            MAXGETFILE */

INCLUDE ./n64.ld

