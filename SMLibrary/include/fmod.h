typedef int FMOD_RESULT;

#define FMOD_DEFAULT                    0x00000000
#define FMOD_LOOP_OFF                   0x00000001
#define FMOD_LOOP_NORMAL                0x00000002
#define FMOD_LOOP_BIDI                  0x00000004
#define FMOD_2D                         0x00000008
#define FMOD_3D                         0x00000010
#define FMOD_CREATESTREAM               0x00000080
#define FMOD_CREATESAMPLE               0x00000100
#define FMOD_CREATECOMPRESSEDSAMPLE     0x00000200
#define FMOD_OPENUSER                   0x00000400
#define FMOD_OPENMEMORY                 0x00000800
#define FMOD_OPENMEMORY_POINT           0x10000000
#define FMOD_OPENRAW                    0x00001000
#define FMOD_OPENONLY                   0x00002000
#define FMOD_ACCURATETIME               0x00004000
#define FMOD_MPEGSEARCH                 0x00008000
#define FMOD_NONBLOCKING                0x00010000
#define FMOD_UNIQUE                     0x00020000
#define FMOD_3D_HEADRELATIVE            0x00040000
#define FMOD_3D_WORLDRELATIVE           0x00080000
#define FMOD_3D_INVERSEROLLOFF          0x00100000
#define FMOD_3D_LINEARROLLOFF           0x00200000
#define FMOD_3D_LINEARSQUAREROLLOFF     0x00400000
#define FMOD_3D_INVERSETAPEREDROLLOFF   0x00800000
#define FMOD_3D_CUSTOMROLLOFF           0x04000000
#define FMOD_3D_IGNOREGEOMETRY          0x40000000
#define FMOD_IGNORETAGS                 0x02000000
#define FMOD_LOWMEM                     0x08000000
#define FMOD_VIRTUAL_PLAYFROMSTART      0x80000000