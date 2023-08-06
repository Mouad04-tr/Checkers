
#include "glob_variable.h"
#include "macro.h"

#define rowcol 30
#define piece 30

int Nbrowocl=8;
int Nbrpiece=12;

#define __STYLE__ "D:\\GTK\\Checkers\\style.css"

typedef enum {
    CHKS_OWNER_NONE,
    CHKS_OWNER_ONE,//user
    CHKS_OWNER_TWO,//machine
} ChksOwner;