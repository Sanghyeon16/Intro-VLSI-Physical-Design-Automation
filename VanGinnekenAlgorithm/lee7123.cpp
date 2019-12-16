#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <cstdlib>
#include <string>

#include <vector>
#include <list>
#include <map>

using namespace std;

int NumOfSinks=0;

const int WireResistance = 1; //resistance per unit length
const int WireCapacitance = 1; //capacitance per unit length

const int BufferResistance = 1;
const int BufferCapacitance = 1;
const int BufferIntrinsicDelay = 1;

const int DriverResistance = 1;

struct Node {
    int index;
    Node* left;
    Node* right;
    double left_length, right_length;

    //solution pair of capacitance(C) and required arrival time(RAT)
    list<pair<double,double>> solutions; 
    //solution pair to track insertion lists
    map< pair<double,double>,list< pair<int,int> > > candidates_map; 
};

struct Edge {
    int upstream;
    int downstream;
    double length;
    bool visited;
};

void BuildingTree(Node* node, vector<Edge> &edges, vector<Node> sinks)
{
    //check visit
    bool visited = true;
    for (size_t i =0; i<edges.size(); i++)
    {
        if(edges[i].visited==false)
            visited = false;
    }

    //set index 0 to sink 0
    if(node->index>0 && node->index <= NumOfSinks)
    {
        Node sink = sinks[node->index - 1];
        node->left = NULL;
        node->right = NULL;
        node->solutions = sink.solutions;
    }
    if(!visited)
    {
        for( size_t i = 0; i< edges.size(); i++)
        {
            if(edges[i].upstream == node->index)
            {
                edges[i].visited = true;
                if (node->left != NULL && node->right == NULL)
                {
                    node->right = new Node;
                    node->right->index = edges[i].downstream; //link using edge connection
                    node->right->left = NULL;
                    node->right->right = NULL;
                    node->right_length = edges[i].length;
                    
                    BuildingTree(node->right, edges, sinks);
                }
                else if (node->right != NULL && node->left == NULL)
                {
                    node->left = new Node;
                    node->left->index = edges[i].downstream;
                    node->left->left = NULL;
                    node->left->right = NULL;
                    node->left_length = edges[i].length;
                    
                    BuildingTree(node->left, edges, sinks);
                }
                else if (node->right == NULL && node->left == NULL)
                {
                    // insert to left child by default
                    node->left = new Node;
                    node->left->index = edges[i].downstream;
                    node->left->left = NULL;
                    node->left->right = NULL;
                    node->left_length = edges[i].length;
                    
                    BuildingTree(node->left, edges, sinks);
                }
                else
                {
                    cout << "[System]: Input file has an issue for building a binary tree" << endl;
                    exit(1);
                }

            }
        }
    }
    else
        return;
}


Node* Parsing(char* fileName)
{
    ifstream inFile(fileName);
    if(inFile.fail())
    {
        cerr << "[System]: Input file open error" << endl;
        inFile.close();
        exit(1);
    }

    //The first line - the number of sinks "n"
    string line;
    getline(inFile, line);
    
    istringstream streaml (line, istringstream:: in);
    streaml >> NumOfSinks;
    streaml.clear();
    
    //From the number of sinks n, initialize the root of the binary tree
    Node* root = new Node;
    root->index = 0;
    root->left = NULL;
    root->right = NULL;

    //Line 2 to line n+1 are information for sink 1 to n - sink's C and RAT "C RAT"
    vector<Node> sinks; //vector container to manage memory
    for(int i=1; i<=NumOfSinks; i++)
    {
        double capacitance = 0.0;
        double load = 0.0;
        Node node;
        getline(inFile, line);
        
        istringstream streamll (line, istringstream::in);
        streamll >> capacitance >> load;
        
        node.index = i;
        node.solutions.push_back(make_pair(capacitance, load)); //index ordered C and RAT(load) pair
        sinks.push_back(node);  //index ordered sinks
        streamll.clear();
    }

    //Line n+2 to line (n+2 + 2n -1) are edge information 
    //- upstream node index, downstream node index, and edge length "i j length"
    vector<Edge> edges;
    while(getline(inFile, line))
    {
        Edge edge;
        int upstream = 0;
        int downstream = 0;
        double length = 0;
        
        istringstream streamll (line, istringstream::in);
        streamll >> upstream >> downstream >>length;
        
        edge.upstream = upstream;
        edge.downstream = downstream;
        edge.length = length;
        edge.visited = false;
        edges.push_back(edge); //index ordered edges
        streamll.clear();
    }

    inFile.close();

    BuildingTree(root, edges, sinks);

    return root;
}

// Add Wire
void AddWire(list< pair<double,double> > &candidates, double length, Node* upstream, Node* downstream)
{
    if (!candidates.empty())
    {
        for(list< pair<double,double> >::iterator it = candidates.begin(); it!=candidates.end(); ++it)
        {
            list< pair<int,int> > wireAdd = downstream->candidates_map[*it];
            //q2 = q1 - rcx^2/2 - rxc1
            it->second = it->second - WireResistance * WireCapacitance * length * length / 2 - WireResistance * length * it->first;
            //c2 = c1 + cx
            it->first = it->first + WireCapacitance * length;

            wireAdd.push_back(make_pair(upstream->index, 0));
            upstream->candidates_map[*it] = wireAdd;
        }
    }
    else 
        return;
}

//Insert Buffer
void InsBuffer(list< pair<double,double> > &candidates, Node* upstream)
{
    if(!candidates.empty())
    {
        list< pair<double,double> > tmp = candidates;
        for (list< pair<double,double> >::iterator it=tmp.begin(); it!=tmp.end(); ++it)
        {
            list< pair<int,int> > bufferInsertion = upstream->candidates_map[*it];
            //q1b = q1 - rbc1 - tb
            it->second = it->second - BufferResistance * it->first - BufferIntrinsicDelay;
            //c1b = cb
            it->first = BufferCapacitance;
            candidates.push_back(*it);
            bufferInsertion.push_back(make_pair(upstream->index, 1));
            upstream->candidates_map[*it] = bufferInsertion;
        }
    }
    else
        return;
}

bool cCompare(const pair<double,double> &leftSubTree, const pair<double,double> &rightSubTree)
{
    return leftSubTree.first < rightSubTree.first;
}

void Pruning(list< pair<double,double> > &candidates)
{
    candidates.sort(cCompare);
    //c1 < c2 < c3
    //  q1 < q2 < q3
    list< pair<double,double> >::iterator it = candidates.begin();
    list< pair<double,double> >::iterator new_it = it;
    ++new_it;
    while (new_it!=candidates.end())
    {
        if(it->first<new_it->first)
        {
            if(it->second>=new_it->second)
                new_it = candidates.erase(new_it); //prune inferior candidate
            else
            {
                it = new_it;
                ++new_it;
            }
        }
        else if (it->first==new_it->first)
        {
            if(it->second<new_it->second)
            {
                it = candidates.erase(it);
                ++new_it;
            }
            else
                new_it = candidates.erase(new_it);
        }
        else
            cout << "[System]: Pruning error occured" <<endl;
    }
    return;
}

list< pair<double,double> > Merging(list< pair<double,double> > leftSubTree, list< pair<double,double> > rightSubTree, Node* upstream)
{
    list< pair<double,double> > MergedCandidates;
    list< pair<double,double> >::iterator it_l=leftSubTree.begin();
    list< pair<double,double> >::iterator it_r=rightSubTree.begin();
    while (it_l!=leftSubTree.end() && it_r!=rightSubTree.end())
    {
		list<pair<int,int> > LeftInsertion = upstream->candidates_map[*it_l];
        list<pair<int,int> > RightInsertion = upstream->candidates_map[*it_r];
        
        for (list< pair<int, int> >::iterator it=RightInsertion.begin(); it != RightInsertion.end();++it)
            LeftInsertion.push_back(*it);

		list<pair<int,int> > MergedInsertion = LeftInsertion;
        
        //cmerge = cl + cr
        //qmerge = min(ql, qr)
        if (it_l->second < it_r->second) {
			MergedCandidates.push_back(make_pair(it_l->first+it_r->first, it_l->second));
			upstream->candidates_map[make_pair(it_l->first + it_r->first, it_l->second)] = MergedInsertion;
			++it_l;
		}
		else {
			MergedCandidates.push_back(make_pair(it_l->first+it_r->first, it_r->second));
			upstream->candidates_map[make_pair(it_l->first + it_r->first, it_r->second)] = MergedInsertion;
			++it_r;
		}
    }
    return MergedCandidates;
}

list< pair<double,double> > VanG_algorithm(Node* node)
{   //node is a sink
    if (node->index>0 && node->index<=NumOfSinks)
    {
        //candidates_map initialization
        list< pair<double,double> > candidates = node->solutions;
        list< pair<int,int> > BufferCandidates;

        for (list< pair<double,double> >::iterator it = candidates.begin(); it != candidates.end(); ++it)
        {
            BufferCandidates.push_back(make_pair(node->index, 0));
            node->candidates_map[*it] = BufferCandidates;
        }
        return candidates;
    }//node is a steiner node
    else
    {
        //Van Ginneken's Algorithm
        //Start from sinks
        //Candidate solutions are propagated toward the source
        list< pair<double,double> > LeftCandidates;
        list< pair<double,double> > RightCandidates;

        if(node->left != NULL)
        {
            LeftCandidates = VanG_algorithm(node->left);
    
            AddWire(LeftCandidates, node->left_length, node, node->left);
            InsBuffer(LeftCandidates, node);
            Pruning(LeftCandidates);
        }
        if(node->right!=NULL)
        {
            RightCandidates = VanG_algorithm(node->right);
            AddWire(RightCandidates, node->right_length, node, node->right);
            InsBuffer(RightCandidates, node);
            Pruning(RightCandidates);
        }
        if(node->left!=NULL && node->right!=NULL)
        {
            list< pair<double,double> > candidates = Merging(LeftCandidates, RightCandidates, node);
            Pruning(candidates);
            return candidates;
        }
		else if (node->left!=NULL&&node->right==NULL) {
			return LeftCandidates;
		}
        else if (node->left==NULL&&node->right!=NULL) {
			return RightCandidates;
		}
        else
        {
            cout << "[System]: Input file structure error ( not sink )" << endl;
            exit(2);
        }
    }
}

void AddDriver(list<pair<double,double> > &candidates, Node* node) {
	if (!candidates.empty()) {
		for (list<pair<double,double> >::iterator it = candidates.begin(); it != candidates.end(); ++it) {
			list<pair<int,int> > driver_insertion = node->candidates_map[*it];
            //q0d = q0 - Rdc0 = SlackMin
			it->second = it->second - DriverResistance * it->first;
			node->candidates_map[*it] = driver_insertion;
		}
	}
	else {
		cout << "There is no candidate!" << endl;
		return;
	}
}



int main(int argc, char *argv[])
{
    char* fileName = argv[1];
    int buffer_number = 0;
    double RAT = 0;

    //Build a tree from input data
    Node* root = Parsing(fileName);

    //Van Ginneken's Algorithm
    root->solutions = VanG_algorithm(root);

    //Pick solution with max SlackMin
    AddDriver(root->solutions, root);
    Pruning(root->solutions);
    
    pair<double,double> solution = root->solutions.back();
    
    list<pair<int,int> > insertion_list = root->candidates_map[solution];
    for (list<pair<int,int> >::iterator it = insertion_list.begin(); it != insertion_list.end(); ++it) {
		if (it->second == 1) {
			buffer_number++;
		}
	} 

    list<pair<int, int> > buffer_list;
	for (list<pair<int, int> >::iterator it = insertion_list.begin(); it != insertion_list.end(); ++it) {
		if (it->second == 1) {
			buffer_list.push_back(*it);
		}
	}

    RAT = solution.second;

    
    //Generate Output file
    ofstream outfile;
    outfile.open ("Output");
    outfile << "Total number of buffers inserted : " << buffer_number << endl;
    
    outfile << "The nodes where buffers are inserted : " << endl;
    for (list< pair<int, int> >::iterator it = buffer_list.begin(); it != buffer_list.end(); ++it)
        outfile << "One buffer is inserted at node " << it->first << endl;
    
    outfile << "The required arrival time at the driver node : " << RAT << endl;
    outfile.close();

    return 0;


}