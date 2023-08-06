
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

cellule7 *inserer_fin_liste_cellule7(cellule7 *liste,cellule7 *ne1);
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

CelluleList *createCelluleList (cellule7 *liste)
{
    CelluleList *newCellList = (CelluleList *)malloc(sizeof(CelluleList));
    if (!newCellList) exit(0);
    newCellList->liste = liste;
    newCellList->suiv = NULL;
    return ((CelluleList *)newCellList);
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

