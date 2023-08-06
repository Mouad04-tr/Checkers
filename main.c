#include <stdio.h>
#include <gtk/gtk.h>
#include <time.h>
#include "glob_variable.h"
#include "macro.h"

#define rowcol 30
#define piece 30

int Nbrowocl=8;
int Nbrpiece=12;
//10 is 20

#define __STYLE__ "D:\\GTK\\Checkers\\style.css"

typedef enum {
    CHKS_OWNER_NONE,
    CHKS_OWNER_ONE,//user
    CHKS_OWNER_TWO,//machine
} ChksOwner;

typedef struct tt
{
    int row;
    int col;
    int diag_not_verify;
    int diag_not_verify2;
    struct tt *suiv;
}cellule;

typedef struct fl
{
    cellule *tete;
    cellule *queue;
}File;

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
}cellule2;

typedef struct
{
    GtkWidget *button;
    struct Chkswindow *view_table;
    ChksOwner owner;//proprietere
    gboolean is_queen;
    int col;//colonne
    int row;
}ChksField;

typedef struct
{
    File *f;
    ChksField *field;
}Glob;

typedef struct
{
    GtkWidget *gr;
    ChksField *filed[rowcol][rowcol];
    ChksField *last;//to know wich button is clicked
}Chkview;

typedef struct
{
    int x;//row
    int y;//column
}Player_piece;

typedef struct
{
    Player_piece Tab[piece];//12 piece pour chaque joueur
    int nbr_piece_disp;//nombre de piece restant pour chaque joueur
}ChkPlayer;

typedef struct Chkswindow
{
    GtkWidget *win;
    int combo1;//depth
    int combo2;//mode
    int combo3;//qui commence le premier
    GtkWidget *labelTwo;
    GtkWidget *labelOne;
    GtkWidget *header_bar;
    Chkview *table;//table dame*
    struct pl *pileback;
    struct pl *pileforward;
    Glob *g;//
    int depth;
    int matrice[rowcol][rowcol];
    int prp;
    ChkPlayer *user;//pour indiquer positionnement des pieces du joueur
    ChkPlayer *machine;//pour indiquer positionnement des pieces de la machine
    int round;
    int pause;
}Chkswindow;

typedef struct Ls
{
    int x, y;
    struct Ls *suiv;
}cellule7;

typedef struct LsL
{
    cellule7 *liste;
    struct LsL *suiv;
} CelluleList;

typedef struct {
    File *best;
    int numfield;
    int score;
}bestpath;

typedef struct ccn
{
    cellule7 *liste;
    int numfield;
    int score;
    struct ccn *suiv;
}cellule5;

typedef struct {
    ChksField *filed[rowcol][rowcol];
    ChkPlayer *machine;
    ChkPlayer *user;
}etat_window;

typedef struct pl
{
    etat_window *wintmp;
    struct pl *suiv;
}CLpile;


void init_player(ChkPlayer *player,ChksOwner owner);
GtkWidget *Make_headerbar(Chkswindow *win);

void chks_field_set_owner(ChksField *field)
{
    GtkCssProvider* provider;
    provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(GTK_CSS_PROVIDER(provider),__STYLE__,NULL);
    GtkStyleContext *context;
    context = gtk_widget_get_style_context(field->button);
    switch (field->owner) {
        case CHKS_OWNER_NONE:
            gtk_style_context_remove_class(context, "chks-player-one");
            gtk_style_context_remove_class(context, "chks-player-two");
            gtk_style_context_remove_class(context, "chks-player-one-quenn");
            gtk_style_context_remove_class(context, "chks-player-two-quenn");
            break;
        case CHKS_OWNER_ONE:
            gtk_style_context_remove_class(context, "chks-player-one-pre");
            gtk_style_context_remove_class(context, "chks-player-one");
            gtk_style_context_remove_class(context, "chks-player-two");
            gtk_style_context_remove_class(context, "chks-player-one-quenn");
            gtk_style_context_remove_class(context, "chks-player-two-quenn");
            if(field->is_queen)gtk_style_context_add_class(context, "chks-player-one-quenn");
            else gtk_style_context_add_class(context, "chks-player-one");
            break;
        case CHKS_OWNER_TWO:
            gtk_style_context_remove_class(context, "chks-player-two-pre");
            gtk_style_context_remove_class(context, "chks-player-one");
            gtk_style_context_remove_class(context, "chks-player-two");
            gtk_style_context_remove_class(context, "chks-player-one-quenn");
            gtk_style_context_remove_class(context, "chks-player-two-quenn");
            if(field->is_queen)gtk_style_context_add_class(context, "chks-player-two-quenn");
            else gtk_style_context_add_class(context, "chks-player-two");
            break;
    }
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

void apply_style_button(ChksField *field[rowcol][rowcol])
{
    int i,j;
    for(i=0;i<Nbrowocl;i++)
        for(j=0;j<Nbrowocl;j++)
            chks_field_set_owner(field[i][j]);
}

int delete_value_ChkPlayer(ChkPlayer *chpl,int row ,int col)
{
    for(int i=0;chpl->nbr_piece_disp;i++)
    {
        if(chpl->Tab[i].x==row && chpl->Tab[i].y==col)
        {
            for(int j=i;j<chpl->nbr_piece_disp;j++)
            {
                chpl->Tab[j].x=chpl->Tab[j+1].x;
                chpl->Tab[j].y=chpl->Tab[j+1].y;
            }
            chpl->nbr_piece_disp--;
            return (int)1;
        }
    }
    return (int)0;
}

ChkPlayer *creer_player()
{
    ChkPlayer *pl=(ChkPlayer*) malloc(sizeof (ChkPlayer));
    if(!pl)
    {
        printf("error allocation");
        exit(-1);
    }
    pl->nbr_piece_disp=0;
    return (ChkPlayer*)pl;
}

Chkview *creer_table()
{
    Chkview *tb= malloc(sizeof (Chkview));
    if(!tb)
    {
        printf("error allocation");
        exit(-1);
    }
    return (Chkview*)tb;
}
File * creer_initaliser_file()
{
    File *f=(File*) malloc(sizeof (File));
    if(!f)
    {
        printf("error allocation");
        exit(-1);
    }
    f->tete=NULL;
    f->queue=NULL;
    return (File*)f;
}


cellule2 *crrer_initailiser_ccellule2(int score,int row,int col)
{
    cellule2  *ne=(cellule2*)malloc(sizeof (cellule2));
    if(!ne)
    {
        printf("error allocation");
        exit(-1);
    }
    ne->score=score;
    ne->row=row;
    ne->col=col;
    ne->suiv=NULL;
    ne->butleft=NULL;
    ne->butright=NULL;
    ne->topleft=NULL;
    ne->topright=NULL;
    return (cellule2*)ne;
}

int est_vide_file(File *f)
{
    if(!f)return (int)-1;
    if(f->tete==NULL)return (int)1;//File vide;
    else return (int)0;//file not vide
}

int enfiler(File *f,cellule *ne)
{
    if(!f)return (int)-1;
    if(!f->tete)
    {
        f->tete=ne;
        f->queue=ne;
        return (int)1;
    }
    f->queue->suiv=ne;
    f->queue=ne;
    return (int)1;
}


int defiler(File *f)
{
    cellule *q;
    if(!f)return (int)-1;
    if(f->tete==f->queue)
    {
        f->tete=NULL;
        f->queue=NULL;
        return (int)1;
    }
    q=f->tete;
    f->tete=f->tete->suiv;
    free(q);
    return (int)1;
}

//row et col sont les coordonnés de last
int verify_for_not_queen(ChksField *filed,ChksField *last,int row,int col,File *f)
{
    int a,b,c,d,droite=0,gauche=0;
    if(f)
    {
        if(f->tete->row==row && f->tete->col==col)defiler(f);
    }
    if(last->view_table->table->filed[row-1][col-1]->row==filed->row &&
       last->view_table->table->filed[row-1][col-1]->col==filed->col)
        return (int)1;
    if(row+1<=Nbrowocl && col+1<=Nbrowocl)
    {
        if(last->view_table->table->filed[row+1-1][col+1-1]->owner==CHKS_OWNER_TWO)//right
        {
            if(row+2<=Nbrowocl && col+2<=Nbrowocl)
            {
                if(last->view_table->table->filed[row+2-1][col+2-1]->owner==CHKS_OWNER_NONE) {
                    a = last->view_table->table->filed[row + 2 - 1][col + 2 - 1]->row;
                    b = last->view_table->table->filed[row + 2 - 1][col + 2 - 1]->col;
                    droite = verify_for_not_queen(filed, last, a, b, f);
                }
            }
        }
    }
    if(row+1<=Nbrowocl && col-1>=1)
    {
        if(last->view_table->table->filed[row+1-1][col-1-1]->owner==CHKS_OWNER_TWO)//left
        {
            if(row+2<=Nbrowocl && col-2>=1)
            {
                if(last->view_table->table->filed[row+2-1][col-2-1]->owner==CHKS_OWNER_NONE)
                {
                    a=last->view_table->table->filed[row+2-1][col-2-1]->row;
                    b=last->view_table->table->filed[row+2-1][col-2-1]->col;
                    gauche=verify_for_not_queen(filed,last,a,b,f);
                }
            }
        }
    }
    if(droite || gauche)return (int)1;
    else return (int)0;
}

int verify_for_not_queen_two(ChksField *filed,ChksField *last,int row,int col,File *f)
{
    int a,b,c,d,droite=0,gauche=0;
    if(f)
    {
        if(f->tete->row==row && f->tete->col==col)defiler(f);
    }
    if(last->view_table->table->filed[row-1][col-1]->row==filed->row &&
       last->view_table->table->filed[row-1][col-1]->col==filed->col)
        return (int)1;
    if(row-1>=1 && col+1<=Nbrowocl)
    {
        if(last->view_table->table->filed[row-1-1][col+1-1]->owner==CHKS_OWNER_ONE)//right
        {
            if(row-2>=1 && col+2<=Nbrowocl)
            {
                if(last->view_table->table->filed[row-2-1][col+2-1]->owner==CHKS_OWNER_NONE) {
                    a = last->view_table->table->filed[row - 2 - 1][col + 2 - 1]->row;
                    b = last->view_table->table->filed[row - 2 - 1][col + 2 - 1]->col;
                    droite = verify_for_not_queen_two(filed, last, a, b, f);
                }
            }
        }
    }
    if(row-1>=1 && col-1>=1)
    {
        if(last->view_table->table->filed[row-1-1][col-1-1]->owner==CHKS_OWNER_ONE)//left
        {
            if(row-2>=1 && col-2>=1)
            {
                if(last->view_table->table->filed[row-2-1][col-2-1]->owner==CHKS_OWNER_NONE)
                {
                    a=last->view_table->table->filed[row-2-1][col-2-1]->row;
                    b=last->view_table->table->filed[row-2-1][col-2-1]->col;
                    gauche=verify_for_not_queen_two(filed,last,a,b,f);
                }
            }
        }
    }
    if(droite || gauche)return (int)1;
    else return (int)0;
}

int first_is_one_four(ChksField *last,ChksOwner owner)
{
    int a,b;
    a=last->row;
    b=last->col;
    if((a+1<=Nbrowocl) && (a-1>=1) && (b+1<=Nbrowocl) && (b-1>=1))
    {
        if((last->view_table->table->filed[a+1-1][b+1-1]->owner==owner) &&
           (last->view_table->table->filed[a+1-1][b-1-1]->owner==owner) &&
           (last->view_table->table->filed[a-1-1][b-1-1]->owner==owner) &&
           (last->view_table->table->filed[a-1-1][b+1-1]->owner==owner ))return (int)1;
        else return (int)0;
    }else  return (int)0;

}

int first_is_one(ChksField *last,int i,ChksOwner owner)
{
    int a,b;
    a=last->row;
    b=last->col;
    switch (i) {
        case 1:
            if(a-1>=1 && b+1<=Nbrowocl)
            {
                if(last->view_table->table->filed[a-1-1][b+1-1]->owner==owner)return (int)1;
                else return (int)0;
            }else return (int)0;
        case 2:
            if(a-1>=1 && b-1>=1)
            {
                if(last->view_table->table->filed[a-1-1][b-1-1]->owner==owner)return (int)1;
                else return (int)0;
            }else return (int)0;
        case 3:
            if(a+1<=8 && b-1>=1)
            {
                if(last->view_table->table->filed[a+1-1][b-1-1]->owner==owner)return (int)1;
                else return (int)0;
            }else return (int)0;
        case 4:
            if(a+1<=Nbrowocl && b+1<=Nbrowocl)
            {
                if(last->view_table->table->filed[a+1-1][b+1-1]->owner==owner)return (int)1;
                else return (int)0;
            }else return (int)0;
    }
    return (int)0;
}

void first_one_or_two(ChksField *last,int *a,int *b,int i,ChksOwner owner,int et,etat_window *etw,int row,int col)
{
    int k,j;
    if(last)
    {
        k=last->row;
        j=last->col;
    }else
    {
        k=row;
        j=col;
    }
    switch (i) {
        case 1:
            k--;
            j++;
            while (k>=1 && j<=Nbrowocl)
            {
                if(last)
                {
                    if(last->view_table->table->filed[k-1][j-1]->owner==owner)
                    {
                        *a=last->view_table->table->filed[k-1][j-1]->row;
                        *b=last->view_table->table->filed[k-1][j-1]->col;
                        return;
                    }else if(last->view_table->table->filed[k-1][j-1]->owner==
                             CHKS_OWNER_NONE)
                    {
                        k--;
                        j++;
                        continue;
                    }else break;
                }else
                {
                    if(etw->filed[k-1][j-1]->owner==owner)
                    {
                        *a=etw->filed[k-1][j-1]->row;
                        *b=etw->filed[k-1][j-1]->col;
                        return;
                    }else if(etw->filed[k-1][j-1]->owner==
                             CHKS_OWNER_NONE)
                    {
                        k--;
                        j++;
                        continue;
                    }else break;
                }
            }
            break;
        case 2:
            k--;
            j--;
            while (k>=1 && j>=1)
            {
                if(last)
                {
                    if(last->view_table->table->filed[k-1][j-1]->owner==owner)
                    {
                        *a=last->view_table->table->filed[k-1][j-1]->row;
                        *b=last->view_table->table->filed[k-1][j-1]->col;
                        return;
                    }else if(last->view_table->table->filed[k-1][j-1]->owner==
                             CHKS_OWNER_NONE)
                    {
                        k--;
                        j--;
                        continue;
                    }else break;
                }else
                {
                    if(etw->filed[k-1][j-1]->owner==owner)
                    {
                        *a=etw->filed[k-1][j-1]->row;
                        *b=etw->filed[k-1][j-1]->col;
                        return;
                    }else if(etw->filed[k-1][j-1]->owner==
                             CHKS_OWNER_NONE)
                    {
                        k--;
                        j--;
                        continue;
                    }else break;
                }
            }
            break;
        case 3:
            k++;
            j--;
            while (k<=Nbrowocl && j>=1)
            {
                if(last)
                {
                    if(last->view_table->table->filed[k-1][j-1]->owner==owner)
                    {
                        *a=last->view_table->table->filed[k-1][j-1]->row;
                        *b=last->view_table->table->filed[k-1][j-1]->col;
                        return;
                    }else if(last->view_table->table->filed[k-1][j-1]->owner==
                             CHKS_OWNER_NONE)
                    {
                        k++;
                        j--;
                        continue;
                    }else break;
                }else{
                    if(etw->filed[k-1][j-1]->owner==owner)
                    {
                        *a=etw->filed[k-1][j-1]->row;
                        *b=etw->filed[k-1][j-1]->col;
                        return;
                    }else if(etw->filed[k-1][j-1]->owner==
                             CHKS_OWNER_NONE)
                    {
                        k++;
                        j--;
                        continue;
                    }else break;
                }
            }
            break;
        case 4:
            k++;
            j++;
            while (k<=Nbrowocl && j<=Nbrowocl)
            {
                if(last)
                {
                    if(last->view_table->table->filed[k-1][j-1]->owner==owner)
                    {
                        *a=last->view_table->table->filed[k-1][j-1]->row;
                        *b=last->view_table->table->filed[k-1][j-1]->col;
                        return;
                    }else if(last->view_table->table->filed[k-1][j-1]->owner==
                             CHKS_OWNER_NONE)
                    {
                        k++;
                        j++;
                        continue;
                    }else break;
                }else
                {
                    if(etw->filed[k-1][j-1]->owner==owner)
                    {
                        *a=etw->filed[k-1][j-1]->row;
                        *b=etw->filed[k-1][j-1]->col;
                        return;
                    }else if(etw->filed[k-1][j-1]->owner==
                             CHKS_OWNER_NONE)
                    {
                        k++;
                        j++;
                        continue;
                    }else break;
                }
            }
            break;
    }
    *a=-1;
    *b=-1;

}

cellule *creer_initialiser_cellule(int a,int b,int c,int c1)
{
    cellule *ne=(cellule*)malloc(sizeof (cellule));
    if(!ne)
    {
        printf("error allocation");
        exit(-1);
    }
    ne->row=a;
    ne->col=b;
    ne->diag_not_verify=c;
    ne->diag_not_verify2=c1;
    ne->suiv=NULL;
    return (cellule*)ne;
}

int two_consecutif(int row,int col ,ChksField *last,int i,ChksOwner owner)
{
    switch (i) {
        case 1:
            if(last->view_table->table->filed[row-1][col-1]->owner==owner)
            {
                if(((row-1)>=1 )&& ((col+1)<=Nbrowocl))
                {
                    if(last->view_table->table->filed[row-1-1][col+1-1]->owner==owner)return (int)1;
                    else return (int)0;
                }else return (int)0;

            }else return (int)0;
            break;
        case 2:
            if(last->view_table->table->filed[row-1][col-1]->owner==owner)
            {
                if(((row-1)>=1 )&& ((col-1)>=1))
                {
                    if(last->view_table->table->filed[row-1-1][col-1-1]->owner==owner)return (int)1;
                    else return (int)0;
                }else return (int)0;

            }else return (int)0;
            break;
        case 3:
            if(last->view_table->table->filed[row-1][col-1]->owner==owner)
            {
                if(((row+1)<=Nbrowocl)&& ((col-1)>=1))
                {
                    if(last->view_table->table->filed[row+1-1][col-1-1]->owner==owner)return (int)1;
                    else return (int)0;
                }else return (int)0;
            }else return (int)0;
            break;
        case 4:
            if(last->view_table->table->filed[row-1][col-1]->owner==owner)
            {
                if(((row+1)<=Nbrowocl)&& ((col+1)<=Nbrowocl))
                {
                    if(last->view_table->table->filed[row+1-1][col+1-1]->owner==owner)return (int)1;
                    else return (int)0;
                }else return (int)0;
            }else return (int)0;
            break;
    }
}

int two_consecutif2(int row,int col ,etat_window *etw,int i,ChksOwner owner)
{
    switch (i) {
        case 1:
            if(etw->filed[row-1][col-1]->owner==owner)
            {
                if(((row-1)>=1 )&& ((col+1)<=Nbrowocl))
                {
                    if(etw->filed[row-1-1][col+1-1]->owner==owner)return (int)1;
                    else return (int)0;
                }else return (int)0;
            }else return (int)0;
            break;
        case 2:
            if(etw->filed[row-1][col-1]->owner==owner)
            {
                if(((row-1)>=1 )&& ((col-1)>=1))
                {
                    if(etw->filed[row-1-1][col-1-1]->owner==owner)return (int)1;
                    else return (int)0;
                }else return (int)0;

            }else return (int)0;
            break;
        case 3:
            if(etw->filed[row-1][col-1]->owner==owner)
            {
                if(((row+1)<=Nbrowocl)&& ((col-1)>=1))
                {
                    if(etw->filed[row+1-1][col-1-1]->owner==owner)return (int)1;
                    else return (int)0;
                }else return (int)0;
            }else return (int)0;
            break;
        case 4:
            if(etw->filed[row-1][col-1]->owner==owner)
            {
                if(((row+1)<=Nbrowocl)&& ((col+1)<=Nbrowocl))
                {
                    if(etw->filed[row+1-1][col+1-1]->owner==owner)return (int)1;
                    else return (int)0;
                }else return (int)0;
            }else return (int)0;
            break;
    }
}

int exist_case_beetwen_one_two(ChksField *last,ChksField *field,ChksOwner owner,ChksOwner notowner,int i,File *f,File *f1,int p)
{
    int c,d,cpt=0,kk=0;
    switch (i) {
        case 1:
            if(p)
            {
                c=last->row-1;
                d=last->col+1;
            }else {
                c=last->row;
                d=last->col;
            }
            if(c<1 || d>Nbrowocl)return (int)-2;
            do {
                if(f1)
                {
                    if(f1->tete)
                    {
                        if(c==f1->tete->row &&
                           d==f1->tete->col)
                            defiler(f1);
                    }
                }
                //le champs est le correspond
                if(!f1)
                {
                    if((field->view_table->table->filed[c-1][d-1]->row==field->row) &&
                       (field->view_table->table->filed[c-1][d-1]->col==field->col) &&
                       (field->view_table->table->filed[c-1][d-1]->owner==CHKS_OWNER_NONE))
                        return (int)1;
                }
                if(field->view_table->table->filed[c-1][d-1]->owner==notowner && cpt==0)cpt=1;
                else if((cpt==1) && (field->view_table->table->filed[c-1][d-1]->owner==CHKS_OWNER_NONE))
                {
                    cellule *ne= creer_initialiser_cellule(c,d,3,1);
                    enfiler(f,ne);
                    kk++;
                }
                if(two_consecutif(c,d,last,i,notowner) ||
                   field->view_table->table->filed[c-1][d-1]->owner==owner)
                    break;
                c--;
                d++;
            } while (c>=1 && d<=Nbrowocl);
            if(kk==0)return (int)-2;//on a rien ajouter a la file pour la traiter
            else return (int)-3;
        case 2:
            if(p)
            {
                c=last->row-1;
                d=last->col-1;
            }else {
                c=last->row;
                d=last->col;
            }
            if(c<1 || d<1 )return (int)-2;
            do {
                if(f1)
                {
                    if(f1->tete)
                    {
                        if(c==f1->tete->row &&
                           d==f1->tete->col)
                            defiler(f1);
                    }
                }
                //le champs est le correspond
                if(!f1)
                {
                    if((field->view_table->table->filed[c-1][d-1]->row==field->row) &&
                       (field->view_table->table->filed[c-1][d-1]->col==field->col) &&
                       (field->view_table->table->filed[c-1][d-1]->owner==CHKS_OWNER_NONE))
                        return (int)1;
                }
                if(field->view_table->table->filed[c-1][d-1]->owner==notowner && cpt==0)cpt=1;
                else if((cpt==1) && (field->view_table->table->filed[c-1][d-1]->owner==CHKS_OWNER_NONE))
                {
                    cellule *ne= creer_initialiser_cellule(c,d,4,2);
                    enfiler(f,ne);
                    kk++;
                }
                if(two_consecutif(c,d,last,i,notowner) ||
                   field->view_table->table->filed[c-1][d-1]->owner==owner)
                    break;
                c--;
                d--;
            } while (c>=1 && d>=1);
            if(kk==0)return (int)-2;//on a rien ajouter a la file pour la traiter
            else return (int)-3;
        case 3:
            if(p)
            {
                c=last->row+1;
                d=last->col-1;
            }else {
                c=last->row;
                d=last->col;
            }
            if(c>Nbrowocl || d<1)return (int)-2;
            do {
                if(f1)
                {
                    if(f1->tete)
                    {
                        if(c==f1->tete->row &&
                           d==f1->tete->col)
                            defiler(f1);
                    }
                }
                //le champs est le correspond
                if(!f1)
                {
                    if((field->view_table->table->filed[c-1][d-1]->row==field->row) &&
                       (field->view_table->table->filed[c-1][d-1]->col==field->col) &&
                       (field->view_table->table->filed[c-1][d-1]->owner==CHKS_OWNER_NONE))
                        return (int)1;//field trouvé
                }
                if(field->view_table->table->filed[c-1][d-1]->owner==notowner && cpt==0)cpt=1;
                else if((cpt==1) && (field->view_table->table->filed[c-1][d-1]->owner==CHKS_OWNER_NONE))
                {
                    cellule *ne= creer_initialiser_cellule(c,d,1,3);
                    enfiler(f,ne);
                    kk++;
                }
                if(two_consecutif(c,d,last,i,notowner) ||
                   field->view_table->table->filed[c-1][d-1]->owner==owner)
                    break;
                c++;
                d--;
            } while (c<=Nbrowocl && d>=1);
            if(kk==0)return (int)-2;//on a rien ajouter a la file pour la traiter
            else return (int)-3;
        case 4:
            if(p)
            {
                c=last->row+1;
                d=last->col+1;
            }else {
                c=last->row;
                d=last->col;
            }
            if((c>Nbrowocl) || (d>Nbrowocl))
                return (int)-2;
            do {
                if(f1)
                {
                    if(f1->tete)
                    {
                        if(c==f1->tete->row &&
                           d==f1->tete->col)
                            defiler(f1);
                    }
                }
                //le champs est le correspond
                if(!f1)
                {
                    if((field->view_table->table->filed[c-1][d-1]->row==field->row) &&
                       (field->view_table->table->filed[c-1][d-1]->col==field->col) &&
                       (field->view_table->table->filed[c-1][d-1]->owner==CHKS_OWNER_NONE))
                        return (int)1;
                }
                if(field->view_table->table->filed[c-1][d-1]->owner==notowner && cpt==0)cpt=1;
                else if((cpt==1) && (field->view_table->table->filed[c-1][d-1]->owner==CHKS_OWNER_NONE))
                {
                    cellule *ne= creer_initialiser_cellule(c,d,2,4);
                    enfiler(f,ne);
                    kk++;
                }
                if(two_consecutif(c,d,last,i,notowner) ||
                   field->view_table->table->filed[c-1][d-1]->owner==owner)
                    break;
                c++;
                d++;
            } while (c<=Nbrowocl && d<=Nbrowocl);
            if(kk==0)return (int)-2;//on a rien ajouter a la file pour la traiter
            else return (int)-3;//aucun de ce qui precede
    }
}


int techek_entred_piece(ChksField *filed,ChksField *last,File *f1)
{
    int i,er=0,m,c,d;
    ChksField *ff,*ff1;
    //si le champ survoler est different de none rien faire
    if(filed->owner==CHKS_OWNER_TWO || filed->owner==CHKS_OWNER_ONE)return (int)0;
    //si le diagonal en bas a gauche et a droite contient piece de l'utilisateur
    if(!last->is_queen)
    {
        if(last->owner==CHKS_OWNER_ONE)
        {
            if(last->row+1<=Nbrowocl && last->col-1>=1 && last->col+1<=Nbrowocl)
            {
                if(last->view_table->table->filed[last->row+1-1][last->col-1-1]->owner==CHKS_OWNER_ONE &&
                   last->view_table->table->filed[last->row+1-1][last->col+1-1]->owner==CHKS_OWNER_ONE)
                    return (int)0;
            }
            if(filed->row==last->row+1 && filed->col==last->col+1)return (int)1;
            if(filed->row==last->row+1 && filed->col==last->col-1)return (int)1;
            return (int)verify_for_not_queen(filed,last,last->row,last->col,f1);
        }else if(last->owner==CHKS_OWNER_TWO)
        {
            if(last->row-1>=1 && last->col-1>=1 && last->col+1<=Nbrowocl)
            {
                if(last->view_table->table->filed[last->row-1-1][last->col-1-1]->owner==CHKS_OWNER_TWO &&
                   last->view_table->table->filed[last->row-1-1][last->col+1-1]->owner==CHKS_OWNER_TWO)
                    return (int)0;
            }
            if(filed->row==last->row-1 && filed->col==last->col+1)return (int)1;
            if(filed->row==last->row-1 && filed->col==last->col-1)return (int)1;
            return (int)verify_for_not_queen_two(filed,last,last->row,last->col,f1);
        }
    }else{//verification pour queen
        /*
        * j'ai trouvé 4 champs one
        * il ne peut pas ce deplacer
        */
        if(first_is_one_four(last,last->owner))return (int)0;
        File *f=creer_initaliser_file();
        if(est_vide_file(f))
        {
            for(i=1;i<=4;i++)
            {
                //Dans cette direction field one ne peut pas ce deplacer
                if(first_is_one(last,i,last->owner))
                {
                    er++;//on a rien inserer dans la file
                    continue;
                }
                switch (last->owner) {
                    case CHKS_OWNER_ONE:
                        m=exist_case_beetwen_one_two(last,filed,CHKS_OWNER_ONE,CHKS_OWNER_TWO,i,f,f1,1);
                        break;
                    case CHKS_OWNER_TWO:
                        m=exist_case_beetwen_one_two(last,filed,CHKS_OWNER_TWO,CHKS_OWNER_ONE,i,f,f1,1);
                        break;
                }
                if(m==-2)er++;
                else if(m==1)
                {
                    return (int)1;
                }
            }
        }
        if(f1)
        {
            if(!f1->tete && !f1->queue)return (int)1;
        }
        if(er==4)return (int)0;/*il n'a pas trouvé et il n'existe pas une case beetwen last machine field
         and first one field   */
        //continue a verifie
        while (!est_vide_file(f))
        {
            for(i=1;i<=4;i++)
            {
                if(i!=f->tete->diag_not_verify && i!=f->tete->diag_not_verify2)
                {
                    ff=last->view_table->table->filed[f->tete->row-1][f->tete->col-1];
                    switch (last->owner) {
                        case CHKS_OWNER_ONE:
                            first_one_or_two(ff,&c,&d,i,CHKS_OWNER_TWO,1,NULL,-1,-1);
                            break;
                        case CHKS_OWNER_TWO:
                            first_one_or_two(ff,&c,&d,i,CHKS_OWNER_ONE,1,NULL,-1,-1);
                            break;
                    }
                    if((c==-1)&&(d==-1))continue;
                    ff1=last->view_table->table->filed[c-1][d-1];
                    switch (last->owner) {
                        case CHKS_OWNER_ONE:
                            m=exist_case_beetwen_one_two(ff1,filed,CHKS_OWNER_ONE,CHKS_OWNER_TWO,i,f,f1,0);
                            break;
                        case CHKS_OWNER_TWO:
                            m=exist_case_beetwen_one_two(ff1,filed,CHKS_OWNER_TWO,CHKS_OWNER_ONE,i,f,f1,0);
                            break;
                    }
                    if(m==1)return (int)1;//field trouvé
                }
            }
            defiler(f);
        }
        if(!f1)return (int)0;
        else {

            if(!f1->tete && !f1->queue)return (int)1;
            else return (int)0;
        }
    }
}

void piece_entered(GtkWidget *button,ChksField *field)
{
    if(!field->view_table->table->last)return;
    if(field->owner != CHKS_OWNER_NONE)return;
    if(techek_entred_piece(field,field->view_table->table->last,NULL))
    {
        GtkStyleContext* context = gtk_widget_get_style_context(button);
        GtkCssProvider* provider = gtk_css_provider_new();
        gtk_css_provider_load_from_path(provider, __STYLE__, NULL);
        switch (field->view_table->table->last->owner) {

            case CHKS_OWNER_ONE:
                gtk_style_context_add_class(GTK_STYLE_CONTEXT(context),"chks-player-one-pre");
                break;
            case CHKS_OWNER_TWO:
                gtk_style_context_add_class(GTK_STYLE_CONTEXT(context),"chks-player-two-pre");
                break;
        }
        gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    }
}

int vider_file(File *f)
{
    if(!f)return (int)-1;
    f->tete=NULL;
    f->queue=NULL;
    return (int)1;
}


void piece_cliecked(GtkWidget *button, ChksField *fieled)
{
    if (fieled->view_table->pause) return;
    if(fieled->view_table->user->nbr_piece_disp==0 ||
       fieled->view_table->machine->nbr_piece_disp==0)
        return;//l'un des deux joueur a gagné

    switch (fieled->view_table->round) {
        case 1:
            if(fieled->owner==CHKS_OWNER_TWO)return;
            break;
        case 2:
            if(fieled->owner==CHKS_OWNER_ONE)return;
            break;
    }
    if(fieled->view_table->table->last)
    {
        if(fieled->owner==CHKS_OWNER_ONE || fieled->owner==CHKS_OWNER_TWO )
        {
            vider_file(fieled->view_table->g->f);
            fieled->view_table->table->last=fieled;
            return;
        }
        //field NONE
        cellule *ne= creer_initialiser_cellule(fieled->row,
                                               fieled->col,-1,-1);
        enfiler(fieled->view_table->g->f,ne);
        fieled->view_table->g->field=fieled;//le dernier None clicker
    }else fieled->view_table->table->last=fieled;
}

void piece_leaved(GtkWidget *button,ChksField *field)
{
    GtkStyleContext* context;
    context = gtk_widget_get_style_context(button);
    gtk_style_context_remove_class(context, "chks-player-one-pre");
    gtk_style_context_remove_class(context, "chks-player-two-pre");
}

int copy_file(File *sourceF, File *destF)
{
    File *tmp=creer_initaliser_file();
    if(!sourceF)return (int)0;
    while (!est_vide_file(sourceF))
    {
        cellule *ne= creer_initialiser_cellule(sourceF->tete->row,sourceF->tete->col,-1,-1);
        cellule *ne1= creer_initialiser_cellule(sourceF->tete->row,sourceF->tete->col,-1,-1);
        enfiler(destF,ne);
        enfiler(tmp,ne1);
        defiler(sourceF);
    }
    while (!est_vide_file(tmp))
    {
        cellule *ne= creer_initialiser_cellule(tmp->tete->row,tmp->tete->col,-1,-1);
        enfiler(sourceF,ne);
        defiler(tmp);
    }
    return (int)1;
}


cellule2 *inserer_fin_liste(cellule2 *liste,int score,int row,int col)
{
    cellule2 *ne= crrer_initailiser_ccellule2(score,row,col);
    if(!liste)return (cellule2*)ne;
    if(!liste->suiv)
    {
        liste->suiv=ne;
        return (cellule2*)liste;
    }
    cellule2 *p,*q;
    p=liste;
    q=liste;
    while (p)
    {
        q=p;
        p=p->suiv;
    }
    q->suiv=ne;
    return (cellule2*)liste;
}

cellule2 *inserer_fin_liste_cellule2(cellule2 *liste,int score,int row,int col)
{
    cellule2 *ne= crrer_initailiser_ccellule2 (score,row,col);
    if(!liste)return (cellule2 *)ne;
    if(!liste->suiv)
    {
        liste->suiv=ne;
        return (cellule2 *)liste;
    }
    cellule2 *p,*q;
    p=liste;
    q=liste;
    while (p)
    {
        q=p;
        p=p->suiv;
    }
    q->suiv=ne;
    return (cellule2 *)liste;
}


cellule2 *construire_arbre1(etat_window *etw,cellule2 *ar,int row,int col)//machine
{
    if((row-1>=1) && (col+1<=Nbrowocl ) && (row-2>=1) && (col+2<=Nbrowocl))
    {
        if((etw->filed[row-1-1][col+1-1]->owner==CHKS_OWNER_ONE) &&
           (etw->filed[row-2-1][col+2-1]->owner==CHKS_OWNER_NONE))
        {
            cellule2 *ne= crrer_initailiser_ccellule2(0,row-2,col+2);
            ar->topright=construire_arbre1(etw,ne,row-2,col+2);
        }
    }
    if((row-1>=1) && (col-1>=1 ) && (row-2>=1) && (col-2>=1))
    {
        if((etw->filed[row-1-1][col-1-1]->owner==CHKS_OWNER_ONE) &&
           (etw->filed[row-2-1][col-2-1]->owner==CHKS_OWNER_NONE))
        {
            cellule2 *ne= crrer_initailiser_ccellule2(0,row-2,col-2);
            ar->topleft=construire_arbre1(etw,ne,row-2,col-2);
        }
    }
    return (cellule2*)ar;
}

cellule2 *construire_arbre2(etat_window *etw,cellule2 *ar,int row,int col)//user
{

    if((row+1<=Nbrowocl) && (col+1<=Nbrowocl ) && (row+2<=Nbrowocl) && (col+2<=Nbrowocl))
    {
        if((etw->filed[row+1-1][col+1-1]->owner==CHKS_OWNER_TWO) &&
           (etw->filed[row+2-1][col+2-1]->owner==CHKS_OWNER_NONE))
        {
            cellule2 *ne= crrer_initailiser_ccellule2(0,row+2,col+2);
            ar->topright=construire_arbre2(etw,ne,row+2,col+2);
        }
    }
    if((row+1<=Nbrowocl) && (col-1>=Nbrowocl ) && (row+2<=Nbrowocl) && (col-2>=Nbrowocl))
    {
        if((etw->filed[row+1-1][col-1-1]->owner==CHKS_OWNER_TWO) &&
           (etw->filed[row+2-1][col-2-1]->owner==CHKS_OWNER_NONE))
        {
            cellule2 *ne= crrer_initailiser_ccellule2(0,row+2,col-2);
            ar->topleft=construire_arbre2(etw,ne,row+2,col-2);
        }
    }
    return (cellule2*)ar;
}



cellule2 *add_none_first(cellule2 *list,int i,etat_window *etw,int row,int col)
{

    switch (i) {
        case 1:
            row=row-1;
            col=col+1;
            while (row>=1 &&col<=Nbrowocl)
            {
                if(etw->filed[row-1][col-1]->owner==CHKS_OWNER_NONE)
                {
                    list=inserer_fin_liste_cellule2(list,0,row,col);
                    row--;
                    col++;
                }else break;
            }
            break;
        case 2:
            row=row-1;
            col=col-1;
            while (row>=1 &&col>=1)
            {
                if(etw->filed[row-1][col-1]->owner==CHKS_OWNER_NONE)
                {
                    list=inserer_fin_liste_cellule2(list,0,row,col);
                    row--;
                    col--;
                }else break;
            }
            break;

        case 3:
            row=row+1;
            col=col-1;
            while (row<=Nbrowocl &&col>=1)
            {
                if(etw->filed[row-1][col-1]->owner==CHKS_OWNER_NONE)
                {
                    list=inserer_fin_liste_cellule2(list,0,row,col);
                    row++;
                    col--;
                }else break;
            }
            break;
        case 4:
            row=row+1;
            col=col+1;
            while (row<=Nbrowocl &&col<=Nbrowocl)
            {
                if(etw->filed[row-1][col-1]->owner==CHKS_OWNER_NONE)
                {
                    list=inserer_fin_liste_cellule2(list,0,row,col);
                    row++;
                    col++;
                }else break;
            }
            break;
    }
    return (cellule2*)list;
}


cellule2 *inserer_debut(cellule2 *list,cellule2 *ne)
{
    if(!list)return (cellule2*)ne;
    ne->suiv=list;
    list=ne;
    return (cellule2*)list;
}

cellule2 *construire_arbre_queen(ChksField *field,cellule2 *list,int onerow,int onecol,int i)
{
    int c,d,k,l;
    cellule2 *kk=NULL;
    int onerow1,onecol1,tworow1,twocol1;
    ChksField *ff=field->view_table->table->filed[onerow-1][onecol-1];
    if(i!=4)//for 2
    {
        first_one_or_two(ff,&c,&d,2,CHKS_OWNER_ONE,1,NULL,-1,-1);
        if(c!=-1 && d!=-1)
        {
            first_one_or_two(ff,&k,&l,2,CHKS_OWNER_TWO,0,NULL,-1,-1);
            onerow1=c-1;
            onecol1=d-1;
            tworow1=k+1;
            twocol1=l+1;
            while (onerow1>=tworow1 && onecol1 >=twocol1)
            {
                if(field->view_table->table->filed[onerow1-1][onecol1-1]->owner==CHKS_OWNER_NONE)
                {
                    kk=construire_arbre_queen(field,list->topleft,onerow1,onecol1,4);
                    list->topleft= inserer_debut(list->topleft,kk);
                }
                onerow1--;
                onecol1--;
            }
        }
    }
    if(i!=2)//for 4
    {
        first_one_or_two(ff,&c,&d,4,CHKS_OWNER_ONE,1,NULL,-1,-1);
        if(c!=-1 && d!=-1)
        {
            first_one_or_two(ff,&k,&l,4,CHKS_OWNER_TWO,0,NULL,-1,-1);
            onerow1=c+1;
            onecol1=d+1;
            tworow1=k-1;
            twocol1=l-1;
            while (onerow1<=tworow1 && onecol1 <=twocol1)
            {
                if(field->view_table->table->filed[onerow1-1][onecol1-1]->owner==CHKS_OWNER_NONE)
                {
                    kk=construire_arbre_queen(field,list->butright,onerow1,onecol1,4);
                    list->butright= inserer_debut(list->butright,kk);
                }
                onerow1++;
                onecol1++;
            }
        }
    }
    if(i!=1)//for 3
    {
        first_one_or_two(ff,&c,&d,3,CHKS_OWNER_ONE,1,NULL,-1,-1);
        if(c!=-1 && d!=-1)
        {
            first_one_or_two(ff,&k,&l,3,CHKS_OWNER_TWO,0,NULL,-1,-1);
            onerow1=c+1;
            onecol1=d-1;
            tworow1=k-1;
            twocol1=l+1;
            while (onerow1<=tworow1 && onecol1 >=twocol1)
            {
                if(field->view_table->table->filed[onerow1-1][onecol1-1]->owner==CHKS_OWNER_NONE)
                {
                    kk=construire_arbre_queen(field,list->butleft,onerow1,onecol1,4);
                    list->butleft= inserer_debut(list->butleft,kk);
                }
                onerow1++;
                onecol1--;
            }
        }
    }
    if(i!=3)//for 1
    {
        first_one_or_two(ff,&c,&d,1,CHKS_OWNER_ONE,1,NULL,-1,-1);
        if(c!=-1 && d!=-1)
        {
            first_one_or_two(ff,&k,&l,1,CHKS_OWNER_TWO,0,NULL,-1,-1);
            onerow1=c-1;
            onecol1=d+1;
            tworow1=k+1;
            twocol1=l-1;
            while (onerow1>=tworow1 && onecol1 <=twocol1)
            {
                if(field->view_table->table->filed[onerow1-1][onecol1-1]->owner==CHKS_OWNER_NONE)
                {
                    kk=construire_arbre_queen(field,list->topright,onerow1,onecol1,4);
                    list->topright= inserer_debut(list->topright,kk);
                }
                onerow1--;
                onecol1++;
            }
        }
    }
    return (cellule2*)list;
}


void drop(ChksField *last,ChksField *field,ChksOwner owner)
{
    int kk;
    char str[10];
    int lrow,lcol,frow,fcol,a=-1,b=-1;
    GtkStyleContext* context;
    lrow=last->row;
    lcol=last->col;
    frow=field->row;
    fcol=field->col;
    if(frow > lrow && fcol < lcol)
    {
        while (frow>lrow && fcol <lcol)
        {
            if(field->view_table->table->filed[frow-1][fcol-1]->owner==owner)
            {
                switch (owner) {
                    case CHKS_OWNER_ONE:
                        kk= atoi(gtk_label_get_label(GTK_LABEL(field->view_table->labelTwo)));
                        sprintf(str,"%d",kk+1);
                        gtk_label_set_label(GTK_LABEL(field->view_table->labelTwo), str);
                        break;
                    case CHKS_OWNER_TWO:
                        kk= atoi(gtk_label_get_label(GTK_LABEL(field->view_table->labelOne)));
                        sprintf(str,"%d",kk+1);
                        gtk_label_set_label(GTK_LABEL(field->view_table->labelOne), str);
                        break;
                }
                field->view_table->table->filed[frow-1][fcol-1]->owner=CHKS_OWNER_NONE;
                a=frow;
                b=fcol;
            }
            frow--;
            fcol++;
        }
    }else if(frow> lrow && fcol > lcol)
    {
        while (frow>lrow && fcol >lcol)
        {
            if(field->view_table->table->filed[frow-1][fcol-1]->owner==owner)
            {
                switch (owner) {
                    case CHKS_OWNER_ONE:
                        kk= atoi(gtk_label_get_label(GTK_LABEL(field->view_table->labelTwo)));
                        sprintf(str,"%d",kk+1);
                        gtk_label_set_label(GTK_LABEL(field->view_table->labelTwo), str);
                        break;
                    case CHKS_OWNER_TWO:
                        kk= atoi(gtk_label_get_label(GTK_LABEL(field->view_table->labelOne)));
                        sprintf(str,"%d",kk+1);
                        gtk_label_set_label(GTK_LABEL(field->view_table->labelOne), str);
                        break;
                }
                field->view_table->table->filed[frow-1][fcol-1]->owner=CHKS_OWNER_NONE;
                a=frow;
                b=fcol;
            }
            frow--;
            fcol--;
        }
    }else if(frow< lrow && fcol > lcol)
    {
        while (frow<lrow && fcol >lcol)
        {
            if(field->view_table->table->filed[frow-1][fcol-1]->owner==owner)
            {
                switch (owner) {
                    case CHKS_OWNER_ONE:
                        kk= atoi(gtk_label_get_label(GTK_LABEL(field->view_table->labelTwo)));
                        sprintf(str,"%d",kk+1);
                        gtk_label_set_label(GTK_LABEL(field->view_table->labelTwo), str);
                        break;
                    case CHKS_OWNER_TWO:
                        kk= atoi(gtk_label_get_label(GTK_LABEL(field->view_table->labelOne)));
                        sprintf(str,"%d",kk+1);
                        gtk_label_set_label(GTK_LABEL(field->view_table->labelOne), str);
                        break;
                }
                field->view_table->table->filed[frow-1][fcol-1]->owner=CHKS_OWNER_NONE;
                a=frow;
                b=fcol;
            }
            frow++;
            fcol--;
        }
    }else if(frow < lrow && fcol < lcol)
    {
        while (frow <lrow && fcol <lcol)
        {
            if(field->view_table->table->filed[frow-1][fcol-1]->owner==owner)
            {
                switch (owner) {
                    case CHKS_OWNER_ONE:
                        kk= atoi(gtk_label_get_label(GTK_LABEL(field->view_table->labelTwo)));
                        sprintf(str,"%d",kk+1);
                        gtk_label_set_label(GTK_LABEL(field->view_table->labelTwo), str);
                        break;
                    case CHKS_OWNER_TWO:
                        kk= atoi(gtk_label_get_label(GTK_LABEL(field->view_table->labelOne)));
                        sprintf(str,"%d",kk+1);
                        gtk_label_set_label(GTK_LABEL(field->view_table->labelOne), str);
                        break;
                }
                field->view_table->table->filed[frow-1][fcol-1]->owner=CHKS_OWNER_NONE;
                a=frow;
                b=fcol;
            }
            frow++;
            fcol++;
        }
    }
    if(a!=-1 && b!=-1)
    {
        switch (owner) {
            case CHKS_OWNER_ONE:
                delete_value_ChkPlayer(field->view_table->user,a,b);
                break;
            case CHKS_OWNER_TWO:
                delete_value_ChkPlayer(field->view_table->machine,a,b);
                break;
        }
    }
}

void drop1(etat_window *etw1,ChksOwner owner,int lrow,int lcol,int frow,int fcol)
{
    GtkStyleContext* context;
    int a=-1,b=-1;
    if(frow > lrow && fcol < lcol)
    {
        while (frow>lrow && fcol <lcol)
        {
            if(etw1->filed[frow-1][fcol-1]->owner==owner)
            {
                etw1->filed[frow-1][fcol-1]->owner=CHKS_OWNER_NONE;
                a=frow;
                b=fcol;
            }
            frow--;
            fcol++;
        }
    }else if(frow> lrow && fcol > lcol)
    {
        while (frow>lrow && fcol >lcol)
        {
            if(etw1->filed[frow-1][fcol-1]->owner==owner)
            {
                etw1->filed[frow-1][fcol-1]->owner=CHKS_OWNER_NONE;
                a=frow;
                b=fcol;
            }
            frow--;
            fcol--;
        }
    }else if(frow< lrow && fcol > lcol)
    {
        while (frow<lrow && fcol >lcol)
        {
            if(etw1->filed[frow-1][fcol-1]->owner==owner)
            {
                etw1->filed[frow-1][fcol-1]->owner=CHKS_OWNER_NONE;
                a=frow;
                b=fcol;
            }
            frow++;
            fcol--;
        }
    }else if(frow < lrow && fcol < lcol)
    {
        while (frow <lrow && fcol <lcol)
        {
            if(etw1->filed[frow-1][fcol-1]->owner==owner)
            {
                etw1->filed[frow-1][fcol-1]->owner=CHKS_OWNER_NONE;
                a=frow;
                b=fcol;
            }
            frow++;
            fcol++;
        }
    }
    if(a!=-1 && b!=-1)
    {
        switch (owner) {
            case CHKS_OWNER_ONE:
                delete_value_ChkPlayer(etw1->user,a,b);
                break;
            case CHKS_OWNER_TWO:
                delete_value_ChkPlayer(etw1->machine,a,b);
                break;
        }
    }
}


//file sera rempli si il ya plusieurs deplacement
void drop_two_betwen_fl_lt(ChksField *last,ChksField *field,File *f,ChksOwner owner,int p)
{
    int a,b,c,d;
    if(!f)
    {
        drop(last,field,owner);
    }else{
        if(p)defiler(f);
        a=f->tete->row;
        b=f->tete->col;
        drop(last,field->view_table->table->filed[a-1][b-1],owner);
        defiler(f);
        if(est_vide_file(f))return;
        do
        {
            c=f->tete->row;
            d=f->tete->col;
            drop(field->view_table->table->filed[a-1][b-1],
                 field->view_table->table->filed[c-1][d-1],owner);
            a=c;
            b=d;
            defiler(f);
        } while (!est_vide_file(f));
    }
}

void drop_two_betwen_fl_lt1(etat_window *etw1,File *f,ChksOwner owner,int lrow,int lcol,int frow,int fcol)
{
    int a,b,c,d;
    if(!f)
    {
        drop1(etw1,owner,lrow,lcol,frow,fcol);
    }else{
        defiler(f);
        a=f->tete->row;
        b=f->tete->col;
        drop1(etw1,owner,lrow,lcol,a,b);
        defiler(f);
        if(est_vide_file(f))return;
        do
        {
            c=f->tete->row;
            d=f->tete->col;
            drop1(etw1,owner,a,b,c,d);
            defiler(f);
        } while (!est_vide_file(f));
    }
}

ChksField *creer_field()
{
    ChksField *fl=(ChksField*)malloc(sizeof (ChksField));
    if(!fl)
    {
        printf("error allocation");
        exit(-1);
    }
    return (ChksField*)fl;
}


etat_window *creer_intialiser_etat_window()
{
    etat_window *info;
    info=(etat_window*)malloc(sizeof (etat_window));
    if(!info)
    {
        printf("error allocation \n");
        exit(-1);
    }
    int i,j;
    for(i=0;i<Nbrowocl;i++)
        for(j=0;j<Nbrowocl;j++)
            info->filed[i][j]=creer_field();
    info->user=creer_player();
    info->machine=creer_player();
    return (etat_window*)info;
}

CLpile *creer_intialiser_Cpile(etat_window *etww)
{
    CLpile *pl=(CLpile*)malloc(sizeof (CLpile));
    if(!pl)
    {
        printf("error allocation \n");
        exit(-1);
    }
    pl->wintmp=etww;
    pl->suiv=NULL;
    return (CLpile*)pl;
}

CLpile *empiler(CLpile *pile,CLpile *ne)
{
    if(!pile)return (CLpile*)ne;
    ne->suiv=pile;
    pile=ne;
    return (CLpile*)pile;
}

CLpile *depiler(CLpile *pile,CLpile *T[1])
{
    if(!pile)return (CLpile*)NULL;
    T[0]=pile;
    pile=pile->suiv;
    T[0]->suiv=NULL;

    return (CLpile*)pile;
}



void copie_win_field(Chkswindow *winS,etat_window *winD)
{
    int i,j;
    for(i=0;i<winS->user->nbr_piece_disp;i++)
    {
        winD->user->Tab[i].x=winS->user ->Tab[i].x;
        winD->user->Tab[i].y=winS->user ->Tab[i].y;
    }
    winD->user->nbr_piece_disp=winS->user->nbr_piece_disp;
    for(i=0;i<winS->machine->nbr_piece_disp;i++)
    {
        winD->machine->Tab[i].x=winS->machine ->Tab[i].x;
        winD->machine->Tab[i].y=winS->machine ->Tab[i].y;
    }
    winD->machine->nbr_piece_disp=winS->machine->nbr_piece_disp;
    for(i=0;i<Nbrowocl;i++)
    {
        for(j=0;j<Nbrowocl;j++)
        {
            winD->filed[i][j]->row=winS->table->filed[i][j]->row;
            winD->filed[i][j]->col=winS->table->filed[i][j]->col;
            winD->filed[i][j]->owner=winS->table->filed[i][j]->owner;
            winD->filed[i][j]->is_queen=winS->table->filed[i][j]->is_queen;
        }
    }
}void copie_win_field1(etat_window *winS,etat_window *winD)
{
    int i,j;
    for(i=0;i<winS->user->nbr_piece_disp;i++)
    {
        winD->user->Tab[i].x=winS->user ->Tab[i].x;
        winD->user->Tab[i].y=winS->user ->Tab[i].y;
    }
    winD->user->nbr_piece_disp=winS->user->nbr_piece_disp;
    for(i=0;i<winS->machine->nbr_piece_disp;i++)
    {
        winD->machine->Tab[i].x=winS->machine ->Tab[i].x;
        winD->machine->Tab[i].y=winS->machine ->Tab[i].y;
    }
    winD->machine->nbr_piece_disp=winS->machine->nbr_piece_disp;
    for(i=0;i<Nbrowocl;i++)
    {
        for(j=0;j<Nbrowocl;j++)
        {
            winD->filed[i][j]->row=winS->filed[i][j]->row;
            winD->filed[i][j]->col=winS->filed[i][j]->col;
            winD->filed[i][j]->owner=winS->filed[i][j]->owner;
            winD->filed[i][j]->is_queen=winS->filed[i][j]->is_queen;
        }
    }
}


cellule *inserer_fin_liste_cellule(cellule *liste,int row,int col)
{
    cellule *ne= creer_initialiser_cellule(row,col,-1,-1);
    if(!liste)return (cellule*)ne;
    if(!liste->suiv)
    {
        liste->suiv=ne;
        return (cellule*)liste;
    }
    cellule *p,*q;
    p=liste;
    while (p)
    {
        q=p;
        p=p->suiv;
    }
    q->suiv=ne;
    return (cellule*)liste;
}




cellule2 *queen_arbre1(cellule2 *ar,int i,etat_window *etw,int onerow,int onecol,ChksOwner owner,ChksOwner notowner)//machine
{
    int c,d,f,g;
    ChksField *field1;
    switch (i)
    {
        case 1://top right
            f=onerow;
            g=onecol;
            do{
                field1=etw->filed[onerow-1][onecol-1];
                if(field1->owner==CHKS_OWNER_NONE)
                {
                    etw->filed[f-1][g-1]->owner=CHKS_OWNER_NONE;
                    cellule2 *ne= crrer_initailiser_ccellule2(0,onerow,onecol);
                    first_one_or_two(NULL,&c,&d,2,owner,1,etw,field1->row,field1->col);
                    if(c!=-1 && d!=-1)
                    {

                        ne=queen_arbre1(ne,2,etw,c,d,owner,notowner);
                    }
                    first_one_or_two(NULL,&c,&d,4,owner,1,etw,field1->row,field1->col);
                    if(c!=-1 && d!=-1)
                    {
                        ne=queen_arbre1(ne,4,etw,c,d,owner,notowner);
                    }
                    ar->topright=inserer_debut(ar->topright,ne);
                }else if((field1->owner==notowner)||two_consecutif2(onerow,onecol,etw,i,owner))break;
                onerow--;
                onecol++;
            } while ((onerow>=1) && (onecol<=Nbrowocl));
            break;
        case 2://top left
            f=onerow;
            g=onecol;
            do{
                field1=etw->filed[onerow-1][onecol-1];
                if(field1->owner==CHKS_OWNER_NONE)
                {
                    etw->filed[f-1][g-1]->owner=CHKS_OWNER_NONE;
                    cellule2 *ne= crrer_initailiser_ccellule2(0,onerow,onecol);
                    first_one_or_two(NULL,&c,&d,1,owner,1,etw,field1->row,field1->col);
                    if(c!=-1 && d!=-1)
                    {
                        ne=queen_arbre1(ne,1,etw,c,d,owner,notowner);
                    }
                    first_one_or_two(NULL,&c,&d,3,owner,1,etw,field1->row,field1->col);
                    if(c!=-1 && d!=-1)
                    {
                        ne=queen_arbre1(ne,3,etw,c,d,owner,notowner);
                    }
                    ar->topleft=inserer_debut(ar->topleft,ne);
                }else if((field1->owner==notowner) || (two_consecutif2(onerow,onecol,etw,i,owner)))break;
                onerow--;
                onecol--;
            } while ((onerow>=1) && (onecol>=1));
            break;
        case 3://buttom left
            f=onerow;
            g=onecol;
            do{
                field1=etw->filed[onerow-1][onecol-1];
                if(field1->owner==CHKS_OWNER_NONE)
                {
                    etw->filed[f-1][g-1]->owner=CHKS_OWNER_NONE;
                    cellule2 *ne= crrer_initailiser_ccellule2(0,onerow,onecol);
                    first_one_or_two(NULL,&c,&d,2,owner,1,etw,field1->row,field1->col);
                    if(c!=-1 && d!=-1)
                    {
                        ne=queen_arbre1(ne,2,etw,c,d,owner,notowner);
                    }
                    first_one_or_two(NULL,&c,&d,4,owner,1,etw,field1->row,field1->col);
                    if(c!=-1 && d!=-1)
                    {
                        ne=queen_arbre1(ne,4,etw,c,d,owner,notowner);
                    }
                    ar->butleft=inserer_debut(ar->butleft,ne);
                }else if((field1->owner==notowner) || (two_consecutif2(onerow,onecol,etw,i,owner)))break;
                onerow++;
                onecol--;
            } while ((onerow<=Nbrowocl) && (onecol>=1));
            break;
        case 4://buttom right
            f=onerow;
            g=onecol;
            do{
                field1=etw->filed[onerow-1][onecol-1];
                if(field1->owner==CHKS_OWNER_NONE)
                {
                    etw->filed[f-1][g-1]->owner=CHKS_OWNER_NONE;
                    cellule2 *ne= crrer_initailiser_ccellule2(0,onerow,onecol);
                    first_one_or_two(NULL,&c,&d,1,owner,1,etw,field1->row,field1->col);
                    if(c!=-1 && d!=-1)
                    {
                        ne=queen_arbre1(ne,1,etw,c,d,owner,notowner);
                    }
                    first_one_or_two(NULL,&c,&d,3,owner,1,etw,field1->row,field1->col);
                    if(c!=-1 && d!=-1)
                    {
                        ne=queen_arbre1(ne,3,etw,c,d,owner,notowner);
                    }
                    ar->butright=inserer_debut(ar->butright,ne);
                }else if((field1->owner==notowner) || (two_consecutif2(onerow,onecol,etw,i,owner)))break;
                onerow++;
                onecol++;
            } while ((onerow<=Nbrowocl) && (onecol<=Nbrowocl));
            break;
    }
    return (cellule2*)ar;
}

cellule2 **table_arbre_shemin(cellule2 **cl,etat_window *etw,ChksOwner owner)
{
    int taille,n,i,row,col,j;
    ChksField *field;
    switch (owner) {
        case CHKS_OWNER_ONE:
            taille=etw->user->nbr_piece_disp;
            cl=(cellule2**) malloc(sizeof (cellule2*)*taille);
            if(!cl)
            {
                printf("error allocation");
                exit(-1);
            }
            for(n=0;n<taille;n++)cl[n]=NULL;
            for(i=0;i<taille;i++)
            {
                row=etw->user->Tab[i].x;
                col=etw->user->Tab[i].y;
                if(!etw->filed[row-1][col-1]->is_queen)
                {
                    if((row+1<=Nbrowocl) && (col-1>=1))
                    {
                        if(etw->filed[row+1-1][col-1-1]->owner==CHKS_OWNER_NONE)//left
                            cl[i]=inserer_fin_liste(cl[i],0,row+1,col-1);
                    }
                    if((row+1<=Nbrowocl) && (col+1<=Nbrowocl))
                    {
                        if(etw->filed[row+1-1][col+1-1]->owner==CHKS_OWNER_NONE)//right
                            cl[i]=inserer_fin_liste(cl[i],0,row+1,col+1);
                    }
                    cellule2 *ne_test= crrer_initailiser_ccellule2(0,row,col);
                    ne_test=construire_arbre2(etw,ne_test,row,col);
                    if(ne_test->topleft || ne_test->topright)
                    {
                        if(!cl[i])
                            cl[i]=ne_test;
                        else {
                            ne_test->suiv = cl[i];
                            cl[i] = ne_test;
                        }
                    }
                }else//  if queen ,a traité
                {
                    cellule2 *aa=NULL;
                    cellule2 *aa1= crrer_initailiser_ccellule2(0,row,col);
                    int c,d,k,l;
                    for(j=1;j<=4;j++)
                    {
                        aa=add_none_first(aa,j,etw,row,col);
                        first_one_or_two(NULL,&c,&d,j,CHKS_OWNER_TWO,1,etw,row,col);
                        if(c==-1 && d==-1)continue;
                        else //construire l'arbre pour le champ
                        {
                            etat_window *etw1=creer_intialiser_etat_window();
                            copie_win_field1(etw,etw1);
                            aa1=queen_arbre1(aa1,j,etw1,c,d,
                                             CHKS_OWNER_TWO,CHKS_OWNER_ONE);
                        }
                    }
                    if(aa1->topright || aa1->topleft || aa1->butright || aa1->butleft)
                        aa=inserer_debut(aa,aa1);
                    cl[i]=aa;
                }
            }
            break;
        case CHKS_OWNER_TWO://cas de la machine
            taille=etw->machine->nbr_piece_disp;
            cl=(cellule2**) malloc(sizeof (cellule2*)*taille);
            if(!cl)
            {
                printf("error allocation");
                exit(-1);
            }
            for(n=0;n<taille;n++)cl[n]=NULL;
            for(i=0;i<taille;i++)
            {
                row=etw->machine->Tab[i].x;
                col=etw->machine->Tab[i].y;
                if(!etw->filed[row-1][col-1]->is_queen)
                {
                    if( (row-1>=1) && (col-1>=1))
                    {
                        if(etw->filed[row-1-1][col-1-1]->owner==CHKS_OWNER_NONE)//left
                            cl[i]=inserer_fin_liste(cl[i],0,row-1,col-1);
                    }
                    if((row-1>=1) && (col+1<=Nbrowocl))
                    {
                        if(etw->filed[row-1-1][col+1-1]->owner==CHKS_OWNER_NONE)//right
                            cl[i]=inserer_fin_liste(cl[i],0,row-1,col+1);
                    }
                    cellule2 *ne_test= crrer_initailiser_ccellule2(0,row,col);
                    ne_test=construire_arbre1(etw,ne_test,row,col);
                    if(ne_test->topleft || ne_test->topright)
                    {
                        if(!cl[i]) cl[i]=ne_test;
                        else {
                            ne_test->suiv = cl[i];
                            cl[i] = ne_test;
                        }
                    }
                }else//  if queen ,a traité
                {
                    cellule2 *aa=NULL;
                    cellule2 *aa1= crrer_initailiser_ccellule2(0,row,col);
                    int c,d,k,l;
                    for(j=1;j<=4;j++)
                    {
                        aa=add_none_first(aa,j,etw,row,col);
                        first_one_or_two(NULL,&c,&d,j,CHKS_OWNER_ONE,1,etw,row,col);
                        if(c==-1 && d==-1)continue;
                        else //construire l'arbre pour le champ
                        {
                            etat_window *etw1=creer_intialiser_etat_window();
                            copie_win_field1(etw,etw1);
                            //(c) et la ligne de primiere one et (d) sa colonon
                            aa1=queen_arbre1(aa1,j,etw1,c,d,CHKS_OWNER_ONE,CHKS_OWNER_TWO);
                        }
                    }
                    if(aa1->topright || aa1->topleft || aa1->butright || aa1->butleft)
                        aa=inserer_debut(aa,aa1);
                    cl[i]=aa;
                }
            }
            break;
    }
    return (cellule2**)cl;
}

cellule7 *crrer_initialiser_cellule7(int x,int y)
{
    cellule7 *ne=(cellule7*)malloc(sizeof (cellule7));
    if(!ne)
    {
        printf("error allocation");
        exit(-1);
    }
    ne->x=x;
    ne->y=y;
    ne->suiv=NULL;
    return (cellule7*)ne;
}

cellule7 *inserer_fin_liste_cellule7(cellule7 *liste,cellule7 *ne1)
{
    if(!liste)return (cellule7 *)ne1;
    if(!liste->suiv)
    {
        liste->suiv=ne1;
        return (cellule7 *)liste;
    }
    cellule7 *p,*q;
    p=liste;
    q=liste;
    while (p)
    {
        q=p;
        p=p->suiv;
    }
    q->suiv=ne1;
    return (cellule7 *)liste;
}

cellule5 *creer_intialiser_cellule5(cellule7 *ll,int numfield)
{
    cellule5 *ne=(cellule5*)malloc(sizeof(cellule5));
    ne->liste=NULL;
    if(!ne)
    {
        printf("error allocation");
        exit(-1);
    }
    cellule7 *p;
    cellule7 *ne1;
    p=ll;
    while (p)
    {
        ne1=crrer_initialiser_cellule7(p->x,p->y);
        ne->liste=inserer_fin_liste_cellule7(ne->liste,ne1);
        p=p->suiv;
    }
    ne->numfield=numfield;
    ne->suiv=NULL;
    return (cellule5*)ne;
}

cellule7 *createCellule1(int x, int y)
{
    cellule7 *newCell = (cellule7 *)malloc(sizeof(cellule7));
    if (!newCell) exit(0);
    newCell->x = x;
    newCell->y = y;
    newCell->suiv = NULL;
    return ((cellule7 *)newCell);
}

cellule7 *addToEndOfListe (cellule7 *liste, int x, int y)
{
    cellule7 *ptr = liste;
    if (!liste) return ((cellule7 *)createCellule1(x, y));
    while (ptr->suiv) ptr = ptr->suiv;
    ptr->suiv = createCellule1(x, y);
    return ((cellule7 *)liste);
}

CelluleList *createCelluleList (cellule7 *liste)
{
    CelluleList *newCellList = (CelluleList *)malloc(sizeof(CelluleList));
    if (!newCellList) exit(0);
    newCellList->liste = liste;
    newCellList->suiv = NULL;
    return ((CelluleList *)newCellList);
}

CelluleList *addList (CelluleList *listes, cellule7 *liste)
{
    CelluleList *newList = createCelluleList(liste);
    if (listes) newList->suiv = listes;
    return ((CelluleList *)newList);
}


// Fonction récursive pour trouver tous les chemins d'un arbre binaire
CelluleList *findPaths (cellule2 *node, int path[2][8], int pathLength, CelluleList *listes)
{
    if (node == NULL) return ((CelluleList *)listes);

    // Ajouter le noeud actuel au chemin
    path[0][pathLength] = node->row;
    path[1][pathLength] = node->col;
    pathLength++;

    // Si le noeud actuel est une feuille, imprimer le chemin
    if (node->topleft == NULL && node->topright == NULL &&
        node->butleft == NULL && node->butright == NULL)
    {
        cellule7 *newList = NULL;
        for (int i = 0; i < pathLength; i++) newList = addToEndOfListe(newList, path[0][i], path[1][i]);
        listes = addList(listes, newList);
    }

    // Sinon, explorer les sous-arbres de manière récursive
    listes = findPaths(node->topleft, path, pathLength, listes);
    listes = findPaths(node->topright, path, pathLength, listes);
    listes = findPaths(node->butleft, path, pathLength, listes);
    listes = findPaths(node->butright, path, pathLength, listes);
    if (pathLength != 1) listes = findPaths(node->suiv, path, pathLength-1, listes);
    return ((CelluleList *)listes);
}



cellule5 *insert_into_cellule5_liste(cellule5 *list,cellule5 *ne)
{
    if(!list)return (cellule5*)ne;
    ne->suiv=list;
    list=ne;
    return (cellule5*)list;
}

cellule5 *generate_all_path(cellule2 **cl,int taille)
{
    cellule5 *list=NULL;
    int path[2][8];
    int pathLength = 0;
    cellule5 *ne;
    int i;
    for(i=0;i<taille;i++)
    {
        cellule2 *p;
        p=cl[i];
        while (p)
        {
            CelluleList *l1=NULL;
            l1=findPaths(p,path,pathLength,l1);
            CelluleList *p1;
            p1=l1;
            while (p1)//probleme ici
            {//
                ne= creer_intialiser_cellule5(p1->liste,i);
                list=insert_into_cellule5_liste(list,ne);
                p1=p1->suiv;
            }
            p=p->suiv;
        }

    }
    return (cellule5*)list;
}

int maximun_cellule5(cellule5 *ll)
{
    int max=ll->score;
    cellule5 *p;
    p=ll;
    while (p)
    {
        if(max<=p->score)max=p->score;
        p=p->suiv;
    }
    return (int)max;
}
int minimun_cellule5(cellule5 *ll)
{
    int min=ll->score;
    cellule5 *p;
    p=ll;
    while (p)
    {
        if(min>=p->score)min=p->score;
        p=p->suiv;
    }
    return (int)min;
}


cellule *recognise_fin_list(cellule *list,cellule *T[1])
{
    if(!list->suiv)
    {
        T[0]=list;
        return (cellule*)list;
    }
    cellule *p;
    p=list;
    while (p->suiv)p=p->suiv;
    T[0]=p;
    return (cellule*)list;
}

void transfert_cellule7_cellule(File *f,cellule7 *list)
{
    cellule *T[1];
    cellule7 *p2;
    p2=list;
    cellule *nn=NULL;
    while (p2)
    {
        nn=inserer_fin_liste_cellule(nn,p2->x,p2->y);
        p2=p2->suiv;
    }
    nn=recognise_fin_list(nn,T);
    f->tete=nn;
    f->queue=T[0];
}

void change_row_col_piece(etat_window *etw,ChksField *field,int lrow,int lcol,ChksOwner owner)
{
    int i;
    if(!etw)
    {
        switch (owner) {
            case CHKS_OWNER_ONE:
                for(i=0;i<field->view_table->user->nbr_piece_disp;i++)
                {
                    if(field->view_table->user->Tab[i].x==lrow &&
                       field->view_table->user->Tab[i].y==lcol)
                    {
                        field->view_table->user->Tab[i].x=field->row;
                        field->view_table->user->Tab[i].y=field->col;
                    }
                }
                break;
            case CHKS_OWNER_TWO:
                for(i=0;i<field->view_table->machine->nbr_piece_disp;i++)
                {
                    if(field->view_table->machine->Tab[i].x==lrow &&
                       field->view_table->machine->Tab[i].y==lcol)
                    {
                        field->view_table->machine->Tab[i].x=field->row;
                        field->view_table->machine->Tab[i].y=field->col;
                    }
                }
                break;
        }
    }else
    {
        switch (owner) {
            case CHKS_OWNER_ONE:
                for(i=0;i<etw->user->nbr_piece_disp;i++)
                {
                    if(etw->user->Tab[i].x==lrow &&
                       etw->user->Tab[i].y==lcol)
                    {
                        etw->user->Tab[i].x=field->row;
                        etw->user->Tab[i].y=field->col;
                    }
                }
                break;
            case CHKS_OWNER_TWO:
                for(i=0;i<etw->machine->nbr_piece_disp;i++)
                {
                    if(etw->machine->Tab[i].x==lrow &&
                       etw->machine->Tab[i].y==lcol)
                    {
                        etw->machine->Tab[i].x=field->row;
                        etw->machine->Tab[i].y=field->col;
                    }
                }
                break;
        }
    }
}

void freerecursif(cellule2 *ar)
{
    if (ar)
    {
        freerecursif(ar->topleft);
        freerecursif(ar->topright);
        freerecursif(ar->butleft);
        freerecursif(ar->butright);
        freerecursif(ar->suiv);
        free(ar->suiv);
    }
}


void freeallcl(cellule2 **cl,int taille)
{
    int i;
    for(i=0;i<taille;i++)
    {
        cellule2 *p;
        p=cl[i];
        freerecursif(p);
    }
}


void freell(cellule5 *ll)
{
    if(ll)
    {
        cellule7* cel = ll->liste, *tmp;
        while (cel)
        {
            tmp = cel;
            cel = cel->suiv;
            free(tmp);
        }
        freell(ll->suiv);
        free(ll);
    }
}


int Minmaxalgorithm(ChksOwner owner,int depth,etat_window *etw,bestpath *bs)
{
    int max,min,row,col,rowdest,coldest,i,j,cpt,cptv=0,cc=0;
    GtkStyleContext* context;
    if(depth==0)
        return (int)(etw->machine->nbr_piece_disp-etw->user->nbr_piece_disp);
    if(owner==CHKS_OWNER_ONE) //user
    {
        cellule2 **cl=NULL;
        cl=table_arbre_shemin(cl,etw,CHKS_OWNER_ONE);
        cellule5 *ll=NULL;
        ll=generate_all_path(cl,etw->user->nbr_piece_disp);
        freeallcl(cl,etw->user->nbr_piece_disp);
        cellule5 *p;
        p=ll;
        while (p)
        {
            etat_window *etw1=creer_intialiser_etat_window();
            copie_win_field1(etw,etw1);
            //effectuer le mouvement sur etw1
            File *f=creer_initaliser_file();
            transfert_cellule7_cellule(f,p->liste);
            ChksField *last;
            ChksField *field;
            row=etw1->user->Tab[p->numfield].x;
            col=etw1->user->Tab[p->numfield].y;
            last=etw1->filed[row-1][col-1];
            rowdest=f->queue->row;
            coldest=f->queue->col;
            field = etw1->filed[rowdest - 1][coldest - 1];
            if(f->tete!=f->queue) {//plusieurs deplacement
                drop_two_betwen_fl_lt1(etw1, f, CHKS_OWNER_TWO,row,col,rowdest,coldest);
            }
            else //un seul deplacement
            {
                drop_two_betwen_fl_lt1(etw1,NULL,CHKS_OWNER_TWO,row,col,rowdest,coldest);
            }
            if(rowdest==Nbrowocl)
            {
                field->is_queen=TRUE;
                last->is_queen=FALSE;
            }
            last->owner=CHKS_OWNER_NONE;
            field->owner=CHKS_OWNER_ONE;
            if(last->is_queen)
            {
                field->is_queen=TRUE;
                last->is_queen=FALSE;
            }
            change_row_col_piece(etw1,field,row,col,CHKS_OWNER_ONE);
            p->score=Minmaxalgorithm(CHKS_OWNER_TWO,depth-1,etw1,bs);
            free(etw1->user);
            free(etw1->machine);
            for(i=0;i<Nbrowocl;i++)
                for (j=0;j<Nbrowocl;j++)
                    free(etw1->filed[i][j]);
            free(etw1);
            p=p->suiv;
        }
        if(ll)
        {
            min=minimun_cellule5(ll);
            freell(ll);
        }
        else return (int)2222;//rien a deplacer pour user donc c'est le bon chemin a effectuer
        return (int)min;
    }else if(owner==CHKS_OWNER_TWO)//machine
    {
        cellule2 **cl=NULL;
        //creer la table des chemein de la machine sur l'etat actuelle ede la table
        cl=table_arbre_shemin(cl,etw,CHKS_OWNER_TWO);
        cellule5 *ll=NULL;
        //generer les chemin de chaque piece est les stocker dans une liste de liste
        ll=generate_all_path(cl,etw->machine->nbr_piece_disp);
        freeallcl(cl,etw->machine->nbr_piece_disp);
        cellule5 *p;
        p=ll;
        //boucler sur les chemin pour tout les piece et effectuer le mouvement
        while (p)
        {
            etat_window *etw1=creer_intialiser_etat_window();
            copie_win_field1(etw,etw1);
            //effectuer le mouvement sur etw1
            File *f=creer_initaliser_file();
            transfert_cellule7_cellule(f,p->liste);
            ChksField *last;
            ChksField *field;
            row=etw1->machine->Tab[p->numfield].x;//source row
            col=etw1->machine->Tab[p->numfield].y;//source col
            last=etw1->filed[row-1][col-1];
            rowdest=f->queue->row;//destination row
            coldest=f->queue->col;//destination col
            field = etw1->filed[rowdest - 1][coldest - 1];
            if(f->tete!=f->queue)
            {//plusieurs deplacement
                drop_two_betwen_fl_lt1(etw1, f, CHKS_OWNER_ONE,row,col,rowdest,coldest);
            }
            else //un seule deplacement
            {
                drop_two_betwen_fl_lt1(etw1,NULL,CHKS_OWNER_ONE,row,col,rowdest,coldest);
            }
            if(rowdest==1)
            {
                field->is_queen=TRUE;
                last->is_queen=FALSE;
            }
            last->owner=CHKS_OWNER_NONE;
            field->owner=CHKS_OWNER_TWO;
            if(last->is_queen)
            {
                field->is_queen=TRUE;
                last->is_queen=FALSE;
            }
            //last est la destination
            change_row_col_piece(etw1,field,row,col,CHKS_OWNER_TWO);
            p->score=Minmaxalgorithm(CHKS_OWNER_ONE,depth-1,etw1,bs);
            p=p->suiv;
            free(etw1->user);
            free(etw1->machine);
            for(i=0;i<Nbrowocl;i++)
                for (j=0;j<Nbrowocl;j++)
                    free(etw1->filed[i][j]);
            free(etw1);
        }
        if(ll)max=maximun_cellule5(ll);
        else
            return (int)-2222;//la machine ne peut pas effectuer un mouvement
        //il faut stocker le bon chemin dans best
        p=ll;
        while (p)
        {
            if(p->score==max)cc++;
            p=p->suiv;
        }
        time_t t;
        srand((unsigned) time(&t));
        cpt=rand()%cc+1;
        p=ll;
        while (p)
        {
            if(p->score==max)
            {
                cptv++;
                if(cptv==cpt) {
                    transfert_cellule7_cellule(bs->best, p->liste);
                    bs->numfield = p->numfield;
                    bs->score = p->score;
                    break;
                }
            }
            p=p->suiv;
        }
        freell(ll);
        return (int)max;
    }
}




int Minmaxalgorithm2(ChksOwner owner,int depth,etat_window *etw,bestpath *bs)
{
    int max,min,row,col,rowdest,coldest,i,j,cpt,cptv=0,cc=0;
    GtkStyleContext* context;
    if(depth==0)
        return (int)(etw->user->nbr_piece_disp-etw->machine->nbr_piece_disp);
    if(owner==CHKS_OWNER_TWO) //machine
    {
        cellule2 **cl=NULL;
        cl=table_arbre_shemin(cl,etw,CHKS_OWNER_TWO);
        cellule5 *ll=NULL;
        ll=generate_all_path(cl,etw->machine->nbr_piece_disp);
        freeallcl(cl,etw->machine->nbr_piece_disp);
        cellule5 *p;
        p=ll;
        while (p)
        {
            etat_window *etw1=creer_intialiser_etat_window();
            copie_win_field1(etw,etw1);
            //effectuer le mouvement sur etw1
            File *f=creer_initaliser_file();
            transfert_cellule7_cellule(f,p->liste);
            ChksField *last;
            ChksField *field;
            row=etw1->machine->Tab[p->numfield].x;
            col=etw1->machine->Tab[p->numfield].y;
            last=etw1->filed[row-1][col-1];
            rowdest=f->queue->row;
            coldest=f->queue->col;
            field = etw1->filed[rowdest - 1][coldest - 1];
            if(f->tete!=f->queue) {//plusieurs deplacement
                drop_two_betwen_fl_lt1(etw1, f, CHKS_OWNER_ONE,row,col,rowdest,coldest);
            }
            else //un seul deplacement
            {
                drop_two_betwen_fl_lt1(etw1,NULL,CHKS_OWNER_ONE,row,col,rowdest,coldest);
            }
            if(rowdest==1)
            {
                field->is_queen=TRUE;
                last->is_queen=FALSE;
            }
            last->owner=CHKS_OWNER_NONE;
            field->owner=CHKS_OWNER_TWO;
            if(last->is_queen)
            {
                field->is_queen=TRUE;
                last->is_queen=FALSE;
            }
            change_row_col_piece(etw1,field,row,col,CHKS_OWNER_TWO);
            p->score=Minmaxalgorithm2(CHKS_OWNER_ONE,depth-1,etw1,bs);
            p=p->suiv;
            free(etw1->user);
            free(etw1->machine);
            for(i=0;i<Nbrowocl;i++)
                for (j=0;j<Nbrowocl;j++)
                    free(etw1->filed[i][j]);
            free(etw1);
        }
        if(ll)
        {
            min=minimun_cellule5(ll);
            freell(ll);
        }
        else return (int)2222;//rien a deplacer pour user donc c'est le bon chemin a effectuer
        return (int)min;
    }else if(owner==CHKS_OWNER_ONE)//machine
    {
        cellule2 **cl=NULL;
        //creer la table des chemein de la machine sur l'etat actuelle ede la table
        cl=table_arbre_shemin(cl,etw,CHKS_OWNER_ONE);
        cellule5 *ll=NULL;
        //generer les chemin de chaque piece est les stocker dans une liste de liste
        ll=generate_all_path(cl,etw->user->nbr_piece_disp);
        freeallcl(cl,etw->user->nbr_piece_disp);
        cellule5 *p;
        p=ll;
        //boucler sur les chemin pour tout les piece et effectuer le mouvement
        while (p)
        {
            etat_window *etw1=creer_intialiser_etat_window();
            copie_win_field1(etw,etw1);
            //effectuer le mouvement sur etw1
            File *f=creer_initaliser_file();
            transfert_cellule7_cellule(f,p->liste);
            ChksField *last;
            ChksField *field;
            row=etw1->user->Tab[p->numfield].x;//source row
            col=etw1->user->Tab[p->numfield].y;//source col
            last=etw1->filed[row-1][col-1];
            rowdest=f->queue->row;//destination row
            coldest=f->queue->col;//destination col
            field = etw1->filed[rowdest - 1][coldest - 1];
            if(f->tete!=f->queue)
            {//plusieurs deplacement
                drop_two_betwen_fl_lt1(etw1, f, CHKS_OWNER_TWO,row,col,rowdest,coldest);
            }
            else //un seule deplacement
            {
                drop_two_betwen_fl_lt1(etw1,NULL,CHKS_OWNER_TWO,row,col,rowdest,coldest);
            }
            if(rowdest==Nbrowocl)
            {
                field->is_queen=TRUE;
                last->is_queen=FALSE;
            }
            last->owner=CHKS_OWNER_NONE;
            field->owner=CHKS_OWNER_ONE;
            if(last->is_queen)
            {
                field->is_queen=TRUE;
                last->is_queen=FALSE;
            }
            //last est la destination
            change_row_col_piece(etw1,field,row,col,CHKS_OWNER_ONE);
            p->score=Minmaxalgorithm2(CHKS_OWNER_TWO,depth-1,etw1,bs);
            p=p->suiv;
            free(etw1->user);
            free(etw1->machine);
            for(i=0;i<Nbrowocl;i++)
                for (j=0;j<Nbrowocl;j++)
                    free(etw1->filed[i][j]);
            free(etw1);
        }
        if(ll)max=maximun_cellule5(ll);
        else return (int)-2222;//la machine ne peut pas effectuer un mouvement
        //il faut stocker le bon chemin dans best
        p=ll;
        while (p)
        {
            if(p->score==max)cc++;
            p=p->suiv;
        }
        time_t t;
        srand((unsigned) time(&t));
        cpt=rand()%cc+1;
        p=ll;
        while (p)
        {
            if(p->score==max)
            {
                cptv++;
                if(cptv==cpt) {
                    transfert_cellule7_cellule(bs->best, p->liste);
                    bs->numfield = p->numfield;
                    bs->score = p->score;
                    break;
                }
            }
            p=p->suiv;
        }
        freell(ll);
        return (int)max;
    }
}

void arbre_descicion(Glob *g)//move two
{
    GtkStyleContext* context;
    cellule2 **cl;
    int row,col,rowdest,coldest,et;
    ChksField *last,*field;
    etat_window *etw=creer_intialiser_etat_window();
    copie_win_field(g->field->view_table,etw);
    bestpath *p=(bestpath*) malloc(sizeof (bestpath));
    p->best=creer_initaliser_file();
    et=Minmaxalgorithm(CHKS_OWNER_TWO,g->field->view_table->depth,etw,p);
    if(!p->best->tete)
    {
        //machine lose
        g->field->view_table->machine->nbr_piece_disp=0;
        return;
    }
    row=g->field->view_table->machine->Tab[p->numfield].x;
    col=g->field->view_table->machine->Tab[p->numfield].y;
    last=g->field->view_table->table->filed[row-1][col-1];
    rowdest=p->best->queue->row;
    coldest=p->best->queue->col;
    field = g->field->view_table->table->filed[rowdest - 1][coldest - 1];
    if(p->best->tete!=p->best->queue) {//plusieurs deplacement
        drop_two_betwen_fl_lt(last, field, p->best, CHKS_OWNER_ONE,1);
    }
    else //un seul deplacement
    {
        drop_two_betwen_fl_lt(last,field,NULL,CHKS_OWNER_ONE,1);
    }
    if(rowdest==1)
    {
        field->is_queen=TRUE;
        last->is_queen=FALSE;
    }
    last->owner=CHKS_OWNER_NONE;
    field->owner=CHKS_OWNER_TWO;
    if(last->is_queen)
    {
        field->is_queen=TRUE;
        last->is_queen=FALSE;
    }
    change_row_col_piece(NULL,field,last->row,
                         last->col,CHKS_OWNER_TWO);
    apply_style_button(last->view_table->table->filed);
    //sauvarder etat de window
    etat_window *etww=creer_intialiser_etat_window();
    copie_win_field(g->field->view_table,etww);
    CLpile *ne=creer_intialiser_Cpile(etww);
    g->field->view_table->pileback=empiler(g->field->view_table->pileback,ne);
    //user lose
    if(p->score==2222)
    {
        g->field->view_table->user->nbr_piece_disp=0;
    }
}
void arbre_descicion_one(Glob *g)//move one
{
    GtkStyleContext* context;
    cellule2 **cl;
    int row,col,rowdest,coldest,et;
    ChksField *last,*field;
    etat_window *etw=creer_intialiser_etat_window();
    copie_win_field(g->field->view_table,etw);
    bestpath *p=(bestpath*) malloc(sizeof (bestpath));
    p->best=creer_initaliser_file();
    et=Minmaxalgorithm2(CHKS_OWNER_ONE,g->field->view_table->depth,etw,p);
    if (!p->best->tete) {
        //machine lose
        g->field->view_table->user->nbr_piece_disp = 0;
        return;
    }
    row = g->field->view_table->user->Tab[p->numfield].x;
    col = g->field->view_table->user->Tab[p->numfield].y;
    last = g->field->view_table->table->filed[row - 1][col - 1];
    rowdest = p->best->queue->row;
    coldest = p->best->queue->col;
    field = g->field->view_table->table->filed[rowdest - 1][coldest - 1];
    if (p->best->tete != p->best->queue) {//plusieurs deplacement
        drop_two_betwen_fl_lt(last, field, p->best, CHKS_OWNER_TWO,1);
    } else //un seul deplacement
    {
        drop_two_betwen_fl_lt(last, field, NULL, CHKS_OWNER_TWO,1);
    }
    if (rowdest == Nbrowocl) {
        field->is_queen = TRUE;
        last->is_queen = FALSE;
    }
    last->owner = CHKS_OWNER_NONE;
    field->owner = CHKS_OWNER_ONE;
    if (last->is_queen) {
        field->is_queen = TRUE;
        last->is_queen = FALSE;
    }
    change_row_col_piece(NULL, field, last->row,
                         last->col, CHKS_OWNER_ONE);
    apply_style_button(g->field->view_table->table->filed);
    //sauvarder etat de window
    etat_window *etww=creer_intialiser_etat_window();
    copie_win_field(g->field->view_table,etww);
    CLpile *ne=creer_intialiser_Cpile(etww);
    g->field->view_table->pileback=empiler(g->field->view_table->pileback,ne);
    //user lose
    if(p->score==2222)
    {
        g->field->view_table->user->nbr_piece_disp=0;
    }
}



gboolean effectuer_moumvment(GtkWidget* self, GdkEventKey event, Glob *g)
{
    GtkStyleContext* context;
    if(event.keyval==GDK_KEY_e)
    {
        if(g->f->tete==NULL) return TRUE;
        if(g->f->tete==g->f->queue)//un seule deplacement
        {
            if(techek_entred_piece(g->field,g->field->view_table->table->last,g->f))
            {
                //deplacement normal , il faut soustraire les piece attraper et traiter le cas de is queen
                // and change style
                switch (g->field->view_table->table->last->owner) {
                    case CHKS_OWNER_ONE:
                        drop_two_betwen_fl_lt(g->field->view_table->table->last,g->field,NULL,CHKS_OWNER_TWO,1);
                        break;
                    case CHKS_OWNER_TWO:
                        drop_two_betwen_fl_lt(g->field->view_table->table->last,g->field,NULL,CHKS_OWNER_ONE,1);
                        break;
                }
            }else return (int)0;
        }else//plusierurs deplacement
        {
            File *ftmp=creer_initaliser_file();
            copy_file(g->f,ftmp);
            if(techek_entred_piece(g->field,g->field->view_table->table->last,g->f))//bon chemin
            {
                //deplacement long , il faut soustraire les piece attraper et traiter le cas de is queen
                switch (g->field->view_table->table->last->owner) {
                    case CHKS_OWNER_ONE:
                        drop_two_betwen_fl_lt(g->field->view_table->table->last,g->field,ftmp,CHKS_OWNER_TWO,0);
                        break;
                    case CHKS_OWNER_TWO:
                        drop_two_betwen_fl_lt(g->field->view_table->table->last,g->field,ftmp,CHKS_OWNER_ONE,0);
                        break;
                }
            }else return (int)0;//mauvais chemin
        }
        switch (g->field->view_table->table->last->owner) {
            case CHKS_OWNER_ONE:
                if(g->field->row==Nbrowocl)
                {
                    g->field->is_queen=TRUE;
                    g->field->view_table->table->last->is_queen=FALSE;
                }
                break;
            case CHKS_OWNER_TWO:
                if(g->field->row==1)
                {
                    g->field->is_queen=TRUE;
                    g->field->view_table->table->last->is_queen=FALSE;
                }
                break;
        }
        ChksOwner ow;
        ow=g->field->view_table->table->last->owner;
        g->field->owner=ow;
        g->field->view_table->table->last->owner=CHKS_OWNER_NONE;
        if(g->field->view_table->table->last->is_queen)
        {
            g->field->is_queen=TRUE;
            g->field->view_table->table->last->is_queen=FALSE;
        }
        apply_style_button(g->field->view_table->table->filed);
        //changer row and col de field dans user ,
        change_row_col_piece(NULL,g->field,g->field->view_table->table->last->row,
                             g->field->view_table->table->last->col,ow);
        g->field->view_table->table->last=NULL;

        //sauvarder etat de window
        etat_window *etww=creer_intialiser_etat_window();
        copie_win_field(g->field->view_table,etww);
        CLpile *ne=creer_intialiser_Cpile(etww);
        g->field->view_table->pileback=empiler(g->field->view_table->pileback,ne);


        int c1=g->field->view_table->combo2;
        if(!c1)//mode player vs boot
        {
            if(g->field->view_table->round==1)arbre_descicion(g);//move two
            else if(g->field->view_table->round==2)//move one
                arbre_descicion_one(g);
        }else if(c1==2)//mode player vs player
        {
            switch (g->field->view_table->round) {
                case 1:
                    g->field->view_table->round=2;
                    break;
                case 2:
                    g->field->view_table->round=1;
                    break;
            }
        }
        if(g->field->view_table->user->nbr_piece_disp==0)
        {
            gtk_header_bar_set_title(GTK_HEADER_BAR(g->field->view_table->header_bar),"TWO Win");
        }else if(g->field->view_table->machine->nbr_piece_disp==0) {
            gtk_header_bar_set_title(GTK_HEADER_BAR(g->field->view_table->header_bar), "ONE Win");
        }
        vider_file(g->f);
        g->field=NULL;
        //empiler l'etat de window
    }
    return (TRUE);
}

void add_filed_grid(Chkswindow *win,ChksField *fieled[rowcol][rowcol])
{
    GtkWidget *im;
    GtkAccelGroup *accel_group;
    accel_group = gtk_accel_group_new ();
    gtk_window_add_accel_group (GTK_WINDOW (win->win), accel_group);
    GtkStyleContext* context;
    GtkCssProvider* provider;
    context = gtk_widget_get_style_context(win->table->gr);
    provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(GTK_CSS_PROVIDER(provider),__STYLE__,NULL);
    gtk_style_context_add_class(GTK_STYLE_CONTEXT(context),"chks-grid");
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    gtk_grid_set_column_homogeneous(GTK_GRID(win->table->gr), TRUE);
    gtk_grid_set_row_homogeneous(GTK_GRID(win->table->gr), TRUE);
    int i,j;
    for(i=0;i<Nbrowocl;i++) {
        for (j = 0; j < Nbrowocl; j++)
        {
            //j is column and i is row
            fieled[i][j]->view_table = win;
            gtk_grid_attach(GTK_GRID(win->table->gr), fieled[i][j]->button, j, i, 1, 1);
            g_signal_connect(fieled[i][j]->button,"clicked",G_CALLBACK(piece_cliecked),fieled[i][j]);
            g_signal_connect(fieled[i][j]->button,"leave",G_CALLBACK(piece_leaved),fieled[i][j]);
            g_signal_connect(fieled[i][j]->button,"enter",G_CALLBACK(piece_entered),fieled[i][j]);
        }
    }

}


void init_table(Chkswindow *win)
{
    int i,j,k=0,m=0;
    GtkStyleContext* context;
    GtkCssProvider* provider;
    provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(GTK_CSS_PROVIDER(provider),__STYLE__,NULL);
    win->table->gr=gtk_grid_new();
    win->table->last=NULL;
    for (i=0;i<Nbrowocl;i++)
    {
        for(j=0;j<Nbrowocl;j++)
        {
            win->table->filed[i][j]=creer_field();
            win->table->filed[i][j]->view_table=NULL;
            win->table->filed[i][j]->button=gtk_button_new();
            context = gtk_widget_get_style_context(win->table->filed[i][j]->button);
            gtk_style_context_add_class(GTK_STYLE_CONTEXT(context),"chks-field");
            //pour taille de pion
            gtk_container_set_border_width(GTK_CONTAINER(win->table->filed[i][j]->button),7);
            gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider),
                                           GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
            win->table->filed[i][j]->is_queen=FALSE;
            win->table->filed[i][j]->row=i+1;
            win->table->filed[i][j]->col=j+1;
            if((win->user->Tab[k].x==(i+1)) &&
               (win->user->Tab[k].y==(j+1)))
            {
                win->table->filed[i][j]->owner=CHKS_OWNER_ONE;//user
                k++;
            }else if((win->machine->Tab[m].x==(i+1)) &&
                     (win->machine->Tab[m].y==(j+1)))
            {
                win->table->filed[i][j]->owner=CHKS_OWNER_TWO;//machine
                m++;
            }else win->table->filed[i][j]->owner=CHKS_OWNER_NONE;

        }
    }
    add_filed_grid(win,win->table->filed);
}

Chkswindow *creer_Chkswindow()
{
    Chkswindow *win=(Chkswindow*) malloc(sizeof (Chkswindow));
    if(!win)
    {
        printf("error allocation \n");
        exit(-1);
    }
    win->pileback=NULL;
    win->combo1=0;
    win->combo2=0;
    win->combo3=0;
    win->pileforward=NULL;
    win->table=creer_table();
    win->user=creer_player();
    win->machine=creer_player();
    return (Chkswindow*)win;
}

void afficher_table(Chkswindow *win)
{
    int i,j;
    for(i=0;i<Nbrowocl;i++)
    {
        for(j=0;j<Nbrowocl;j++)
        {
            switch (win->table->filed[i][j]->owner) {
                case CHKS_OWNER_ONE:
                    if(win->table->filed[i][j]->is_queen) printf("-1  ");
                    else printf("1  ");
                    break;
                case CHKS_OWNER_TWO:
                    if(win->table->filed[i][j]->is_queen) printf("-2  ");
                    else printf("2  ");
                    break;
                case CHKS_OWNER_NONE:
                    printf("0  ");
                    break;

            }
        }
        printf("\n");
    }
    fflush(stdout);
    printf("\n\n\n");
}


gboolean BotVsBot(Chkswindow *win)
{
    int row, col, rowdest, coldest;
    bestpath *p;
    ChksField *last, *field;
    etat_window *etw;
    if(win->machine->nbr_piece_disp==0 || win->user->nbr_piece_disp==0)
    {
        if(win->machine->nbr_piece_disp==0)
        {
            gtk_header_bar_set_title(GTK_HEADER_BAR(win->header_bar),"ONE Win");
        }else if(win->user->nbr_piece_disp==0)
        {
            gtk_header_bar_set_title(GTK_HEADER_BAR(win->header_bar), "TWO Win");
        }
        return FALSE;
    }
    if(win->round==1)
    {
        win->round=2;
        etw = creer_intialiser_etat_window();
        copie_win_field(win, etw);
        p = (bestpath *) malloc(sizeof(bestpath));
        p->best = creer_initaliser_file();
        Minmaxalgorithm2(CHKS_OWNER_ONE, win->depth, etw, p);
        if (!p->best->tete) {
            //machine lose
            win->user->nbr_piece_disp = 0;
            return TRUE;
        }
        row = win->user->Tab[p->numfield].x;
        col = win->user->Tab[p->numfield].y;
        last = win->table->filed[row - 1][col - 1];
        rowdest = p->best->queue->row;
        coldest = p->best->queue->col;
        field = win->table->filed[rowdest - 1][coldest - 1];
        if (p->best->tete != p->best->queue) {//plusieurs deplacement
            drop_two_betwen_fl_lt(last, field, p->best, CHKS_OWNER_TWO,1);
        } else //un seul deplacement
        {
            drop_two_betwen_fl_lt(last, field, NULL, CHKS_OWNER_TWO,1);
        }
        if (rowdest == Nbrowocl) {
            field->is_queen = TRUE;
            last->is_queen = FALSE;
        }
        last->owner = CHKS_OWNER_NONE;
        field->owner = CHKS_OWNER_ONE;
        if (last->is_queen) {
            field->is_queen = TRUE;
            last->is_queen = FALSE;
        }
        change_row_col_piece(NULL, field, last->row,
                             last->col, CHKS_OWNER_ONE);
        apply_style_button(win->table->filed);
    }else
    {
        win->round=1;
        etw = creer_intialiser_etat_window();
        copie_win_field(win, etw);
        p = (bestpath *) malloc(sizeof(bestpath));
        p->best = creer_initaliser_file();
        Minmaxalgorithm(CHKS_OWNER_TWO, win->depth, etw, p);
        if (!p->best->tete) {
            //machine lose
            win->machine->nbr_piece_disp = 0;
            return TRUE;
        }
        row = win->machine->Tab[p->numfield].x;
        col = win->machine->Tab[p->numfield].y;
        last = win->table->filed[row - 1][col - 1];
        rowdest = p->best->queue->row;
        coldest = p->best->queue->col;
        field = win->table->filed[rowdest - 1][coldest - 1];
        if (p->best->tete != p->best->queue) {//plusieurs deplacement
            drop_two_betwen_fl_lt(last, field, p->best, CHKS_OWNER_ONE,1);
        } else //un seul deplacement
        {
            drop_two_betwen_fl_lt(last, field, NULL, CHKS_OWNER_ONE,1);
        }
        if (rowdest == 1) {
            field->is_queen = TRUE;
            last->is_queen = FALSE;
        }
        last->owner = CHKS_OWNER_NONE;
        field->owner = CHKS_OWNER_TWO;
        if (last->is_queen) {
            field->is_queen = TRUE;
            last->is_queen = FALSE;
        }
        change_row_col_piece(NULL, field, last->row,
                             last->col, CHKS_OWNER_TWO);
        apply_style_button(win->table->filed);
    }

//    afficher_table(win);

    return TRUE;

}



void reinitialiser_window(Chkswindow *win)
{
    int i,j,k=0,m=0;
//    gtk_header_bar_set_title(GTK_HEADER_BAR(win->header_bar),"Checkers");
    //init player il le met dans les positions initiales
    gtk_label_set_label(GTK_LABEL(win->labelOne),"0");
    gtk_label_set_label(GTK_LABEL(win->labelTwo),"0");
    if(win->prp)
    {
        for(i=0;i<Nbrowocl;i++)
        {
            for (j=0;j<Nbrowocl;j++)
            {
                win->table->filed[i][j]->row=i+1;
                win->table->filed[i][j]->col=j+1;
                switch (win->matrice[i][j]) {
                    case 1:
                        win->table->filed[i][j]->is_queen=FALSE;
                        win->user->nbr_piece_disp++;
                        win->user->Tab[k].x=i+1;
                        win->user->Tab[k].y=j+1;
                        win->table->filed[i][j]->owner=CHKS_OWNER_ONE;//user
                        k++;
                        break;
                    case -1:
                        win->table->filed[i][j]->is_queen=TRUE;
                        win->user->nbr_piece_disp++;
                        win->user->Tab[k].x=i+1;
                        win->user->Tab[k].y=j+1;
                        win->table->filed[i][j]->owner=CHKS_OWNER_ONE;//user
                        k++;
                        break;
                    case 2:
                        win->table->filed[i][j]->is_queen=FALSE;
                        win->machine->nbr_piece_disp++;
                        win->machine->Tab[m].x=i+1;
                        win->machine->Tab[m].y=j+1;
                        win->table->filed[i][j]->owner=CHKS_OWNER_TWO;//user
                        m++;
                        break;
                    case -2:
                        win->table->filed[i][j]->is_queen=TRUE;
                        win->machine->nbr_piece_disp++;
                        win->machine->Tab[m].x=i+1;
                        win->machine->Tab[m].y=j+1;
                        win->table->filed[i][j]->owner=CHKS_OWNER_TWO;//user
                        m++;
                        break;
                    default:
                        win->table->filed[i][j]->owner=CHKS_OWNER_NONE;//user
                        win->table->filed[i][j]->is_queen=FALSE;
                }
            }
        }
    }else{
        init_player(win->user,CHKS_OWNER_ONE);
        init_player(win->machine,CHKS_OWNER_TWO);
        for (i=0;i<Nbrowocl;i++)
        {
            for(j=0;j<Nbrowocl;j++)
            {
                win->table->filed[i][j]->is_queen=FALSE;
                win->table->filed[i][j]->row=i+1;
                win->table->filed[i][j]->col=j+1;
                if((win->user->Tab[k].x==(i+1)) &&
                   (win->user->Tab[k].y==(j+1)))
                {
                    win->table->filed[i][j]->owner=CHKS_OWNER_ONE;//user
                    k++;
                }else if((win->machine->Tab[m].x==(i+1)) &&
                         (win->machine->Tab[m].y==(j+1)))
                {
                    win->table->filed[i][j]->owner=CHKS_OWNER_TWO;//machine
                    m++;
                }else
                    win->table->filed[i][j]->owner=CHKS_OWNER_NONE;
            }
        }
    }
    apply_style_button(win->table->filed);
}


void Myfonct(GtkWidget *button,Chkswindow *win)
{
    reinitialiser_window(win);
    win->pileforward=NULL;
    win->pileback=NULL;
    int c1,c2;

    //lire depth
    switch (win->combo1) {
        case 0:
            win->depth=6;
            break;
        case 1:
            win->depth=4;
            break;
        case 2:
            win->depth=2;
            break;
    }
    //lire mode
    switch (win->combo2) {
        case 1:
            g_timeout_add(1000 * 4, G_SOURCE_FUNC(BotVsBot),win);
    }

}

void transfer_copie_to_window(etat_window *etw,Chkswindow *win)
{
    int i,j;
    for(i=0;i<Nbrowocl;i++)
    {
        for(j=0;j<Nbrowocl;j++)
        {
            win->table->filed[i][j]->row=etw->filed[i][j]->row;
            win->table->filed[i][j]->col=etw->filed[i][j]->col;
            win->table->filed[i][j]->is_queen=etw->filed[i][j]->is_queen;
            win->table->filed[i][j]->owner=etw->filed[i][j]->owner;
        }
    }
    win->machine->nbr_piece_disp=etw->machine->nbr_piece_disp;
    win->user->nbr_piece_disp=etw->user->nbr_piece_disp;
    for(i=0;i<etw->machine->nbr_piece_disp;i++) {
        win->machine->Tab[i].x = etw->machine->Tab[i].x;
        win->machine->Tab[i].y = etw->machine->Tab[i].y;
    }
    for(i=0;i<etw->user->nbr_piece_disp;i++) {
        win->user->Tab[i].x = etw->user->Tab[i].x;
        win->user->Tab[i].y = etw->user->Tab[i].y;
    }
}

void backfunction(GtkWidget *button,Chkswindow *win)
{
    CLpile *T[1]={0};
    win->pileback= depiler(win->pileback,T);
    if(T[0])win->pileforward= empiler(win->pileforward,T[0]);
    if(win->pileback)
    {
        transfer_copie_to_window(win->pileback->wintmp,win);
        apply_style_button(win->table->filed);
    }
    else
    {
        reinitialiser_window(win);
        win->pileback=NULL;
    }
}

void overfunction(GtkWidget *button,Chkswindow *win)
{
    CLpile *T[1]={0};
    win->pileforward= depiler(win->pileforward,T);
    if(!T[0])return;
    win->pileback= empiler(win->pileback,T[0]);
    transfer_copie_to_window(win->pileback->wintmp,win);
    apply_style_button(win->table->filed);
}

void pause_function(GtkWidget *button,Chkswindow *win);
void continue_function(GtkWidget *button,Chkswindow *win)
{
    win->pause = 0;
    gtk_button_set_label(GTK_BUTTON(button), "Pause");
    g_signal_connect(button, "clicked", G_CALLBACK(pause_function), win);
}

void pause_function(GtkWidget *button,Chkswindow *win)
{
    win->pause = 1;
    gtk_button_set_label(GTK_BUTTON(button), "Continue");
    g_signal_connect(button, "clicked", G_CALLBACK(continue_function), win);

}

GtkWidget *Make_headerbar(Chkswindow *win)
{
    GtkWidget *hd;
    GtkWidget *btn,*btn1,*btn2, *btn3;
    hd=gtk_header_bar_new();
    btn= gtk_button_new_with_label("New");
    btn1= gtk_button_new_with_label("<");
    btn2= gtk_button_new_with_label(">");
    btn3= gtk_button_new_with_label("Pause");

    GtkWidget *labelOne= gtk_label_new("One :");
    GtkWidget *labelTwo= gtk_label_new("Two :");
    win->labelOne= gtk_label_new("0");
    win->labelTwo= gtk_label_new("0");
    Myfonct(btn,win);
    g_signal_connect(btn,"clicked",G_CALLBACK(Myfonct),win);
    g_signal_connect(btn1,"clicked",G_CALLBACK(backfunction),win);
    g_signal_connect(btn2,"clicked",G_CALLBACK(overfunction),win);
    g_signal_connect(btn3,"clicked",G_CALLBACK(pause_function),win);


//    GtkStyleContext* context = gtk_widget_get_style_context(btn);
//    gtk_style_context_add_class(GTK_STYLE_CONTEXT(context),"Newgamebtn");
//    GtkWidget *combo1=gtk_combo_box_text_new ();
//    gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT(combo1),"PLa vs BOOT");
//    gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT(combo1),"BOOT vs BOOT");
//    gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT(combo1),"PLa vs PLa");
//    gtk_combo_box_set_active(GTK_COMBO_BOX(combo1),0);
//
//    GtkWidget *combo2=gtk_combo_box_text_new ();
//    gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT(combo2),"Easy");
//    gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT(combo2),"Medium");
//    gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT(combo2),"Hard");
//    gtk_combo_box_set_active(GTK_COMBO_BOX(combo2),0);
//    GtkWidget *combo3=gtk_combo_box_text_new ();
//    //pour designer qui va commencer
//    gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT(combo3),"ONE");
//    gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT(combo3),"TWO");
//    gtk_combo_box_set_active(GTK_COMBO_BOX(combo3),0);
//    gtk_header_bar_pack_start(GTK_HEADER_BAR(hd),combo1);
//    gtk_header_bar_pack_start(GTK_HEADER_BAR(hd),combo2);
////    gtk_header_bar_pack_start(GTK_HEADER_BAR(hd),combo3);

    gtk_header_bar_pack_start(GTK_HEADER_BAR(hd),labelOne);
    gtk_header_bar_pack_start(GTK_HEADER_BAR(hd),win->labelOne);

    gtk_header_bar_pack_start(GTK_HEADER_BAR(hd),labelTwo);
    gtk_header_bar_pack_start(GTK_HEADER_BAR(hd),win->labelTwo);


    gtk_header_bar_pack_end(GTK_HEADER_BAR(hd),btn);
    gtk_header_bar_pack_end(GTK_HEADER_BAR(hd),btn3);
    gtk_header_bar_pack_end(GTK_HEADER_BAR(hd),btn2);
    gtk_header_bar_pack_end(GTK_HEADER_BAR(hd),btn1);
    gtk_header_bar_set_title(GTK_HEADER_BAR(hd),"Checkers");
    gtk_header_bar_set_decoration_layout (GTK_HEADER_BAR(hd),"menu:close");
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(hd),TRUE);
//    win->combo1=combo1;
//    win->combo2=combo2;
//    win->combo3=combo3;
    return (GtkWidget*)hd;
}

void init_player(ChkPlayer *pl, ChksOwner owner)
{
    pl->nbr_piece_disp=0;
    int i,j,k=0;
    switch (owner) {
        //si il s'agit de one je le mais en haut
        case CHKS_OWNER_ONE:
            for (i=1;i<=(Nbrowocl/2)-1;i++)
            {
                if(i%2!=0)//impair
                {
                    for (j=2;j<=Nbrowocl;j+=2)
                    {
                        pl->Tab[k].x=i;
                        pl->Tab[k].y=j;
                        k++;
                        pl->nbr_piece_disp++;
                    }
                }else{//pair
                    for (j=1;j<Nbrowocl;j+=2)
                    {
                        pl->Tab[k].x=i;
                        pl->Tab[k].y=j;
                        k++;
                        pl->nbr_piece_disp++;
                    }
                }
            }
            break;
            //si il s'agit de two je le mais en bas
        case CHKS_OWNER_TWO:
            for (i=(Nbrowocl/2)+2;i<=Nbrowocl;i++)
            {
                if(i%2!=0)
                {
                    for (j=2;j<=Nbrowocl;j+=2)//sortant
                    {
                        pl->Tab[k].x=i;
                        pl->Tab[k].y=j;
                        k++;
                        pl->nbr_piece_disp++;
                    }
                }else
                {
                    for (j=1;j<Nbrowocl;j+=2)//entrant
                    {
                        pl->Tab[k].x=i;
                        pl->Tab[k].y=j;
                        k++;
                        pl->nbr_piece_disp++;
                    }
                }
            }
            break;
    }
}



GtkWidget *Principale_fonction11(GtkWidget *btn,Chkswindow *win)
{
    //770 and 820 for 10*10
    //770 and 820 for 10*10
//    int ff=Nbrowocl;
//    int ff4=Nbrpiece;
    win->combo1=gtk_combo_box_get_active(GTK_COMBO_BOX(cm));//depth
    win->combo2=gtk_combo_box_get_active(GTK_COMBO_BOX(cmm));//mode
    win->combo3=gtk_combo_box_get_active(GTK_COMBO_BOX(cm7));;//qui commence

    gtk_widget_destroy(win3);

    win->win= gtk_window_new(GTK_WINDOW_TOPLEVEL);
    if(Nbrowocl==8)gtk_window_set_default_size(GTK_WINDOW(win->win),575,580);
    else if(Nbrowocl==10)gtk_window_set_default_size(GTK_WINDOW(win->win),720,720);
//    gtk_window_set_default_size(GTK_WINDOW(win->win),570,580);
    gtk_window_move(GTK_WINDOW(win->win),2000,100);
    g_signal_connect(win->win,"destroy",G_CALLBACK(gtk_main_quit),NULL);
    win->g=(Glob*)malloc(sizeof (Glob));
    win->g->f=creer_initaliser_file();
    win->g->field=NULL;
    init_table(win);
    gtk_container_add(GTK_CONTAINER(win->win),win->table->gr);
    //lire qui va commencer
    switch (win->combo3) {
        case 0:
            win->round=2;//two :qui vas commencer
            break;
        case 1:
            win->round=1;//one :qui vas commencer
            break;
    }
    //lire si la table est personnalisé
    if(win->prp)//table personnalise
    {
        int i,j;
        int mm[rowcol][rowcol]={
                {0,0,0,1,0,0,0,0},
                {0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0},
                {0,0,0,-2,0,0,0,0},
                {0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0},
                {0,0,0,0,0,0,0,0},

        };
        for(i=0;i<Nbrowocl;i++)
            for(j=0;j<Nbrowocl;j++)
                win->matrice[i][j]=mm[i][j];
    }

    win->header_bar=Make_headerbar(win);
    gtk_window_set_titlebar(GTK_WINDOW(win->win),win->header_bar);
    g_signal_connect(win->win,"key-press-event",G_CALLBACK(effectuer_moumvment),win->g);
    gtk_widget_show_all(win->win);
}



void radio_clicked(GtkWidget *radio,gpointer data)
{
    int val= atoi(data);
    int ff=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio));
    if(ff)
    {
        if(val==12)//nombre piece
        {
            Nbrpiece=12;
        }else if(val==20)//nombre piece
        {
            Nbrpiece=20;
        }else if(val==10)//row * col
        {
            Nbrowocl=10;
        }else if(val==8)//row * col
        {
            Nbrowocl=8;
        }
    }
}


//creer premiere interface pour la saisie des information

void window_principale()
{
    //label Mode
    label *stlbbb=crer_initialiser_label();
    remplir_info(stlbbb->label_name,"Mode");
    stlbbb->xalign=0;
    make_label(stlbbb);


    //make combo text MODE
    combo_box_text *cm11=creer_initaliser_combo_box_text();
    remplir_info(cm11->text,"Player vs Player");
    combo_box_text *cm22=creer_initaliser_combo_box_text();
    remplir_info(cm22->text,"BOOT  vs BOOT");
    combo_box_text *cm33=creer_initaliser_combo_box_text();
    remplir_info(cm33->text,"Player vs BOOT");
    cm22->suiv=cm11;
    cm33->suiv=cm22;
    cmm= make_combo_box_text(cm33);
    gtk_combo_box_set_active(GTK_COMBO_BOX(cmm),0);

    //make box hor mode
    box *stgbhhh=creer_initialiser_box();
    remplir_info(stgbhhh->bord,"* { border: black solid 2px;}");
    stgbhhh->orientation=GTK_ORIENTATION_HORIZONTAL;
    stgbhhh->spacing=10;
    make_box(stgbhhh);
    gtk_box_pack_start(GTK_BOX(stgbhhh->box1),stlbbb->label,FALSE,TRUE,0);
    gtk_box_pack_start(GTK_BOX(stgbhhh->box1),cmm,FALSE,TRUE,0);


    //label  depth
    label *stlbb=crer_initialiser_label();
    remplir_info(stlbb->label_name,"Depth");
    stlbb->xalign=0;
    make_label(stlbb);

    //make combo text DEPTH
    combo_box_text *cm1=creer_initaliser_combo_box_text();
    remplir_info(cm1->text,"Easy");
    combo_box_text *cm2=creer_initaliser_combo_box_text();
    remplir_info(cm2->text,"Medium");
    combo_box_text *cm3=creer_initaliser_combo_box_text();
    remplir_info(cm3->text,"Hard");
    cm2->suiv=cm1;
    cm3->suiv=cm2;
    cm= make_combo_box_text(cm3);
    gtk_combo_box_set_active(GTK_COMBO_BOX(cm),0);

    //make box hor depth
    box *stgbhh=creer_initialiser_box();
    remplir_info(stgbhh->bord,"* { border: black solid 2px;}");
    stgbhh->orientation=GTK_ORIENTATION_HORIZONTAL;
    stgbhh->spacing=10;
    make_box(stgbhh);
    gtk_box_pack_start(GTK_BOX(stgbhh->box1),stlbb->label,FALSE,TRUE,0);
    gtk_box_pack_start(GTK_BOX(stgbhh->box1),cm,FALSE,TRUE,0);

    //label Nombre row col
    label *stlb=crer_initialiser_label();
    remplir_info(stlb->label_name,"Nombre colonne");
    stlb->xalign=0;
    make_label(stlb);

    //nombre Nombre row col
    GSList *group=NULL;
    GSList *group3=NULL;
    radio_button *rd=creer_initialiser_radio_button();
    remplir_info(rd->label,"10");
    make_radio_button(rd,group);
    g_signal_connect(rd->radio_button,"toggled",G_CALLBACK(radio_clicked),rd->label->info);
    group3= gtk_radio_button_get_group(GTK_RADIO_BUTTON(rd->radio_button));
    radio_button *rd1=creer_initialiser_radio_button();
    remplir_info(rd1->label,"8");
    make_radio_button(rd1,group3);
    g_signal_connect(rd1->radio_button,"toggled",G_CALLBACK(radio_clicked),rd1->label->info);



    //make box hor  Nombre row col

    box *stgbh=creer_initialiser_box();
    remplir_info(stgbh->bord,"* { border: black solid 2px;}");
    stgbh->orientation=GTK_ORIENTATION_HORIZONTAL;
    stgbh->spacing=10;
    make_box(stgbh);
    gtk_box_pack_start(GTK_BOX(stgbh->box1),stlb->label,FALSE,TRUE,0);
    gtk_box_pack_start(GTK_BOX(stgbh->box1),rd->radio_button,FALSE,TRUE,0);
    gtk_box_pack_start(GTK_BOX(stgbh->box1),rd1->radio_button,FALSE,TRUE,0);


    //label Nombre piece
    label *stlbb2=crer_initialiser_label();
    remplir_info(stlbb2->label_name,"Nombre de pieces");
    stlbb2->xalign=0;
    make_label(stlbb2);

    //nombre piece
    GSList *group1=NULL;
    GSList *group5=NULL;
    radio_button *rd2=creer_initialiser_radio_button();
    remplir_info(rd2->label,"12");
    make_radio_button(rd2,group);
    g_signal_connect(rd2->radio_button,"toggled",G_CALLBACK(radio_clicked),rd2->label->info);
    group5= gtk_radio_button_get_group(GTK_RADIO_BUTTON(rd2->radio_button));
    radio_button *rd3=creer_initialiser_radio_button();
    remplir_info(rd3->label,"20");
    make_radio_button(rd3,group5);
    g_signal_connect(rd3->radio_button,"toggled",G_CALLBACK(radio_clicked),rd3->label->info);


    //make box hor  Nombre piece

    box *stgbh5=creer_initialiser_box();
    remplir_info(stgbh5->bord,"* { border: black solid 2px;}");
    stgbh5->orientation=GTK_ORIENTATION_HORIZONTAL;
    stgbh5->spacing=10;
    make_box(stgbh5);
    gtk_box_pack_start(GTK_BOX(stgbh5->box1),stlbb2->label,FALSE,TRUE,0);
    gtk_box_pack_start(GTK_BOX(stgbh5->box1),rd2->radio_button,FALSE,TRUE,0);
    gtk_box_pack_start(GTK_BOX(stgbh5->box1),rd3->radio_button,FALSE,TRUE,0);

    //label  qui commence
    label *stlbb5=crer_initialiser_label();
    remplir_info(stlbb5->label_name,"QUI Commence");
    stlbb5->xalign=0;
    make_label(stlbb5);

    //make combo text qui commence
    combo_box_text *cm115=creer_initaliser_combo_box_text();
    remplir_info(cm115->text,"ONE");
    combo_box_text *cm225=creer_initaliser_combo_box_text();
    remplir_info(cm225->text,"TWO");
    cm225->suiv=cm115;
    cm7= make_combo_box_text(cm225);
    gtk_combo_box_set_active(GTK_COMBO_BOX(cm7),0);

    //make box hor qui commence
    box *stgbhhhh9=creer_initialiser_box();
    remplir_info(stgbhhhh9->bord,"* { border: black solid 2px;}");
    stgbhhhh9->orientation=GTK_ORIENTATION_HORIZONTAL;
    stgbhhhh9->spacing=10;
    make_box(stgbhhhh9);
    gtk_box_pack_start(GTK_BOX(stgbhhhh9->box1),stlbb5->label,FALSE,TRUE,0);
    gtk_box_pack_start(GTK_BOX(stgbhhhh9->box1),cm7,FALSE,TRUE,0);

    //button new game
    Chkswindow *win=creer_Chkswindow();
    win->prp=0;

    GtkWidget *btn= gtk_button_new_with_label("New game");
    g_signal_connect(btn,"clicked",G_CALLBACK(Principale_fonction11),win);

    //crer button new game when i click i call princiaple function

    GtkWidget *fix= gtk_fixed_new();
    gtk_fixed_put(GTK_FIXED(fix),btn,0,200);
    gtk_fixed_put(GTK_FIXED(fix),stgbhhhh9->box1,200,50);
    gtk_fixed_put(GTK_FIXED(fix),stgbh5->box1,200,100);
    gtk_fixed_put(GTK_FIXED(fix),stgbh->box1,200,150);
    gtk_fixed_put(GTK_FIXED(fix),stgbhh->box1,200,200);
    gtk_fixed_put(GTK_FIXED(fix),stgbhhh->box1,200,300);

    win3= gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(win3),400,400);
    gtk_container_add(GTK_CONTAINER(win3),fix);

    gtk_widget_show_all(win3);

}


int main(int argc,char *argv[])
{
    gtk_init(&argc, &argv);
    window_principale();
    gtk_main();
    return 0;
}