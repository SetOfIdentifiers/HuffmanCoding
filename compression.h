/***********************************************************************************************************/
/*Huffman.h
/*Created: 2012
/***********************************************************************************************************/

#ifndef HUFFMAN_H_
#define HUFFMAN_H_

#include "Node.h"
#include <vector>
#include <iostream>

const std::string HEADER_FILE_EXTENSION = ".header";

/*!
    A struct for the file header, containing information to reconstruct the tree of a compressed file.
*/
struct HuffmanMetaInfo
{
    int size;
    int character_frequency[256];
};


/*!
    The huffman class containing functions to compress an ASCII file into a binary file, and decompress a binary file into an ASCII file
*/
class Huffman
{
private:
    int frequency[256];
    std::vector<unsigned char> huffman_table[256];
    Node<unsigned char>* create_tree();
    void encode(Node<unsigned char> *frequency_tree);
    void encode_node(Node<unsigned char> *node, std::vector<unsigned char> encoded_character);

public:

    Huffman();

    bool compress(std::string in, std::string out);
    bool decompress(std::string in, std::string out);
};

#endif