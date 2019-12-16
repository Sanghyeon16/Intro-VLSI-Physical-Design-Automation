#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

//Vertex
//  adjacent position[0][1] weight[2]
//  position[row][column]
//  number of adjacent nodes
//  parent[row][column]
struct _Vertex
{
	int (*adj_position)[3];
    int row_column[2];
    int degree;
    int parent[2];
};
typedef struct _Vertex Vertex;

//Graph
//row
//column
struct _Graph
{
	int row;
	int column;
	Vertex (**alist); 
};
typedef struct _Graph Graph;

//create_vertex
//add adjacent nodes with direction
void create_vertex(Vertex *node, int row, int column, int row_bound, int column_bound, int blockll_row, int blockll_column, int blockur_row, int blockur_column)
{
	//Vertex *node = (Vertex *)malloc(sizeof(Vertex));
	node->row_column[0] = row;
    node->row_column[1] = column;
    int degree_count = 0;
    
    //South
    if ((row-1>0) && (!((((row-1)<=blockur_row)&&((row-1)>=blockll_row))&&((column>=blockll_column)&&(column<=blockur_column)))))
        degree_count += 1;
    else
        degree_count = degree_count;

    //North
    if ((row+1<=row_bound) && (!((((row+1)<=blockur_row)&&((row+1)>=blockll_row))&&((column>=blockll_column)&&(column<=blockur_column)))))
        degree_count += 1;
    else
        degree_count = degree_count;

    //West
    if ((column-1>0) && (!(((row<=blockur_row)&&(row>=blockll_row))&&(((column-1)>=blockll_column)&&((column-1)<=blockur_column)))))
        degree_count += 1;
    else
        degree_count = degree_count;

    //East
    if ((column+1<=column_bound) && (!(((row<=blockur_row)&&(row>=blockll_row))&&(((column+1)>=blockll_column)&&((column+1)<=blockur_column)))))
        degree_count += 1;
    else
        degree_count = degree_count;

    node->degree = degree_count;

    // allocate adjacent node memory
    node->adj_position = (int (*)[3])realloc(node->adj_position, (degree_count+2) * sizeof(node->adj_position));
    int degree_count_new = 0;
    
    //North
    if ((row+1<=row_bound) && (!((((row+1)<=blockur_row)&&((row+1)>=blockll_row))&&((column>=blockll_column)&&(column<=blockur_column)))))
    {
        node->adj_position[degree_count_new][0] = row+1;
        node->adj_position[degree_count_new][1] = column;
        node->adj_position[degree_count_new][2] = 1;
        degree_count_new += 1;
    }
    else
        degree_count_new = degree_count_new;

    //South
    if ((row-1>0) && (!((((row-1)<=blockur_row)&&((row-1)>=blockll_row))&&((column>=blockll_column)&&(column<=blockur_column)))))
    {
        node->adj_position[degree_count_new][0] = row-1;
        node->adj_position[degree_count_new][1] = column;
        node->adj_position[degree_count_new][2] = 1;
        degree_count_new += 1;
    }
    else
        degree_count_new = degree_count_new;

    //West
    if ((column-1>0) && (!(((row<=blockur_row)&&(row>=blockll_row))&&(((column-1)>=blockll_column)&&((column-1)<=blockur_column)))))
    {
        node->adj_position[degree_count_new][0] = row;
        node->adj_position[degree_count_new][1] = column-1;
        node->adj_position[degree_count_new][2] = 1;
        degree_count_new += 1;
    }
    else
        degree_count_new = degree_count_new;

    //East
    if ((column+1<=column_bound) && (!(((row<=blockur_row)&&(row>=blockll_row))&&(((column+1)>=blockll_column)&&((column+1)<=blockur_column)))))
    {
        node->adj_position[degree_count_new][0] = row;
        node->adj_position[degree_count_new][1] = column+1;
        node->adj_position[degree_count_new][2] = 1;
        degree_count_new += 1;
    }
    else
        degree_count_new = degree_count_new;


}

//create_graph
//memory allocation
//create_vertex
Graph *create_graph(int row, int column)
{
	int i, j;
	Graph * G = (Graph *)malloc(sizeof(Graph));
    G->alist = (Vertex **)malloc(sizeof(Vertex *)*(row+1));
    for (i=0; i<row; i++)   
        G->alist[i] = (Vertex*)malloc(sizeof(Vertex)*(column+1));
    // Deal with those nodes which has index of 0
    for(i=0; i<row; i++)
    {
        for(j=0; j<column; j++)
        {
            if(i==0 || j==0)
            {
                create_vertex(&(G->alist[i][j]), 0,0,0,0,0,0,0,0);
            }
        }
    }
	G->row    = row ;
	G->column = column ;	
	
	return G ;
}

//Dijkstra_Algorithm
void Dijkstra_Algorithm(Graph *G, int s_row, int s_column, int t_row, int t_column)
{
   int dist[G->row+1][G->column+1];
   int group[G->row+1][G->column+1]; 
   int i,j,k;
   int i_scan3,j_scan3,k_scan3;
   int i_scan2,j_scan2,k_scan2;
   int distmin = INT_MAX; 
   int row_change3,column_change3; 
   int g3row,g3column;

   for (i=1; i<=G->row; i++)
    {
       for (j=1; j<=G->column; j++)
       {
            group[i][j] = 1;
            dist[i][j] = INT_MAX;
            G->alist[i][j].parent[0] = 0;
            G->alist[i][j].parent[1] = 0;
       }
    }

    group[s_row][s_column] = 3;
    dist[s_row][s_column] = 0;
    
    do
    {
        for (i_scan3=1; i_scan3<=G->row; i_scan3++)
        {
            for (j_scan3=1; j_scan3<=G->column; j_scan3++)
            { 
                if(group[i_scan3][j_scan3] == 3)
                {
                    for(k_scan3=0; k_scan3<(G->alist[i_scan3][j_scan3].degree); k_scan3++)
                    {
                        g3row = G->alist[i_scan3][j_scan3].adj_position[k_scan3][0];
                        g3column = G->alist[i_scan3][j_scan3].adj_position[k_scan3][1];
                        if((group[g3row][g3column] == 1) && ((dist[i_scan3][j_scan3] + G->alist[i_scan3][j_scan3].adj_position[k_scan3][2]) < dist[g3row][g3column]))
                        {
                            G->alist[g3row][g3column].parent[0] = i_scan3;
                            G->alist[g3row][g3column].parent[1] = j_scan3;
                            dist[g3row][g3column] = dist[i_scan3][j_scan3] + G->alist[i_scan3][j_scan3].adj_position[k_scan3][2];
                            group[g3row][g3column] += 1; //group1 to group2
                        }
                    }
                    group[i_scan3][j_scan3] += 1;
                    for (i_scan2=1; i_scan2<=G->row; i_scan2++)
                    {
                        for (j_scan2=1; j_scan2<=G->column; j_scan2++)
                            {
                                if((group[i_scan2][j_scan2] == 2)&&(dist[i_scan2][j_scan2]<= distmin))
                                {
                                    row_change3 = i_scan2;
                                    column_change3 = j_scan2;
                                    distmin = dist[i_scan2][j_scan2];
                                }
                            }
                    }
                    distmin=INT_MAX;
                    group[row_change3][column_change3] += 1; //group2 to group3
                }    
            }
        }
    }
    while (group[t_row][t_column] != 4);
    
    printf("Total path cost : %d\n", dist[t_row][t_column]);
    printf("The path in term of grid cells on the path:(From target node to source node)\n");
    int i_row = t_row;
    int i_column = t_column;
    int i_rowraw, i_columnraw;

    while((i_row != s_row) || (i_column != s_column))
    {
        printf("(%d,%d) ",i_row,i_column);
        i_rowraw = i_row;
        i_columnraw = i_column;
        i_row = G->alist[i_rowraw][i_columnraw].parent[0];
        i_column = G->alist[i_rowraw][i_columnraw].parent[1];
    }
    printf("\n");
}

int main(int argc, char ** argv)
{
    FILE *fp; 
	char *line = NULL;
	size_t len = 0;
    ssize_t read;
    
	int curLine = 0, row, column; 
	int i,j,k;
    int s_row,s_column;
    int t_row,t_column;
    int blockll_row, blockll_column, blockur_row, blockur_column;
    int fill_row, fill_column, fiur_row, fiur_column;
    Vertex **vertex_list, **a, **b;
	Graph *G;
    
    if(argc != 2)
    {
        printf("Please type input file as an argument\nExample : ./lee7123 <input file>\n");
        //inputex1.txt
        return 0;
    }
    fp = fopen(argv[1], "r");

	if(fp == NULL)
        exit(EXIT_FAILURE);
		
	while ((read = getline(&line, &len, fp)) != -1) 
	{
        if(curLine<=2)
        {
            char *saveptr;
	        char *w1 = strtok_r(line, " \t\v\f\r", &saveptr);
            char *w2 = strtok_r(NULL, " \t\v\f\r", &saveptr);
		    int figure1, figure2, figure3, figure4;
                
		    if((w1 != NULL) && (w2 != NULL))
		    {
			    figure1 = atoi(w1);
			    figure2 = atoi(w2);
		    }
                

            switch(curLine)
            {
                case 0:
                    row = figure1;
	    	        column = figure2;
                    vertex_list = (Vertex **)malloc(sizeof(Vertex *)*(row+1));
                    for (i=1; i<=row; i++)  
                        vertex_list[i] = (Vertex *)malloc(sizeof(Vertex)*(column+1));
                    break;
                case 1:
                    s_row = figure1;
                    s_column = figure2;
                    break;
                case 2:
                    t_row = figure1;
                    t_column = figure2;
                    break; 
            }
            curLine += 1;
		    
        }
		else if(curLine<=4)
        {
            char *saveptr;
		    char *w1 = strtok_r(line, " \t\v\f\r", &saveptr);
            char *w2 = strtok_r(NULL, " \t\v\f\r", &saveptr);
            char *w3 = strtok_r(NULL, " \t\v\f\r", &saveptr);
            char *w4 = strtok_r(NULL, " \t\v\f\r", &saveptr);
		    int figure1, figure2, figure3, figure4;
            
            if((w1 != NULL) && (w2 != NULL))
		    {
			    figure1 = atoi(w1);
			    figure2 = atoi(w2);
                figure3 = atoi(w3);
			    figure4 = atoi(w4);
		    }
                
            if(curLine == 3)
            {
                blockll_row = figure1;
                blockll_column = figure2;
                blockur_row = figure3;
                blockur_column = figure4;

                for (i=1; i<=row; i++)
                {
                    for (j=1; j<=column; j++)
                        create_vertex(&vertex_list[i][j], i, j, row, column, blockll_row, blockll_column, blockur_row, blockur_column);
                }
            }
            else
            {
                fill_row = figure1;
                fill_column = figure2;
                fiur_row = figure3;
                fiur_column = figure4;

                for (i=fill_row; i<=fiur_row; i++)
                {
                    for (j=fill_column; j<=fiur_column; j++)
                    {
                        for (k=0; k<vertex_list[i][j].degree; k++)
                        {
                            if ((vertex_list[i][j].adj_position[k][0] >= fill_row) && (vertex_list[i][j].adj_position[k][0] <= fiur_row) && (vertex_list[i][j].adj_position[k][1] <= fiur_column) && (vertex_list[i][j].adj_position[k][1] >= fill_column))
                                vertex_list[i][j].adj_position[k][2] = 0;
                            else
                                vertex_list[i][j].adj_position[k][2] = vertex_list[i][j].adj_position[k][2];
                        }
                    }
                }			
            }
	        curLine += 1;
        }
    }

	G = create_graph(row, column);
	G->alist = vertex_list;
		
    int i_free;
    for(i_free=0; i_free<column+1; i_free++)
        free((Vertex *)vertex_list[i_free]);

    free((Vertex *)vertex_list);

    for(i = 1; i <= row; i++)
	{
		for (j = 1; j <= column; j++)
		{
            for (k=0; k<(G->alist[i][j].degree); k++)
                printf("(%d, %d) --- %d --- (%d, %d)\n",i,j,(G->alist[i][j].adj_position[k][2]),(G->alist[i][j].adj_position[k][0]),(G->alist[i][j].adj_position[k][1]));
		}
	}
    //Conduct Dijkstra algorithm
    Dijkstra_Algorithm(G,  t_row,t_column, s_row, s_column);

    //free memory
    free(G);
	free(line);
    fclose(fp);

    exit(EXIT_SUCCESS);
}