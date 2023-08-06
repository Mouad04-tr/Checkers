
typedef struct tt
{
    int row;
    int col;
    int diag_not_verify;
    int diag_not_verify2;
    struct tt *suiv;
} cellule;

typedef struct fl
{
    cellule *tete;
    cellule *queue;
} File;

typedef struct cc1
{
    int score;
    int row;
    int col;
    struct cc1 *topleft;
    struct cc1 *topright;
    struct cc1 *suiv;
    struct cc1 *butright;
    struct cc1 *butleft;
} cellule2;

typedef struct
{
    GtkWidget *button;
    struct Chkswindow *view_table;
    ChksOwner owner; // proprietere
    gboolean is_queen;
    int col; // colonne
    int row;
} ChksField;

typedef struct
{
    File *f;
    ChksField *field;
} Glob;

typedef struct
{
    GtkWidget *gr;
    ChksField *filed[rowcol][rowcol];
    ChksField *last; // to know wich button is clicked
} Chkview;

typedef struct
{
    int x; // row
    int y; // column
} Player_piece;

typedef struct
{
    Player_piece Tab[piece]; // 12 piece pour chaque joueur
    int nbr_piece_disp;      // nombre de piece restant pour chaque joueur
} ChkPlayer;

typedef struct Chkswindow
{
    GtkWidget *win;
    int combo1; // depth
    int combo2; // mode
    int combo3; // qui commence le premier

    GtkWidget *header_bar;
    Chkview *table; // table dame*
    struct pl *pileback;
    struct pl *pileforward;
    Glob *g; //
    int depth;
    int matrice[rowcol][rowcol];
    int prp;
    ChkPlayer *user;    // pour indiquer positionnement des pieces du joueur
    ChkPlayer *machine; // pour indiquer positionnement des pieces de la machine
    int round;
} Chkswindow;

typedef struct Ls
{
    int x, y;
    struct Ls *suiv;
} cellule7;

typedef struct LsL
{
    cellule7 *liste;
    struct LsL *suiv;
} CelluleList;

typedef struct
{
    File *best;
    int numfield;
    int score;
} bestpath;

typedef struct ccn
{
    cellule7 *liste;
    int numfield;
    int score;
    struct ccn *suiv;
} cellule5;

typedef struct
{
    ChksField *filed[rowcol][rowcol];
    ChkPlayer *machine;
    ChkPlayer *user;
} etat_window;

typedef struct pl
{
    etat_window *wintmp;
    struct pl *suiv;
} CLpile;
