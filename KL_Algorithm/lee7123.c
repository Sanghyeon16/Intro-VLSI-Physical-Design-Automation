#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_dgree 1000 //max connection

struct _Nodes
{
    int dgree;
    int label;
    int weight;
    int (*table)[2]; // label, weight
};
typedef struct _Nodes Nodes; 

struct _Graph{
    int V;
    int E;
    int (*edgeTable)[2];
    int (*edgePair)[2];
    Nodes **neighborList;
};
typedef struct _Graph Graph;


Nodes *new_nodes(int label)
{
    Nodes *n = (Nodes *)malloc(sizeof(Nodes));
    n->dgree = 0;
    n->label = label;
    n->table = NULL;
    return n;
}



Graph *newGraph(int Node, Nodes *nodeList[])
{
    Graph *G = (Graph *)malloc(sizeof(Graph));
    G->V = Node;
    G->E = 0;
    G->neighborList = (Nodes **)calloc(Node + 1, sizeof(Nodes *));
    //sort Nodes by label using table
    for(int i = 0; i < G->V; i++)  
        G->neighborList[nodeList[i]->label] = nodeList[i];
    G->neighborList[0] = new_nodes(0);
    
    G->edgeTable = (int (*)[2])calloc(1, sizeof *(G->edgeTable));
    G->edgePair = (int (*)[2])calloc(1, sizeof *(G->edgePair));
    memset(G->edgeTable, 0, sizeof *(G->edgeTable));
    memset(G->edgePair, 0, sizeof *(G->edgePair));
    
    return G;
}

//  Partition check and  show the pairs cutted
//  
int cut(Graph *G, Nodes *A[], Nodes *B[])
{
    int totalCost = 0;
    Nodes *nA, *nB;
    
    //cost [i][j] is cost betwwen i node and j node
    int (*cost)[G->V + 1] = (int (*)[G->V +1])calloc(G->V + 1, sizeof *cost);
    char *bind = (char*)calloc(G->V + 1, sizeof *bind);
    memset(cost, 0, (G->V +1) * sizeof *cost);
    memset(bind, 0, (G->V + 1) * sizeof*bind);

    for(int i=0; i < G->V/2; i++)
    {
        bind[A[i]->label] = 'A';
        bind[B[i]->label] = 'B';
    }
    bind[B[G->V - G->V/2 -1]->label] = 'B';

    for(int i=0; i< G->V/2; i++)
    {
        nA = A[i];
        nB = A[i];
        for(int j=0; j< nA->dgree; j++)
        {//note that table[0] is label and table[1] is weight
            cost[nA->label][nA->table[j][0]] = nA->table[j][1]; //if connected cost is 1 (weight)
            
            
            if(bind[nA->label] != bind[nA->table[j][0]])
            {//if the jth neighbor is not in the same bind
                totalCost += nA->table[j][1]; //cost += 1;
                printf("%d\t--/--\t%d\n", nA->label, nA->table[j][0]);
            }
        }
    }
    free(cost);
    free(bind);
    return totalCost;
}


void KL_algorithm(Graph *G, Nodes *a[], Nodes *b[]){
	int MIN = (int)(~0) << (sizeof(int) * 8 - 1);
	int MAX = ~MIN ;
    int V = G->V;


	int	*Indexing = calloc(V + 1, sizeof *Indexing);
	int *Acnt = (int *)calloc( MAX_dgree * 2 + 1, sizeof *Acnt );
	int *Bcnt = (int *)calloc( MAX_dgree * 2 + 1, sizeof *Bcnt );
	int *d = (int *)calloc(V + 1, sizeof *d );
	int (*cost)[V + 1] = (int (*)[V + 1])calloc(V + 1, sizeof *cost);
	int *Gsum = (int *)calloc(V / 2 + 1, sizeof *Gsum);
	int (*swap)[2] = (int (*)[2])calloc(V / 2 + 1, sizeof *swap);
	int *fixed = (int *)calloc(V + 1, sizeof *fixed);
	int i, j, k;
    int maxk, maxgain, pass = 0, cumulative_gain = 0 ;
	char *bind = (char *)calloc(V + 1, sizeof *bind) ; 
	
    Nodes *nA, *nB, 
	**sortedA = (Nodes **)calloc( V / 2, sizeof *sortedA),
	**sortedB = (Nodes **)calloc(V - V / 2, sizeof *sortedB);

	KLprocess:
	memset(Acnt, 0, (MAX_dgree * 2 + 1) * sizeof *Acnt);
	memset(Bcnt, 0, (MAX_dgree * 2 + 1) * sizeof *Bcnt);
	memset(d, 0, (V + 1) * sizeof *d)  ;
	memset(cost, 0, (V + 1) * sizeof *cost) ;
	memset(Gsum , 0, (V / 2 + 1) * sizeof *Gsum ) ;
	memset(swap, 0, (V / 2 + 1) * sizeof *swap) ;
	memset(bind, 0, (V + 1) * sizeof *bind) ;
	memset(fixed, 0, (V + 1) * sizeof *fixed ) ;

	Gsum[0] = 0 ;
	maxk = 0 ;
	maxgain = MIN ;



    //fix block binding
    //
	for(i = 0; i < V / 2; i++)
    {
		bind[ a[i]->label ] = 'a' ;
		bind[ b[i]->label ] = 'b' ;
	}	

    //in case of odd Nodes
	if( V - V / 2  != V / 2)
    {
		bind[ b[V - V / 2 - 1]->label ] = 'b' ;
	}

    //D(ai)
	int max_d_a = 0, min_d_a = MAX_dgree + MAX_dgree ;
	for (i = 0; i < V / 2; i++)
    {
		nA = a[i];
		for(j = 0; j < nA->dgree; j++)
        {
			if(bind[ nA->table[j][0] ] == bind[ nA->label ])
            {
				d[nA->label] -= nA->table[j][1] ;
			}
            else
            {
				d[nA->label] += nA->table[j][1] ;
			}
			cost[ nA->label ][ nA->table[j][0] ] = nA->table[j][1] ;
		}	
		Acnt[ MAX_dgree + d[nA->label] ] += 1;
		min_d_a = min_d_a > MAX_dgree + d[nA->label] ? MAX_dgree + d[nA->label] : min_d_a ;
		max_d_a = max_d_a < MAX_dgree + d[nA->label] ? MAX_dgree + d[nA->label] : max_d_a ;
	}	


    //D(bi)
	int max_d_b = 0, min_d_b = MAX_dgree + MAX_dgree ;
	for(i = 0; i < V - V / 2 ; i++)
    {
		nB = b[i] ;
		for(j = 0; j < nB->dgree; j++)
        {
			if(bind[ nB->table[j][0] ] == bind[nB->label ])
            {
				d[nB->label] -= nB->table[j][1] ;
			}
            else
            {
				d[nB->label] += nB->table[j][1] ;
			}
			cost[ nB->label ][ nB->table[j][0] ] = nB->table[j][1] ;
		}
		Bcnt[ MAX_dgree + d[nB->label] ] += 1;
		min_d_b = min_d_b > MAX_dgree + d[nB->label] ? MAX_dgree + d[nB->label] : min_d_b ;
		max_d_b = max_d_b < MAX_dgree + d[nB->label] ? MAX_dgree + d[nB->label] : max_d_b ;
	}


	

	int iter ;
	int sum_d_a[ max_d_a - min_d_a + 1]  ;
	memset(sum_d_a, 0,  (max_d_a - min_d_a + 1) * sizeof (int) );

    //Ex. sum_DofA[] = [0, Acnt[1]] from material example
	for(iter = min_d_a; iter <= max_d_a; iter ++)
    {
		if( iter > min_d_a )
        {
			sum_d_a[ iter - min_d_a ] = sum_d_a[ iter - 1 - min_d_a ]  + Acnt[ iter - 1 ];
		}
        else
        {
			sum_d_a[ iter - min_d_a ] = 0;
		}
	}
	int sum_d_b[ max_d_b - min_d_b + 1] ;
	memset(sum_d_b, 0,  (max_d_b - min_d_b + 1) * sizeof (int) );
	for(iter = min_d_b; iter <= max_d_b; iter ++)
    {
		if( iter > min_d_b )
        {
			sum_d_b[ iter - min_d_b ] = sum_d_b[ iter - 1 - min_d_b ]  + Bcnt[ iter - 1 ];
		}
        else
        {
			sum_d_b[ iter - min_d_b ] = 0;
		}
	}
	for(iter = 0 ; iter < V / 2; iter ++)
    {
		Nodes *v_a  =  a[ iter ] ;
		int d_MAX_dgree_centered = MAX_dgree + d[ v_a->label] ;
		Acnt[ d_MAX_dgree_centered  ] -= 1;
		int index = Acnt[ d_MAX_dgree_centered  ] ;
		sortedA[ index + sum_d_a[ d_MAX_dgree_centered  - min_d_a ] ] = v_a ; 
	}
	
	for(iter = 0 ; iter <V - V / 2; iter ++)
    {
		Nodes *v_b  =  b[ iter ] ;
		int d_MAX_dgree_centered = MAX_dgree + d[ v_b->label] ;
		Bcnt[ d_MAX_dgree_centered  ] -= 1;
		int index = Bcnt[ d_MAX_dgree_centered  ] ;
		sortedB[ index + sum_d_b[ d_MAX_dgree_centered  - min_d_b ] ] = v_b ;
	}
	memcpy(a, sortedA, (V / 2) * sizeof *sortedA) ;
	memcpy(b, sortedB, (V - V / 2) * sizeof *sortedB) ;

	for(i = 0; i < V; i++)
    {
		if(i < V / 2)
        {
			Indexing[ a[i]->label ] = i ;
		}
        else
        {
			Indexing[ b[i - V / 2]->label ] = i ;
		}
		
	}

	//InnerLoop
	for (k = 1; k <= V / 2; k++)
    {
		int wilbefixed[2], wilbeswaped[2] ;
		for(i =  V / 2 - 1; i >= 0 ; i--)
        {
			int a_i_label;
			a_i_label = a[i]->label ;
			if(d[ a_i_label ] + d[ b[ V - V / 2 - 1 ]->label ] < maxgain )
				break;
			
			if (!fixed[ a_i_label ])
            {
				for(j = V - V / 2 - 1; j >= 0  ; j--)
                {
					int b_j_label = b[j]->label ;
					if(d[ a_i_label ] + d[ b_j_label ] < maxgain )
						break;
					
					if(!fixed[ b_j_label ])
                    {
						int gain = d[ a_i_label ] + d[ b_j_label ] - 2 * cost[ a_i_label ][ b_j_label ] ;
						if( gain >= maxgain )
                        { 
							maxgain = gain ;
							wilbefixed[0] = a_i_label ;
							wilbeswaped[0] = i ;
							wilbefixed[1] = b_j_label ;
							wilbeswaped[1] = j ;
						}
					}
				}
			}
		}
		
		fixed[ wilbefixed[0] ] = 1 ;
		fixed[ wilbefixed[1] ] = 1 ;
		swap[k][0] = a[wilbeswaped[0] ]->label ;
		swap[k][1] = b[wilbeswaped[1] ]->label ;		


        //D update
		nA = a[ wilbeswaped[0] ] ;
		nB = b[ wilbeswaped[1] ] ;
		for(i = 0; i < nA->dgree; i++)
        {
			int label = nA->table[i][0] ;
			if(!fixed[label])
            {
				if( bind[ nA->label ] == bind[ label ] )
                {
					d[ label ] += 2 * cost[ nA->label ][ label ] ;
					int j = Indexing[ label ] ;
					while(j + 1 < V / 2 && d[a[j]->label] > d[ a[j  + 1]->label ] )
                    {
						Nodes *temp = a[j + 1] ;
						a[j + 1] = a[j] ;
						a[j] = temp ;
						Indexing[ a[j]->label ] = j ;
						Indexing[ a[j + 1]->label ] = j + 1;
						( j = j + 1 ) ;
					}
				}
                else
                {
					d[ label ] -= 2 * cost[ nA->label ][ label ] ;
					int j = Indexing[ label ]  - V / 2;
					while(j - 1 >= 0 && d[  d[ b[j]->label ] < b[j - 1]->label]  )
                    {
						Nodes *temp = b[j - 1] ;
						b[j - 1] = b[j] ;
						b[j] = temp ;
						Indexing[ b[j]->label ] = j + V / 2;
						Indexing[ b[j - 1]->label ] = j - 1 + V / 2;
						( j = j - 1 ) ;
					}
				}
		
			}
		}
		for(i = 0; i < nB->dgree; i++)
        {
			int label = nB->table[i][0] ;
			if(!fixed[label])
            {
				if(bind[ nB->label ] == bind[ label ])
                {
					d[ label ] += 2 * cost[ nB->label ][ label ] ;
					int j = Indexing[ label ] - V / 2 ;
					while(j + 1 < V - V / 2 && d[ b[j ]->label] < d[ b[j + 1]->label ] )
                    {
						Nodes *temp = b[j + 1] ;
						b[j + 1] = b[j] ;
						b[j] = temp ;
						Indexing[ b[j]->label ] = j + V / 2;
						Indexing[ b[j + 1]->label ] = j + 1 + V / 2;
						( j = j + 1 ) ;
					}
				}
                else
                {
					d[ label ] -= 2 * cost[ nB->label ][ label ] ;
					int j = Indexing[ label ] ;
					while(j - 1 >= 0 && d[  d[ a[j]->label ] < a[j - 1]->label] )
                    {
						Nodes *temp = a[j - 1] ;
						a[j - 1] = a[j] ;
						a[j] = temp ;
						Indexing[ a[j]->label ] = j ;
						Indexing[ a[j - 1]->label ] = j - 1;
						( j = j - 1 ) ;
					}
				}
			}
		}



		Gsum[k] = Gsum[k - 1] + maxgain;
		if(Gsum[k] > Gsum[ maxk ])
        {
			maxk = k ;
		}
		maxgain = MIN ;
	}
	if(maxk == 0)
    {
		free(d) ;
		free(cost);
		free(Gsum);
		free(swap);
		free(bind) ;
		free(fixed);
		free(sortedA); 
		free(sortedB); 
        free( Acnt );
        free( Bcnt );
		free(Indexing);

		return ;
	}
    else
    {
		pass += 1 ;
		cumulative_gain += Gsum[maxk ] ;


		for(i = 1; i <= maxk; i++){
			int exi[2] = { Indexing[ swap[i][0] ], Indexing[ swap[i][1] ] - (V / 2) } ;
			Nodes *temp = a[ exi[0] ]  ;
			a[ exi[0] ] = b[ exi[1] ] ;
			b[ exi[1] ] = temp ;
			int tint = Indexing[ swap[i][0] ] ;
			Indexing[ swap[i][0] ] = Indexing[ swap[i][1] ] ;
			Indexing[ swap[i][1] ] = tint ;
			bind[ swap[i][0] ] = 'b' ;
			bind[ swap[i][1] ] = 'a' ;
		}
		goto KLprocess ;
	}
}

int main(int argc, char *argv[])
{
    int curline = 0;
    int Node, Edge;
    int (*etable)[2], (*epair)[2];
    Nodes **ntable, **setA, **setB;
    Graph *G;
    if(argc != 2)
    {
        printf("Please type input file as an argument\nExample : ./lee7123 <input file>\n");
            return 0;
    }
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    fp = fopen(argv[1], "r");


    if(fp == NULL)
        exit(EXIT_FAILURE);
    
    while((read = getline(&line, &len, fp)) != -1)
    {
        int numA, numB;
        char *next_ptr; // save pointer
        char *ptrA = strtok_r(line, " \r\t\f", &next_ptr); //set A
        char *ptrB = strtok_r(NULL, " \r\t\f", &next_ptr); //set B

        if(ptrA != NULL && ptrB != NULL)
        {
            numA = atoi(ptrA);
            numB = atoi(ptrB);
        }

        //init Nodes and Edges
        if(curline == 0) //The first line of input file is the number of nodes and edges
        {
            Node = numA;
            Edge = numB;

            ntable = (Nodes **)calloc(Node + 1, sizeof(Nodes *));
            etable = (int (*)[2])calloc(Edge +1, sizeof *etable);
            epair = (int (*)[2])calloc(Edge +1, sizeof *epair);
            
            memset(etable, 0, sizeof *etable);
            memset(epair, 0, sizeof *epair);

            for(int i=0; i <= Node; i++)
                ntable[i] = new_nodes(i);
        }
        else
        {
            Nodes *nA, *nB;
            etable[curline][0] = curline;
            etable[curline][1] = 1; // cost is equally 1

            epair[curline][0] = numA;
            epair[curline][1] = numB;
            nA = ntable[numA];
            nB = ntable[numB];

            //find connected nodes to nA
            nA->dgree += 1;
            nA->table = (int (*)[2])realloc(nA->table,nA->dgree * sizeof *(nA->table));
            nA->table[nA->dgree - 1][0] = nB->label;
            nA->table[nA->dgree - 1][1] = 1; // weight
            //find connected nodes to nB
            nB->dgree += 1;
            nB->table = (int (*)[2])realloc(nB->table,nB->dgree * sizeof *(nB->table));
            nB->table[nB->dgree - 1][0] = nA->label;
            nB->table[nB->dgree - 1][1] = 1; // weight
            
            ntable[nA->label] = nA;
            ntable[nB->label] = nB;

        }
        curline += 1;
    }//Node table and Egde table is generated

    
    //init Graph
    G = newGraph(0, NULL);
    G->V = Node; // Nodes size
    G->E = Edge; // Edge size
    if(G->neighborList[0]->table!= NULL)
        free(G->neighborList[0]->table);
    free(G->neighborList[0]);

    free(G->edgeTable);
    free(G->neighborList);
    free(G->edgePair);

    G->edgeTable = etable;
    G->edgePair = epair;
    G->neighborList = ntable;

    //Kernighen-Lin Algorithm
    setA = (Nodes **)calloc(Node/2, sizeof(Nodes *));
    setB = (Nodes **)calloc(Node - Node/2, sizeof(Nodes *));

    //Partition {1, 2, ..., Node/2} : {Node/2+1, Node/2+2, ..., Node}
    for(int i = 0; i< Node/2; i++)
        setA[i] = G->neighborList[i+1];
    
    for(int i = 0; i< Node - Node/2; i++)
        setB[i] = G->neighborList[Node/2 + i + 1];

    int total_cost = cut(G, setA, setB);

    printf("Initially cut cost: %d\n",total_cost);
    KL_algorithm(G, setA, setB);

    total_cost = cut(G, setA, setB);
    printf("Partition A\tPartition B\n") ;
    printf("Cut Size: %d \n", total_cost);
    
    
    //free memory
    free(setA);
    free(setB);

    if(G->neighborList != NULL)
    {
        for( int i=0; i<= G->V; i++)
        {
            if(G->neighborList[i]->table != NULL)
                free(G->neighborList[i]->table);
        }
        free(G->neighborList);
    }
    if(G->edgeTable != NULL)
        free(G->edgeTable);
    if(G->edgePair != NULL)
        free(G->edgePair);

    free(G);
    
    free(line);
    fclose(fp);

    exit(EXIT_SUCCESS);
}