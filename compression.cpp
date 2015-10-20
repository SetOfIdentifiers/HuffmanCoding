/***********************************************************************************************************/
/*Compression.cpp
/*Created: 2012
/***********************************************************************************************************/

#include "Compression.h"
#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <memory>

/***********************************************************************************************************/

const int UNSIGNED_CHAR_BOUND = std::numeric_limits<unsigned char>::max() + 1;

/*!
    Clears the frequency array.
*/
Huffman::Huffman()
{
    std::memset(&frequency, 0, sizeof(int) * UNSIGNED_CHAR_BOUND);
}

/*!
    A 'help' function used to call the 'encodeNode' function with the private data member 'frequencyTree'.
*/
void Huffman::encode(Node<unsigned char> *frequency_tree)
{
    encode_node(frequency_tree, std::vector<unsigned char>());
    delete frequency_tree;
}

/*!
    Recursive function that traverses a set of nodes (from a binary tree) and yields the corresponding 
    binary number of their data value, using the huffman algoritm.
*/
void Huffman::encode_node(Node<unsigned char> *node, std::vector<unsigned char> encoded_character)
{
    if (!node->left && !node->right)
        huffman_table[node->data] = std::vector<unsigned char>(encoded_character);
    else
    {
        encoded_character.push_back('0');
        encode_node(node->left, std::vector<unsigned char>(encoded_character));

        encoded_character[encoded_character.size() - 1] = '1';
        encode_node(node->right, std::vector<unsigned char>(encoded_character));
    }
}

/*!
    Function that takes a text file and parses its contents, then stores retrieved information regarding the number
    (frequency) of encounters of each character in an array. This array is used to create a binary tree which
    in turn is used to create a huffmantable consisting of bitstrings for every character. The original file 
    contents are then looked up in the huffman table and the lookup is written to the binary file. In order to decompress 
    successfully in the future, the frequency table is saved to a different binary file along with the size of the contents.
    The other file is known as the header file and contains information necessary for decompression.
*/
bool Huffman::compress(std::string in, std::string out)
{
    std::fstream file;          // file stream
    std::vector<unsigned char> contents; // Store the values from the initial file
    std::string temp;           // String used for writing bytes
    int bits = 0;               // Size of the compressed file in bits, will be put in header
    int file_size = -1;

    file.open(in.c_str(), std::ios_base::in | std::ios_base::binary);

    if(!file.good())
        return false;

    // Parse the file and calculate frequency

    file.seekg(0, file.end);
    file_size = file.tellg();
    file.seekg(0, file.beg);

    int size_counter = 0;
    
    while(size_counter < file_size) 
    {
        unsigned char curr_char = file.get();
        contents.push_back(curr_char);
        frequency[curr_char]++;
        size_counter++;
    }

    file.close();
    
    // Create list with all the characters actually in the collection
    encode(create_tree());


    // Create compressed version
    file.open(out.c_str(), std::ios_base::out | std::ios_base::binary);

    for(int i = 0; i < contents.size(); i++)
        for(int j = 0; j < huffman_table[contents[i]].size(); j++)
        {
            temp.push_back(huffman_table[contents[i]][j]);
            bits++;
            
            if(temp.length() == 8)
            {
                unsigned char byte = 0x0;

                for(unsigned int bit = 0; bit < 8; bit++)
                    if(temp[bit] == '1')
                        byte = byte | (0x1 << bit);

                file << (unsigned char)byte;	
                temp.clear();
            }
        }

    if(!temp.empty())
    {
        unsigned char last_byte = 0x0;

        // Bits was not divisible by 8, write the last byte with some trash info
        for(int last_bits = 0; last_bits < temp.length(); last_bits++)
            if(temp[last_bits] == '1')
                last_byte |= (0x1 << last_bits);

        file << (unsigned char)last_byte;
    }

    file.close();

    // Write header to a separate file
    file.open(out + HEADER_FILE_EXTENSION, std::ios_base::out | std::ios_base::binary);

    HuffmanMetaInfo huffman_header;
    std::memcpy(&(huffman_header.character_frequency), &frequency, UNSIGNED_CHAR_BOUND * sizeof(int));
    huffman_header.size = bits;

    file.write((const char*)(&huffman_header), sizeof(HuffmanMetaInfo));
    file.close();

    return true;
}

/*!
    Reads the 'header'-file belonging to the already compressed (binary)file and collects frequency for the different characters 
    as well as the number of bits to be read. After that information has been processed 
    and stored, the function proceeds to parse byte after byte of the compressed file in order to 
    create a bitstring consisting of all the bits in the binary file. The frequency information previously collected
    is used to recreate the very same Huffman table that was used to compress the file, the bitstring is then translated
    using the huffman table and written to a file.
*/

bool Huffman::decompress(std::string in, std::string out)
{
    std::fstream file;          // File stream
    HuffmanMetaInfo new_header;  // Data structure containing read header
    int curr_size = 0;           // Counter to compare bits to the bits in the header
    std::string huffman_string;  // A bitstring to be translated into proper chars

    // Process the header to parse reconstruction information.
    file.open(in + HEADER_FILE_EXTENSION,  std::ios_base::in | std::ios_base::binary);

    if(!file.good())
        return false;

    file.read((char*)(&new_header), sizeof(HuffmanMetaInfo));
    memcpy(&frequency, &(new_header.character_frequency), UNSIGNED_CHAR_BOUND * sizeof(int));
    file.close();

    // Read the rest of the file
    file.open(in, std::ios_base::in | std::ios_base::binary);

    if(!file.good())
        return false;

    while(curr_size < new_header.size)
    {
        unsigned char input_byte = file.get();

        // Extract the individual bits.

        for(unsigned int bits = 0; bits < 8; bits++)
        {
            unsigned char masked_byte = input_byte & (0x1 << bits);

            if(masked_byte != 0x0)
                huffman_string += "1";
            else
                huffman_string += "0";

            if(++curr_size > new_header.size)
                break;
        }
    } 
    
    file.close();

    // Create Huffmantable from retreived frequencies
    encode(create_tree());

    // Create decompressed file
    file.open(out, std::ios_base::out | std::ios_base::binary);

    std::vector<unsigned char> bit_string; 
    
    for(int i = 0; i < huffman_string.size(); i++)
    {
        bit_string.push_back(huffman_string[i]);
        
        for(int x = 0; x < UNSIGNED_CHAR_BOUND; x++)
        {
            if(huffman_table[x] == bit_string)
            {
                file << (unsigned char)x;
                bit_string.clear();
                break;
            }
        }
    }

    file.close();

    return true;
}

/*!
    A help function that returns a a binary tree,
    Nodes are created for each occuring character in the original file with a weight equal to the number of
    times it is encountered in the file. These nodes are inserted into a priority queue
    so the node with least weight always is at top, the priority queue is then popped twice
    and the two nodes that were popped are inserted at a root node which is then reinserted into the priority queue.
    This process continues until there is only node in the tree, that node is the Huffman tree.
*/
Node<unsigned char>* Huffman::create_tree()
{
    std::priority_queue<Node<unsigned char>, std::vector<Node<unsigned char>*>, CompareNode<unsigned char>> trees;

    for(int i = 0; i < UNSIGNED_CHAR_BOUND; i++)
        if(frequency[i] > 0)
            trees.push(new Node<unsigned char>((unsigned char)i, frequency[i]));

    Node<unsigned char> *node_a, *node_b;
    
    while(trees.size() > 1)
    {
        Node<unsigned char> *node_c = new Node<unsigned char>('\0', 0);

        node_a = trees.top();
        trees.pop();
        node_b = trees.top();
        trees.pop();

        node_c->left = node_a;
        node_c->right = node_b;

        trees.push(node_c);
    }

    return trees.top();
}