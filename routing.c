/**************************************************************************
**
**  Writen By: Michal Benson
**
**  Purpose: This is an simulation of the Dijkstra routing algorhithm
**
**  Input: Takes a file reference to an NxN input matrix N being the number
**	    of nodes in the network, and each pair (i,j) the cost of the 
**	    direct link travling from i to j.  If the two nodes are not
**	    directly connected to eachother, the value of (i,j) needs to
**	    be 'X'.  If a node is referencing its self (i=j), the value 
**	    should be entered as 0.  
**	    By default, the program looks for inputMatrix.txt
**
**  Output: Will write out to a file, the routing table, path, and cost
**	    from the root node in the input network at the conclusion 
**	    of the algorhithm.
**************************************************************************/

#include <cstdlib>
#include <iostream>
#include <fstream>

using namespace std;

int sumPath(int ** routeTable, int ** network, int node);
void dijstra(int nodes, int rootNode, int** network, int** routeTable);
int lengthOfNext(ifstream& inStream);
void readInTable(const char* INPUT_FILE, ifstream& inStream, int nodes, int **network);
int getNodes(const char* INPUT_FILE, ifstream& inStream);

int main (int noArgs, char *args[])
{
    // Initialize needed variables
    int nodes = 0;
    int rootNode = 0;
    int ** network;
    int ** routeTable;

    // Set the default input file name
    const char INPUT_FILE[] = "inputMatrix.txt";

    // Create the inpnt stream object
    ifstream inStream;

    // Determine the number of nodes in the network
    nodes = getNodes(INPUT_FILE, inStream);

    // Initialize our route table (create a NxN matrix in memory)
    network = new int* [nodes];
    for (int i = 0; i < nodes; i++)
    {	network[i] = new int[nodes]; }
    
    // Read in the routing table
    readInTable(INPUT_FILE, inStream, nodes, network);

    // Initialize our output table for the algorhithm
    /* We only need to keep track of the path 
    ** We cannot do this dynamicly later to exactly fit the
    ** length of the path as we would lose the table to scope*/
    routeTable = new int* [nodes];
    for (int i = 0; i < nodes; i++)
    {	routeTable[i] = new int[nodes];   }

    // Put NULL's in all cells in the routeTable it initialize it
    for (int i = 0; i < nodes; i++)
    {	for ( int j = 0; j < nodes; j++ )
	{   routeTable[i][j] = -1;	}
    }

    // Run the algorhithm
    dijstra(nodes, rootNode, network, routeTable);

    // Print out the results
    cout << endl << "FINAL ROUTE TABLE" << endl;

    for (int i = 0; i < nodes; i++)
    {	for (int j = 0; j < nodes; j++)
	{   if (routeTable[i][j] != -1)
	    {	cout << routeTable[i][j] << ' '; }
	}
	cout << " Cost : " << sumPath(routeTable, network, i);
	cout << endl;
    }

    return 0;
}

/****************************************************************************
**  getNodes(INPUT_FILE, inStream)
**
**  Determines the number of nodes in a network represented by a NxN matrix
**
**  Needs: Needs a const char[] containing the name of the file containing
**	    the input matrix describing the network
**	    Also needs a reference to the inStream object 
**
**  Returns: Returns an int value representing the number of nodes described
**		in the input file
**
**  Affects: NONE
**
****************************************************************************/

int getNodes(const char* INPUT_FILE, ifstream& inStream)
{
    int nodes = 0;
    // Open the input stream file
    inStream.open(INPUT_FILE);

    // Determine the number of nodes in the network
    while (inStream.peek() != '\n')
    {
	// if the next character is not a ' ', increment nodes
	if (inStream.peek() != ' ') 
	    { nodes++; }

	// discard the node we just looked at
	inStream.ignore(1);
	
	// if the next character is not a space, there was a multi
	// number node value, so discard this value also
	// DO NOT DISCARD THE \n CHARACTER
	if (inStream.peek() != ' ' && inStream.peek() != '\n') 
	    { inStream.ignore(1); }

	// Discard the ' ' that is now up next in the stream
	if ( inStream.peek() == ' ') { inStream.ignore(1); }
    }
    
    // Close the input file
    inStream.close();
    
    // Return what we found 
    return nodes;
}

/****************************************************************************
** readInTable(INPUT_FILE, inStream, nodes, **network)
**
**  Utility function for importing an nxn matrix from a specified file and
**	writing it into memory.  Needed for numbers represented by > 1 digit
**
**  Needs:  - A const char[] naming the input file to be used
**	    - A reference to the inStream object
**	    - Number of nodes in the input file
**	    - A double int pointer to the location of the network table.  
**		This must already have all the space allocated 
**		(the NxN blocks of memory) in an array style format 
**		[nodes]x[nodes]
**
**  Returns: NONE
**
**  Affects: Writes the contents of INPUT_FILE into memory at network
**
****************************************************************************/

void readInTable(const char* INPUT_FILE, ifstream& inStream, int nodes, int **network)
{
    // Initialize our memory space for the routing table
    char temp;

    // open the input file
    inStream.open(INPUT_FILE);

    // Build our route table
    for (int i = 0; i < nodes; i++)
    {	for (int j = 0; j < nodes; j++)
	{   
	    // temp int variable for unit conversion
	    int temp;

	    // Ignore all newline characters
	    while (inStream.peek() == '\n' || inStream.peek() == ' ')
		{ inStream.ignore(1); }

	    // Once inStream produces a non return character //

	    // Get and set the length of the next item in the input file
	    int length = lengthOfNext(inStream);
	    // Create a temp array of the above length for use later
	    char tempArray[length];

	    // Check if the next character is X
	    // If temp = X the value is set to -1 and discard X
	    if ( inStream.peek() == 'X' ) 
	    { 
		temp = -1; 
		inStream.ignore(1);
	    }
	    // If the next character is not X, read all the characters before
	    //  the next white space (' ' or \n) and put them in an array
	    else
	    {
		for (int i = 0; i < length; i++)
		    { tempArray[i] = inStream.get(); }
		// Convert this array to a int.
		temp = atoi(tempArray);
	    }
	    // Save the result from above in (i,j)
	    network[j][i] = temp;
	}
    }
    // Close the input file, we are done with it
    inStream.close();
}


/****************************************************************************
** lengthOfNext(inStream)
**
**  Utility function for file importing.  Determines the length of the next
**	token in the input stream delimited by ' ' and or \n
**
**  Needs:  - A reference to the inStream object which must already be open
**		to a file.
**
**  Returns: - An int representing the number of characters in the next word
**		delimited by ' ' or \n
**
**  Affects: NONE - The stream is returned to it starting location upon
**		    completion
**
****************************************************************************/

int lengthOfNext(ifstream& inStream)
{
    // Init the needed variables
    int length = 0;

    // Mark the input stream with temp for returning later
    int mark = inStream.tellg();
    
    // Look for the next ' ' or \n character
    while (inStream.peek() != ' ' && inStream.peek() != '\n')
    {
	// If ' ' or \n is not next, throw the character away
        inStream.ignore(1);
	// Increemnt the length counter for this entity
        length++;
    }

    // Return the stream to its starting location and return length
    inStream.seekg(mark);
    return length;
}

/****************************************************************************
** dijstra(nodes, rootNode, **network, **routeTable)
**
**  Implementation of the dijstra routing algorithm
**
**  Needs:  int nodes - the number of nodes in the network
**	    int rootNode - the number of the root node (must be < nodes)
**	    int **network - the NxN connectivity matrix describing this network
**	    int **routeTable - the pre-initialized NxN table to save the 
**		determined routes to
**
**  Returns: None
**
**  Effects: All data in int **routeTable is overwritten by this function
**
****************************************************************************/
void dijstra(int nodes, int rootNode, int** network, int** routeTable)
{
    bool done = false;
    
    // Initialize needed talbes
    int ** tenative;
    
    // Create our tenative node list
    tenative = new int* [nodes];
    for ( int i = 0; i < nodes; i++ )
    {	tenative[i] = new int [nodes];    }

    // -1 out all entries in the tenative list
    for ( int i = 0; i < nodes; i++ )
    {	for ( int j = 0; j < nodes; j++ )
	{   tenative[i][j] = -1;  }
    }

    // Set the root node route in the route table
    routeTable[rootNode][0] = 0;
    // Start the algorhithm by looking at the root node
    int currentNode = rootNode;

    // Start algorhithm
    while ( done == false)
    {
	// Incriment though all neighbor nodes of the current node
	for ( int neighbor = 0; neighbor < nodes; neighbor++ )
	{
	    // Skip all neighbors that are already in the routeTable or not connected at all
	    if ( routeTable[neighbor][0] == -1 && network[currentNode][neighbor] > 0 )
	    {
		// If not in route table and neighbor is connected to current
		// Determine the route value from the root to the neighbor node
		//   along this path and along the tenative path (if any)
		int pathCost = sumPath(routeTable, network, neighbor) + network[currentNode][neighbor];
		int tenativeCost = sumPath(tenative, network, neighbor);
		
		// If this value is lower than the value in tenative for getting to neighbor
		// Or there is no route to the neighbor node in tenative yet
		if ( pathCost < tenativeCost || tenative[neighbor][0] == -1)
		{
		    // First clear out the entry in tenative
		    for ( int i = 0; i < nodes; i++)
		    {	tenative[currentNode][i] = -1;    }

		    // Write the lower cost path to the tenative table
		    int i = 0;
		    while ( routeTable[currentNode][i] != -1 )
		    {	
			tenative[neighbor][i] = routeTable[currentNode][i]; 
			i++;
		    }
		    tenative[neighbor][i] = neighbor;
		}

	    }
	}// done scanning neighbors

	int cost = -1;
	int leastCost = -1;
	int leastCostNode = -1;

	// After checking all neighbor nodes, scan tenative for the lowest cost path
	// That is not alredy in route table
	for ( int node = 0; node < nodes; node++)
	{   
	    if ( routeTable[node][0] == -1 )
	    {	
		cost = sumPath(tenative, network, node);
		if ( (cost < leastCost || leastCost == -1) && cost > 0 )
		{   leastCostNode = node; 
		    leastCost = cost;
		}
	    }
	}
	// If we found no better routes, we are done
	if ( leastCostNode == -1 )
	{   done = true; }
		
	// Move the lowest cost path to routeTable
	if ( leastCostNode != -1 )
	{   for ( int i = 0; i < nodes; i++)
	    {	routeTable[leastCostNode][i] = tenative[leastCostNode][i]; }
	}
	// Set the next node to scan as the neighbor whose path was just moved
	currentNode = leastCostNode;
    } // End algorhithm
}


/****************************************************************************
** sumPath(**routeTable, **network, node)
**
**  Utility function for determiningn the cost of a path to a node.  The
**	path being specified by routeTable[node][*], the cost of each hop
**	specified by network[source][dest]
**
**  Needs:  int **routeTable - a table describing a route to int node.  The
**		end of the route must be specified by a -1
**	    int **network - NxN connectivity table describing the network
**	    int node - The node to determine the path cost to based on 
**		int **routeTable
**
**  Returns: int cost - the positive cost for the path to the node based on
**		routeTable.  It returns 0 if their is either no existing route
**		to int node, or if the node is connected directly to its self
**
**  Effects: None
**	    
****************************************************************************/

int sumPath(int ** routeTable, int ** network, int node)
{
    int cost = 0;
    int i = 0;
    int source = 0;
    int dest = 0;
    while ( routeTable[node][i] != -1 )
    {
	// Our destination is now the next node in the routeTable
	dest = routeTable[node][i];
	// Increment cost by the cost to go from the source to the destination
	cost += network[source][dest];
	// Our destination is now our souce node
	source = dest;
	i++;
    }

    return cost;
}
