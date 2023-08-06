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

int vider_file(File *f)
{
    if(!f)return (int)-1;
    f->tete=NULL;
    f->queue=NULL;
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

cellule2 *inserer_debut(cellule2 *list,cellule2 *ne)
{
    if(!list)return (cellule2*)ne;
    ne->suiv=list;
    list=ne;
    return (cellule2*)list;
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

cellule7 *addToEndOfListe (cellule7 *liste, int x, int y)
{
    cellule7 *ptr = liste;
    if (!liste) return ((cellule7 *)createCellule1(x, y));
    while (ptr->suiv) ptr = ptr->suiv;
    ptr->suiv = createCellule1(x, y);
    return ((cellule7 *)liste);
}

CelluleList *addList (CelluleList *listes, cellule7 *liste)
{
    CelluleList *newList = createCelluleList(liste);
    if (listes) newList->suiv = listes;
    return ((CelluleList *)newList);
}

cellule5 *insert_into_cellule5_liste(cellule5 *list,cellule5 *ne)
{
    if(!list)return (cellule5*)ne;
    ne->suiv=list;
    list=ne;
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
