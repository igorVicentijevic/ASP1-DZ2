#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_STRLEN 100
#define MAX_STEK_LEN 100000
#define MAX_RED_LEN 100000
#define MAX_POVEZANI_SIZE 50

int m,n;
int brPovezanih;
char ***glavnaTabela;

int nextID = 0;

FILE *fajl;



//region Stek
struct stek{
    struct cvor* stek[MAX_STEK_LEN];
    int top;
} stek;

void InicijalizujStek()
{
    stek.top = 0;
}

void Push(struct cvor *cvor)
{
    stek.stek[stek.top++] = cvor;
}

bool isEmpty()
{
    return stek.top == 0;
}

struct cvor* Pop()
{
    if(stek.top == 0)
        return NULL;
    return stek.stek[--stek.top];
}
//endregion



bool ProveriAlokaciju(void* a)
{
    if (a==NULL)
        printf("NEUSPESNA ALOKACIJA!");
    return a!=NULL;
}

char*** InicijalizujTabelu()
{
    char ***tabela;
    tabela = malloc(sizeof(char**)*m);
    ProveriAlokaciju(tabela);
    for (int i = 0; i < m; ++i) {
        tabela[i] = malloc(sizeof(char*)*n);
        ProveriAlokaciju(tabela);
    }
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < n; ++j)
            tabela[i][j] = NULL;

    return tabela;
}
void IspisiPodatkeIzTabele(char*** tabela)
{
    printf("####################\n");
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) printf("%s ",tabela[i][j] != NULL ? tabela[i][j] : "NULL");
        putchar('\n');
    }
    printf("####################\n");
}

//region Stablo

struct cvor {
    char ***tabela;
    bool jeValidan;
    int id;
    int nivo;

    struct cvor* kraj;
    struct cvor* levi;
    struct cvor* desni;
} ;

struct cvor *stablo;

void IspisiCvor(struct cvor* cvor)
{
    IspisiPodatkeIzTabele(cvor -> tabela);
}

struct cvor* NapraviCvor(char ***tabela)
{
    struct cvor* t = malloc(sizeof(struct cvor));
    t -> tabela = tabela;
    t -> levi = NULL;
    t -> desni = NULL;
    t -> kraj = NULL;
    t -> id = nextID++;
    t -> jeValidan = true;
    t -> nivo = -1;
    return t;
}

void DodajCvor(struct cvor* noviCvor, struct cvor* parent)
{
    if(parent -> levi == NULL && parent -> jeValidan == true)
    {
        parent -> levi = noviCvor;
    }
    else
    {
        parent -> kraj -> desni = noviCvor;
    }
    parent -> kraj = noviCvor;
}

void InicijalizujStablo(char*** tabela)
{
    stablo = NapraviCvor(tabela);

}
//endregion


//region Fajl

void IspisiTabeluUFajl(FILE *_fajl,char ***tabela)
{
    if (_fajl == NULL)
        _fajl = fopen("izlaz.txt","w");

    fprintf(_fajl,"####################\n");
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) fprintf(_fajl,"%s ",tabela[i][j] != NULL ? tabela[i][j] : "NULL");
        putc('\n',_fajl);
    }
    fprintf(_fajl,"####################\n");
}

void IspisiUFajl(struct cvor* cvor)
{
    IspisiTabeluUFajl(fajl,cvor -> tabela);
}

void IspisiIdUFajl(FILE *_fajl,int id)
{
    if(_fajl == NULL)
        _fajl = fopen("izlaz.txt","w");
    fprintf(_fajl,"%d\n",id);
}

//endregion

struct upareni{
    char *clan1;
    char *clan2;
    char odnos;
    int redClan1;
    int redClan2;
};

//region UlancanaLista
struct node{
    struct upareni *info;
    struct node* next;
};

struct lista{
    struct node* first;
    struct node* kraj;
};

struct lista* Add(struct upareni *x,struct lista* list)
{
    struct node* t = malloc(sizeof (struct node));
    t -> info = x;
    t -> next = NULL;
    if(list == NULL)
    {
        list = malloc(sizeof(struct lista));
        list -> kraj = t;
        list -> first = t;
        return list;
    }
    list -> kraj -> next = t;
    list -> kraj = t;
    return list;
}

struct lista* Remove(struct upareni *x,struct lista* list)
{
    if (list == NULL || list -> first == NULL )
        return list;

    struct node* p =list -> first;
    struct node* q = NULL;
    bool nadjen = false;
    while(p!=NULL) {
        if(p -> info == x)
        {
            nadjen = true;
            break;
        }
        q = p;
        p = p->next;
    }

    if(!nadjen) return list;


    if (q != NULL) {
        q->next = p->next;
        if (q->next == NULL)
            list->kraj = q;
    }
    else
    {
        list -> first = p -> next;
    }

    free(p);
    return list;
}


void DealocirajListu(struct lista* list)
{
    if (list -> first ==NULL)
    {
        free(list);
        return;
    }
    struct node *t = list -> first;
    struct node *p =t -> next;

    while(p != NULL)
    {
        free(t);
        t = p;
        p = p -> next;
    }

    free(t);
    free(list);
}
//endregion

struct lista *povezani;

int NadjiRed(char* x)
{
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            {
                if(glavnaTabela[i][j] == NULL)
                    continue;
                 if(strcmp(x,glavnaTabela[i][j]) == 0)
                    return i;
            }
        }
    }
    return -1;
}
void UcitajUparenost()
{
    char str[MAX_STRLEN] = "\0";
    char delim[3] = "+-";

    povezani = NULL;

    int len = 0;
    char c;

    while (1)
    {
        struct upareni *t = malloc(sizeof(struct upareni));

        int j = 0;
        char znak = '+';
        while((c = getchar())!='\n')
        {
            if (c == -1)
                break;
            else if(c == '-')
                znak = c;
            str[j++] = c;
        }

        if (j == 0) break;
        str[j] = '\0';


        char *token = strtok(str,delim);
        char *rec = malloc(strlen(token)+1);
        strcpy(rec,token);

        t ->clan1 = rec;
        t ->redClan1 = NadjiRed(rec);


        token = strtok(NULL,delim);
        rec = malloc(strlen(token)+1);
        strcpy(rec,token);

        t ->clan2 = rec;
        t ->redClan2 = NadjiRed(rec);

        t -> odnos = znak;
        povezani =Add(t,povezani);
        struct lista *l =povezani;

        len++;
    }

    brPovezanih = len;
    printf("Broj povezanih: %d\n",brPovezanih);

    struct node* p = povezani -> first;
    while(p != NULL){
        printf("%s %s %c\n",p->info ->clan1,p -> info -> clan2,p -> info -> odnos);
        p = p -> next;
    }
}


void UnesiPodatkeUTabelu()
{
    const char delim[2] = ",";
    for (int i = 0; i < m; ++i) {
        char str[MAX_STRLEN];
        scanf("%s\n",str);
        for (int j = 0; j < n; ++j) {
            char* token = strtok(j == 0?str:NULL,delim);
            char* t = malloc(sizeof(char)* strlen(token)+1);
            strcpy(t,token);
            glavnaTabela[i][j] = t;

        }
    }
}
//region Obrada

char *** KopirajTabelu(char*** tabela)
{
    char ***kopija = InicijalizujTabelu();
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < n; ++j)
            kopija[i][j] = tabela[i][j];

    return kopija;

}


bool ProveriValidnost(char ***tabela)
{
    struct node* p = povezani -> first;
    while(p != NULL)
    {
        int i = p -> info -> redClan1;
        char znak = '-';
        for (int j = 0; j < n ; ++j) {
            if(tabela[i][j] == NULL)
                continue;
            if(strcmp(tabela[i][j],p -> info -> clan1) != 0)
                continue;
            // nadjen prvi

            if(tabela[p -> info -> redClan2][j] == NULL)
            {
                znak = p -> info -> odnos;
                break;
            }

            if(strcmp(tabela[p -> info -> redClan2][j],p -> info -> clan2) == 0)
            {
                znak = '+';
                break;
            }
        }

        if(znak != p -> info ->odnos)
            return false;

        p = p -> next;
    }
    return true;
}
//region Red
struct red{
    struct cvor* red[MAX_RED_LEN];
    int front;
    int rear;
}red;
void InicijalizujRed()
{
    red.front = 0;
    red.rear = 0;
}
void Insert(struct cvor* x)
{
    red.rear = (red.rear+1) % MAX_RED_LEN;
    if(red.front != red.rear)
        red.red[red.rear] = x;
}

struct cvor* Delete()
{
    if (red.front != red.rear)
    {
        red.front = (red.front+1) % MAX_RED_LEN;
        return red.red[red.front];
    }
    else
        printf("RED JE PUN!\n");
}


//endregion

FILE *debug;
FILE *idTabelaVeza;

void IspisiVezuIdTabela(struct cvor* cvor)
{
    fprintf(idTabelaVeza,"=================\n");
    IspisiIdUFajl(idTabelaVeza,cvor -> id);
    IspisiTabeluUFajl(idTabelaVeza,cvor -> tabela);
}
void SveMoguceKombinacije(struct cvor* parent)
{
    char ***parentTabela = parent -> tabela;

    struct red pr = red;


    for (int i = 1; i < m; ++i) {
        struct lista *dozvoljeni = NULL;
        for (int j = 0; j < n; ++j) {
            dozvoljeni = Add(glavnaTabela[i][j],dozvoljeni);
        }

        for (int j = 0; j < n; ++j) {
            if (parentTabela[i][j] == NULL) continue;
            dozvoljeni = Remove(parentTabela[i][j],dozvoljeni);
        }

        for (int j = 0; j < n; ++j) {
            if(parentTabela[i][j] != NULL)
                continue;

            struct node* t = dozvoljeni -> first;

            while(t != NULL) //prolazak kroz listu dozvoljenih
            {
                char ***trenutnaTabela = KopirajTabelu(parentTabela);
                trenutnaTabela[i][j] = t->info;

                struct cvor* noviCvor = NapraviCvor(trenutnaTabela);

                bool validnost = ProveriValidnost(trenutnaTabela);
                noviCvor -> jeValidan = validnost;
                noviCvor -> nivo = parent ->nivo +1;

                pr = red;
                if(validnost==true)
                {
                    Insert(noviCvor);

                }

                DodajCvor(noviCvor,parent);

                IspisiVezuIdTabela(noviCvor);
                t = t -> next;

            }
        }
        DealocirajListu(dozvoljeni);
    }

}


void FormirajStablo() {
    if(debug == NULL)
        debug = fopen("debug.txt","a");
    InicijalizujRed();
    char ***tabelaKoren = InicijalizujTabelu();
    for (int j = 0; j < m; ++j)
        tabelaKoren[0][j] = glavnaTabela[0][j];

    InicijalizujStablo(tabelaKoren);
    Insert(stablo);

    stablo -> nivo = 0;
    while(red.rear != red.front)//!isEmpty())
    {
        struct cvor* next = Delete();
        while(next -> jeValidan == false)
            next = Delete();

        SveMoguceKombinacije(next);
    }
    fclose(debug);


}

//region FormiranjeAdreseFajlaResenja
void tostring(char str[], int num)
{
    int i, rem, len = 0, nl;

    nl = num;
    while (nl != 0)
    {
        len++;
        nl /= 10;
    }
    for (i = 0; i < len; i++)
    {
        rem = num % 10;
        num = num / 10;
        str[len - (i + 1)] = rem +'0';
    }
    str[len] = '\0';
}
void NapraviAdresuFajlaZaResenje(char adresaIzlaznogFajla[],int brResenja)
{
    char toString[100];
    tostring(toString,brResenja);

    strcat(adresaIzlaznogFajla,toString);
    strcat(adresaIzlaznogFajla,".txt");
}

//region StekInt
struct stekInt{
    int stek[MAX_STEK_LEN];
    int top;
} stekInt;

void InicijalizujStekInt()
{
    stekInt.top = 0;
}

void PushInt(int x)
{
    stekInt.stek[stekInt.top++] = x;
}

bool isEmptyStekInt()
{
    return stekInt.top == 0;
}

int PopInt()
{
    if(stekInt.top == 0)
        return -1;
    return stekInt.stek[--stekInt.top];
}
//endregion
int brResenja;
//endregion
bool IspisiResenja(struct cvor* _stablo,bool samoPrvo,bool koristiFajl,bool ispisiResenja)
{
    if(_stablo == NULL)
        return false;
    InicijalizujStek();
    InicijalizujStekInt();
    Push(_stablo);


    brResenja = 1;

    struct stek st = stek;
    bool resen = true;

    FILE *resenjeFajl = NULL;
    InicijalizujRed();
    bool imaResenje =false;

    struct cvor* obilazak[200];
    int i = 0;


    PushInt(i);

    while(!isEmpty())
    {
        if(resen)
        {
            resen = false;

        }

        struct cvor* next = Pop();
        i = PopInt();

        struct cvor* prev = NULL;
        while(next != NULL)
        {
            obilazak[i] = next;
            if(next -> desni != NULL) {
                Push(next->desni);
                PushInt(i);
            }
            i++;
            prev = next;
            next = next -> levi;
        }

        if(prev -> jeValidan)
        {
            imaResenje = true;
            resen = true;

            if(ispisiResenja)
            {
                if(koristiFajl) {
                    char adresaFajlaResenje[100] = "resenja/resenje";
                    NapraviAdresuFajlaZaResenje(adresaFajlaResenje, brResenja++);

                    if (resenjeFajl != NULL) fclose(resenjeFajl);
                    resenjeFajl = fopen(adresaFajlaResenje, "w");
                }

                for (int j = 0; j < i; j++) {
                    IspisiTabeluUFajl(koristiFajl ? resenjeFajl : stdout, obilazak[j]->tabela);
                }
            }
            if(samoPrvo)
            {fclose(resenjeFajl);return true;}

        }


    }
    fclose(resenjeFajl);
    return imaResenje;
}
//endregion
void Dealociraj()
{
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            free(glavnaTabela[i][j]);
        }
        free(glavnaTabela[i]);
    }
    free(glavnaTabela);
}

FILE *ispisanoStablo;
void IspisiStablo(struct cvor* _stablo)
{
    InicijalizujStek();
    InicijalizujStekInt();

    _stablo -> nivo = 0;

    Push(_stablo);
    bool jeLevi = true;



    while(!isEmpty())
    {
        struct cvor* next = Pop();


        while(next != NULL) {


            for (int i = 0; i < next -> nivo; ++i) {
                fprintf(ispisanoStablo, i == next -> nivo - 1 ? "|" : "  ");
            }

            fprintf(ispisanoStablo,"%c%c:%d\n", jeLevi?'l':'d',next -> jeValidan?' ':'x',next->id);

            if (next->desni != NULL)
            {
                next -> desni -> nivo = next -> nivo +1;
                Push(next -> desni);
            }

            jeLevi = true;
            int t = next -> nivo;
            next = next->levi;
            if(next != NULL) next -> nivo = t+1;

        }

        jeLevi = false;
    }

}

void IspisiStabloVisestrukoGrananje(struct cvor* _stablo)
{
    InicijalizujStek();


    Push(_stablo);



    while(!isEmpty())
    {
        struct cvor* next = Pop();


        while(next != NULL) {


            for (int i = 0; i < next -> nivo; i++) {
                fprintf(ispisanoStablo, i == next -> nivo - 1 ? "|" : "    ");
            }

            fprintf(ispisanoStablo,"%d\n", next->id);

            if(next -> desni != NULL)
                Push(next ->desni);


            next = next->levi;
        }


    }

}


//region KorisnikIgraIgru
bool krajIgre = false;

char ***korisnickaTabela;

struct cvor* trenutniCvor;

void InicijalizacijaIgre()
{
    trenutniCvor = stablo;
    korisnickaTabela = InicijalizujTabelu();
    for (int i = 0; i < n; ++i) {
        korisnickaTabela[0][i] = glavnaTabela[0][i];
    }
}

bool UporediTabele(char ***tabela1, char ***tabela2)
{
    for (int i = 1; i < m; ++i)
        for (int j = 0; j < n; ++j)
        {
            if(tabela1[i][j] == NULL && tabela2[i][j] == NULL)
                continue;
            if(tabela1[i][j] == NULL && tabela2[i][j] != NULL)
                return false;
            if(tabela1[i][j] != NULL && tabela2[i][j] == NULL)
                return false;

            if(strcmp(tabela1[i][j], tabela2[i][j]) != 0)
                return false;
        }
    return true;
}

void AzurirajTrenutniCvor()
{
    struct cvor* next = trenutniCvor -> levi;
    if(next == NULL)
    {
        krajIgre = true;
        return;
    }

    while(!UporediTabele(next ->tabela,korisnickaTabela))
    {
        next = next -> desni;
        if(next == NULL) {
            krajIgre = true;
            return;
        }
    }
    trenutniCvor = next;

}

void PogresnoUnetPodatak()
{
    printf("Unet pogresan podatak! Unesite ponovo!\n");
}

void KrajIgre()
{

    krajIgre = true;
}

void OdigrajPotez()
{
    int i,j;
    IspisiPodatkeIzTabele(korisnickaTabela);
    printf("Unesite indekse polja koje popunjavate (0 najmanji indeks) [i j]: ");
    scanf("%d%d",&i,&j);

    if(i <= 0 || i >= m || j < 0 || j >= n)
    {
        PogresnoUnetPodatak();
        return;
    }

    char *vrednost = malloc(MAX_STRLEN);
    printf("Unesite vrednost polja: ");
    scanf("%s",vrednost);
    korisnickaTabela[i][j] = vrednost;
    IspisiPodatkeIzTabele(korisnickaTabela);

    AzurirajTrenutniCvor();
    if(IspisiResenja(trenutniCvor -> levi,true,false,false) == false)
        KrajIgre();


}

void DaLiSamNaDobromPutu()
{
    struct cvor * t = trenutniCvor;

    bool imaResenje = IspisiResenja(trenutniCvor ,true,false,false);

    printf("----------------\n");
    printf(imaResenje ? "Na dobrom si putu!\n":"Nisi na dobrom putu!\n");
    printf("----------------\n");

}


void Pomoc()
{
    bool postojiResenje = IspisiResenja(trenutniCvor,true,false,true);
    if(!postojiResenje) printf("Resenje ne postoji!\n");
    KrajIgre();

}

void IspisiMeni()
{
    printf("################\n");
    printf("1. Nastavi Igru\n");
    printf("2. Da li sam na dobrom putu?\n");
    printf("3. Pomoc\n");
    printf("4. Kraj igre\n");
    printf("################\n");
}


void Meni()
{
    int izbor;
    scanf("%d",&izbor);

    switch (izbor) {
        case 1: OdigrajPotez();
            break;
        case 2: DaLiSamNaDobromPutu();
            break;
        case 3: Pomoc();
            break;
        case 4: KrajIgre();
            break;
        default:
            PogresnoUnetPodatak();
    }
}
void KorisnikIgraIgru()
{
    InicijalizacijaIgre();
    //OdigrajPotez();

    while(krajIgre == false)
    {
        IspisiMeni();
        Meni();

    }

    printf("IGRA JE ZAVRSENA!\n");
}



//endregion



int main() {
    idTabelaVeza = fopen("idTabelaVeza.txt", "w");
    InicijalizujStek();
    InicijalizujRed();

    scanf("%d",&m);
    scanf("%d",&n);
    glavnaTabela = InicijalizujTabelu();

    UnesiPodatkeUTabelu();
    UcitajUparenost();

    IspisiPodatkeIzTabele(glavnaTabela);

    FormirajStablo();

    ispisanoStablo = fopen("ispisanoStablo.txt","w");
    //IspisiStablo(stablo);
    IspisiStabloVisestrukoGrananje(stablo);
    fclose(ispisanoStablo);

    IspisiResenja(stablo,false,true,true);


    printf("Broj cvorova je: %d\n",nextID-1);

    printf("Broj resenja je: %d\n",brResenja-1);

    fclose(idTabelaVeza);


    printf("========================\n");

    fflush(stdout);
    fflush(stdin);

    KorisnikIgraIgru();

    Dealociraj();

    fclose(fajl);
    fclose(idTabelaVeza);

    getchar();
    getchar();

    return 0;

}