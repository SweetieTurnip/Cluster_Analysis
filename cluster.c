/**
 * Kostra programu pro 2. projekt IZP 2022/23
 *
 * Jednoducha shlukova analyza: 2D nejblizsi soused.
 * Single linkage
 * Author: Aleksander Postelga
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX

#include <string.h>
#include <stdbool.h>

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *	#define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

#define MAX_CORDINATE 1000

/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *	pocet objektu ve shluku,
 *	kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *	ukazatel na pole shluku.
 */

struct obj_t {
    int id;
    float x;
    float y;
};
struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};

/*
 Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 Ukazatel NULL u pole objektu znamena kapacitu 0.
*/
void init_cluster(struct cluster_t *c, int cap)
{
    assert(c != NULL);
    assert(cap >= 0);

    c->capacity = cap;
    c->size = 0;
    c->obj = (struct obj_t*)malloc(cap * sizeof(struct obj_t));
    if(c->obj == NULL)
    {
        c->capacity = 0;
    }
}

/*
 Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 */
void clear_cluster(struct cluster_t *c)
{
    free(c->obj);
    c->size = 0;
}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/*
 Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    c->obj = (struct obj_t*)arr;
    c->capacity = new_cap;
    return c;
}

/*
 Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 nevejde.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
    if(c->size >= c->capacity)
    {
     	c = resize_cluster(c, c->capacity + CLUSTER_CHUNK);
        if(c == NULL)
        {
            fprintf(stderr, "Reallocation cluster error\n");
        }
    }
    c->obj[c->size].id = obj.id;
    c->obj[c->size].x = obj.x;
    c->obj[c->size].y = obj.y;
    c->size++;
}

/*
 Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
 */
void sort_cluster(struct cluster_t *c);

/*
 Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 Objekty ve shluku 'c1' budou serazeny vzestupne podle identifikacniho cisla.
 Shluk 'c2' bude nezmenen.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);

   for(int i = 0; i < c2->size; i++)
   {
       append_cluster(c1, c2->obj[i]);
   }
    sort_cluster(c1);
}

/**********************************************************************/
/* Prace s polem shluku */

/*
 Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    assert(idx < narr);
    assert(narr > 0);

    clear_cluster(&carr[idx]);
    memmove(&carr[idx], &carr[idx+1], sizeof(struct cluster_t) * (narr-idx-1));
    return narr-1;
}
/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);

    return (sqrt(pow(o1->x - o2->x, 2) + pow(o1->y - o2->y, 2)));
}
/*
 Pocita vzdalenost dvou shluku.
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);

    float min = INT_MAX;
    float dist;
    for(int i = 0; i < c1->size; i++)
    {
        for(int j = 0; j < c2->size; j++)
        {
            dist = obj_distance(&c1->obj[i], &c2->obj[j]);
            min = dist < min ? dist : min;
        }
    }
    return min;
}
/*
 Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 adresu 'c1' resp. 'c2'.
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);

    float min = INT_MAX;
    for(int i = 0; i < narr; i++)
    {
     	for(int j = 0;j < narr;j++)
        {
            if(i != j)
            {
             	if(cluster_distance(&carr[i], &carr[j]) < min)
                {
                    min = cluster_distance(&carr[i], &carr[j]);
                    *c1 = i;
                    *c2 = j;
                }
            }
        }
    }
}
// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b)
{
    const struct obj_t *o1 = (const struct obj_t *)a;
    const struct obj_t *o2 = (const struct obj_t *)b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/*
 Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
*/
void sort_cluster(struct cluster_t *c)
{
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/*
 Tisk shluku 'c' na stdout.
*/
void print_cluster(struct cluster_t *c)
{
    for (int i = 0; i < c->size; i++)
    {
     	if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

//kontroluje, zda je ID unikátní
bool unique(int *id_arr, int count, int id)
{
    for(int i = 0; i < count; i++)
    {
     	if(id_arr[i] == id) return false;
    }
    return true;
}
/*
 Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
*/
int load_clusters(char *filename, struct cluster_t **arr)
{
    assert(arr != NULL);

    FILE *fp;
    int num_clusters, count;
    int x, y, current_line = 0;
    char n; //kontrola pro \n
    int res; //pocet nalezenych podobnosti fscanf 
    bool error = false;
    if((fp = fopen(filename, "r")) == NULL)
    {
        strcat(filename, ".txt");
        if((fp = fopen(filename, "r")) == NULL)
        {
            fprintf(stderr, "Can not open the file\n");
            return -1;
        }
    }
    //zapise hodnotu count
    if((fscanf(fp, "count=%d\n", &count)) != 1)
    {
        fprintf(stderr, "Can not find count of objects in file - %s", filename);
        return -1;
    }
    current_line++;
    if(count < 0)
    {
        fprintf(stderr, "Count must be positive\n");
        return -1;
    }
    *arr =(struct cluster_t *) malloc(count * sizeof(struct cluster_t));
    if(*arr == NULL)
    {
        fprintf(stderr, "Can not allocate memmory for cluster array\n");
        return -1;
    }
    int *id = malloc(count * sizeof(int));
    if(id == NULL)
    {
        fprintf(stderr, "Can not allocate memmory for id\n");
        free(*arr);
        *arr = NULL;
        return -1;
    }
    num_clusters = 0;
    for(int i = 0; i < count; i++)
    {
        res = fscanf(fp, "%d%d%d%c", &id[i], &x, &y, &n);
        current_line++;
        init_cluster(&(*arr)[i], 1);
        num_clusters++;
        if((*arr)[i].capacity == 0)
        {
            fprintf(stderr, "Allocation cluster in line %d error\n", current_line);        
            error = true;
            break;
        }
        if(res!=3)
        {
            if(res != 4 || n != '\n')
            {
                fprintf(stderr, "Line %d in file %s has bad format\n", current_line, filename);
                error = true;
                break;
            }
        } 
        if(!unique(id, i, id[i]))
        {
            fprintf(stderr, "ID in line %d in file %s is not unique\n", current_line, filename);
            error = true;
            break;
        }
	    if(x > 1000 || x < 0 || y > 1000 || y < 0)
        {
            fprintf(stderr, "Line %d in file %s consists x or y out of range\n", current_line, filename);
            error = true;
            break;
        }
        (*arr)[i].obj->id = id[i];    	
        (*arr)[i].obj->x = x;
        (*arr)[i].obj->y = y;
        (*arr)[i].size++;
        n = 0;
    }
    free(id);
    fclose(fp);
    //Сisti pole clusteru, pokud je zjistena chyba a vrati -1
    if(error)
    {
     	for(int i = 0; i < num_clusters; i++)
        {
            clear_cluster(&(*arr)[i]);
        }
	    free(*arr);
        *arr = NULL;
        return -1;
    }
    return num_clusters;
}
/*
 Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 Tiskne se prvnich 'narr' shluku.
*/
void print_clusters(struct cluster_t *carr, int narr)
{
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
     	printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}
//zkontroluje, zda argv[2] je cislo
bool isNumber(char *str)
{
    int len = strlen(str);
    for(int i = 0; i < len; i++)
    {
     	if(str[i] < '0' || str[i] > '9') return false;
    }
    return true;
}

int main(int argc, char *argv[])
{
    struct cluster_t *clusters;
    int c1, c2;
    int param = 1;
    bool error = false;

    //práce s argumenty
    if(argc < 2 || argc > 3)
    {
     	fprintf(stderr, "Error of arguments\n");
        return 1;
    }
    if(argc == 3)
    {
     	if(!isNumber(argv[2]))
        {
            fprintf(stderr, "Argument argv[2] should be number\n");
            return 1;
        }
	param = atoi(argv[2]);
        if(param <= 0)
        {
            fprintf(stderr, "Parametr N = %d should be bigger\n", param);
            return 1;
        }
    }
    char *filename = malloc(FILENAME_MAX + 5);
    if(filename == NULL)
    {
        fprintf(stderr, "Can not allocate memmory for filename\n");
        return 1;
    }
    strcpy(filename, argv[1]);
    int cluster_count = load_clusters(filename, &clusters); //-1 v pripade chyby
    if(cluster_count == -1)
    {
        free(filename);
        return 1;
    }
    if(cluster_count < param)
    {
     	fprintf(stderr, "Argument %d is too big for file %s\n", param, argv[1]);
        error = true;
    }
    while(cluster_count > param && !error)
    {
     	if(cluster_count == 0) break;
        find_neighbours(clusters, cluster_count, &c1, &c2);
        merge_clusters(&clusters[c1], &clusters[c2]);
        if(&clusters[c1] == NULL)
        {
            error = true;
        }
	cluster_count = remove_cluster(clusters, cluster_count, c2);
        if(&clusters[c2] == NULL)
        {
            error = true;
        }
    }
    if(!error) print_clusters(clusters, cluster_count);
    for(int i = 0; i < cluster_count; i++)
    {
     	clear_cluster(&clusters[i]);
    }
    free(clusters);
    free(filename);
    clusters = NULL;
    if(error) return 1;
    return 0;
}

