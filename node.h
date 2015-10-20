/***********************************************************************************************************/
/*Node.h
/*Created: 2012
/***********************************************************************************************************/

#ifndef NODE_H
#define NODE_H

/*!
    Node in a binary tree.
*/
template <typename Type>
class Node
{
public:

    Type data;
    Node<Type> *left;
    Node<Type> *right;
    int frequency_of_occurance;

    Node()
    {
        data = 0;
        frequency_of_occurance = 0;
        left = 0;
        right = 0;
    }

    ~Node()
    {
        if(left)
        {
            delete left;
            left = 0;
        }

        if(right)
        {
            delete right;
            right = 0;
        }
    }

    Node(char char_data, int input_frequency_of_occurance)
    {
        data = char_data;
        frequency_of_occurance = input_frequency_of_occurance;
        left = 0;
        right = 0;
    }

    /*!
        Recursively determines the weight of a tree by calculating the sum of the weight of its nodes.
    */
    int weight() const
    {
        int weight_sum = frequency_of_occurance;

        if(left)
            weight_sum += left->weight();

        if(right)
            weight_sum += right->weight();
        
        return weight_sum;
    }

};

/*!
    Comparator used in order for std::priority_queue to function.
*/
template <typename Type>
struct CompareNode
{
    bool operator() (const Node<Type> & node1, const Node<Type> & node2) const
    {
        return node1.weight() > node2.weight();
    }

    bool operator() (const Node<Type> * node1, const Node<Type> * node2) const
    {
        return node1->weight() > node2->weight();
    }
};

#endif