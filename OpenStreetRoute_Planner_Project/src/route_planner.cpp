#include "route_planner.h"
#include <algorithm>

RoutePlanner::RoutePlanner(RouteModel &model, float start_x, float start_y, float end_x, float end_y): m_Model(model) 
{
    // Convert inputs to percentage:
    start_x *= 0.01;
    start_y *= 0.01;
    end_x *= 0.01;
    end_y *= 0.01;

    // TODO 2: Use the m_Model.FindClosestNode method to find the closest nodes to the starting and ending coordinates.
    // Store the nodes you find in the RoutePlanner's start_node and end_node attributes.
    start_node = &m_Model.FindClosestNode(start_x,start_y);
    end_node = &m_Model.FindClosestNode(end_x,end_y);

}


// TODO 3: Implement the CalculateHValue method.
// Tips:
// - You can use the distance to the end_node for the h value.
// - Node objects have a distance method to determine the distance to another node.

float RoutePlanner::CalculateHValue(RouteModel::Node const *node) 
{
    /*
        float distance(Node other) const {
            return std::sqrt(std::pow((x - other.x), 2) + std::pow((y - other.y), 2));
        }
    */
    return node->distance(*end_node);

    //equivalent to node->distance (*node).distance
}


// TODO 4: Complete the AddNeighbors method to expand the current node by adding all unvisited neighbors to the open list.
// Tips:
// - Use the FindNeighbors() method of the current_node to populate current_node.neighbors vector with all the neighbors.
// - For each node in current_node.neighbors, set the parent, the h_value, the g_value. 
// - Use CalculateHValue below to implement the h-Value calculation.
// - For each node in current_node.neighbors, add the neighbor to open_list and set the node's visited attribute to true.

void RoutePlanner::AddNeighbors(RouteModel::Node *current_node) 
{
    //Use the FindNeighbors() method of the current_node to populate current_node.neighbors vector with all the neighbors.

    //Find neighbor returns pt of Node type RouteModel::Node *RouteModel::Node::FindNeighbor(std::vector<int> node_indices)
    current_node->FindNeighbors();
    for(RouteModel::Node *neighbor:current_node->neighbors)
    {
        /* Publiuce var in class 
        Node * parent = nullptr;
        float h_value = std::numeric_limits<float>::max();
        float g_value = 0.0;
        bool visited = false;
        */

        //For each node in current_node.neighbors, set the parent, the h_value, the g_value. neighbor is pt
        //f=g+h 
        neighbor->parent = current_node;
        //Use CalculateHValue below to implement the h-Value calculation.
        neighbor->g_value = current_node->g_value + neighbor->distance(*current_node);
        neighbor->h_value = CalculateHValue(neighbor);
        //For each node in current_node.neighbors, add the neighbor to open_list and set the node's visited attribute to true.
        open_list.push_back(neighbor);
        //set the node's visited attribute to true.
        neighbor->visited = true;  

    }
}

// TODO 5: Complete the NextNode method to sort the open list and return the next node.
// Tips:
// - Sort the open_list according to the sum of the h value and g value.
// - Create a pointer to the node in the list with the lowest sum.
// - Remove that node from the open_list.
// - Return the pointer.
bool Compare_f_values_b(RouteModel::Node *firstNode,RouteModel::Node *secondNode)
{
        auto f1 = 0.0;
        auto f2 = 0.0; 
        f1 = firstNode->g_value + firstNode->h_value;
        f2 = secondNode->g_value + secondNode->h_value;
        return f1>f2;
}

RouteModel::Node *RoutePlanner::NextNode() 
{
    //See sort function
    std::sort(open_list.begin(),open_list.end(),Compare_f_values_b);
    //.front is reference
    RouteModel::Node *lowest_f = open_list.front(); 
    //.begin is index erase deletes idx
    open_list.erase(open_list.begin());
    return lowest_f;
}


// TODO 6: Complete the ConstructFinalPath method to return the final path found from your A* search.
// Tips:
// - This method should take the current (final) node as an argument and iteratively follow the 
//   chain of parents of nodes until the starting node is found.
// - For each node in the chain, add the distance from the node to its parent to the distance variable.
// - The returned vector should be in the correct order: the start node should be the first element
//   of the vector, the end node should be the last element.

std::vector<RouteModel::Node> RoutePlanner::ConstructFinalPath(RouteModel::Node *current_node) 
{
    // Create path_found vector
    distance = 0.0f;
    std::vector<RouteModel::Node> path_found;
    RouteModel::Node *temp_node = current_node;
    path_found.push_back(*current_node);

    // TODO: Implement your solution here.
    // - This method should take the current (final) node as an argument and iteratively follow the 
    //   chain of parents of nodes until the starting node is found.
    while(temp_node->parent != nullptr)
    {
        path_found.push_back(*temp_node->parent);
        // - For each node in the chain, add the distance from the node to its parent to the distance variable.
        distance += temp_node->distance(*temp_node->parent);
        temp_node = temp_node->parent;
    }

    //1st node is final we need to reverse path
    std::reverse(path_found.begin(),path_found.end());

    distance *= m_Model.MetricScale(); // Multiply the distance by the scale of the map to get meters.
    return path_found;

}


// TODO 7: Write the A* Search algorithm here.
// Tips:
// - Use the AddNeighbors method to add all of the neighbors of the current node to the open_list.
// - Use the NextNode() method to sort the open_list and return the next node.
// - When the search has reached the end_node, use the ConstructFinalPath method to return the final path that was found.
// - Store the final path in the m_Model.path attribute before the method exits. This path will then be displayed on the map tile.

void RoutePlanner::AStarSearch() 
{

    //open_list.emplace_back(start_node);
    RouteModel::Node *current_node = nullptr;
    current_node = start_node;
    current_node->visited=true;

    // TODO: Implement your solution here.
    /*
        1. Initialize empty set of opn nodes
        2. Initialize starting node with x,y,g and h (current coords and goal)
        3. Add new node to list of open nodes
        4. While list of open nodes in non empty
            a. Sort open list by f value 
            b. Pop opimal cell 
            c. Mark cells coordinates 
            d. If current cell is goal return grid 
            e. else expand cell to neighbors 
        5. Havent found path if open nodes is empty

    */

   while(!open_list.empty())
   {
       //a. Sort open list by f value and current_node = b.
       current_node = RoutePlanner::NextNode();
       //d. If current cell is goal return grid 
       //probably need another solution, not a fan of comparing floats/doubles like this
       if(current_node->distance(*end_node) == 0)
       {
           m_Model.path = RoutePlanner::ConstructFinalPath(current_node);
            
       }
       else
       {
           AddNeighbors(current_node);
       }
       
   }
    
}