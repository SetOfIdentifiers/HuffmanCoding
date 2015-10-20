/***********************************************************************************************************/
/*main.cpp
/*Created: 2012
/***********************************************************************************************************/

#include <iostream>
#include <string>
#include "Compression.h"

/*Constants*************************************************************************************************/

const char COMPRESS_COMMAND = 1;
const char DECOMPRESS_COMMAND = 2;
const char EXIT_COMMAND = 3;
const char HELP_COMMAND = 4;
const int NUM_COMMANDS = 5;
const std::string COMPRESS_COMMAND_TEXT = "(1) Compress\n";
const std::string DECOMPRESS_COMMAND_TEXT = "(2) Decompress\n";
const std::string EXIT_COMMAND_TEXT = "(3) Exit\n";
const std::string HELP_COMMAND_TEXT = "(4) Help\n";
const std::string INVALID_COMMAND_MESSAGE = "Invalid command\n";
const std::string COMPRESS_COMMAND_MESSAGE = "Enter path to input file for compression:\n";
const std::string DECOMPRESS_COMMAND_MESSAGE = "Enter path to the output file for decompression:\n";
const std::string COMPRESSION_COMPLETED_MESSAGE = "Compression completed.\n";
const std::string DECOMPRESSION_COMPLETED_MESSAGE =" Decompression completed\n";
const std::string HELP_COMMAND_MESSAGE = "When compressing a file, the output is always a file called compressed in the \nsame directory as the application. When decompressing, this file is always used.";
const std::string FILE_NOT_FOUND = "File not found\n";
const std::string COMMAND_INPUT_TEXT = "Input command:\n";
const std::string COMPRESSED_FILE_NAME = "compressed";

typedef void (*Command)(void);

Command commands[NUM_COMMANDS]; // Array with function pointers to each recognized command

/*Functions*************************************************************************************************/


/*!
    Performs the exit command
*/
void exit_command()
{
    exit(0);
}

/*!
    Performs the help command
*/
void help_command()
{
    std::cout << HELP_COMMAND_MESSAGE;
}

/*!
    Performs the compress command
*/
void compress_command()
{
    Huffman huffman_compress = Huffman();
    std::string filename;

    std::cout << COMPRESS_COMMAND_MESSAGE;
    std::cin >> filename;

    if(huffman_compress.compress(filename, COMPRESSED_FILE_NAME))
        std::cout << COMPRESSION_COMPLETED_MESSAGE;
    else
        std::cout << FILE_NOT_FOUND;
}

/*!
    Performs the decompress command
*/
void decompress_command()
{
    Huffman huffman_decompress = Huffman();
    std::string filename;

    std::cout << DECOMPRESS_COMMAND_MESSAGE;
    std::cin >> filename;

    if(huffman_decompress.decompress(COMPRESSED_FILE_NAME, filename))
        std::cout << DECOMPRESSION_COMPLETED_MESSAGE;
    else
        std::cout << FILE_NOT_FOUND;
}

/*!
    Prints a listning of all the commands
*/
void list_commands()
{

    std::cout << COMPRESS_COMMAND_TEXT << DECOMPRESS_COMMAND_TEXT
              << EXIT_COMMAND_TEXT	   <<  HELP_COMMAND_TEXT 
              << COMMAND_INPUT_TEXT;
}

/*!
    Initialize the function pointers in the command array
*/
void register_commands()
{
    commands[COMPRESS_COMMAND] = compress_command;
    commands[DECOMPRESS_COMMAND] = decompress_command;
    commands[EXIT_COMMAND] = exit_command;
    commands[HELP_COMMAND] = help_command;
}

/*!
   Determines if a command is valid or not
*/
bool valid_command(int command)
{
    return command > 0 && command < NUM_COMMANDS;
}

/*!
    Reads user input and attempts to interpret it as a command
*/
int get_command()
{
    int command;

    while(!(std::cin >> command))
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    }

    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');

    return command;
}

/*!
    Parse command and determine if it is a valid command. If command valid was, execute it.
*/
void parse_command()
{
    while(true)
    {
        list_commands();

        // fix here, if not single char fix char

        int command = get_command();

        if(valid_command(command))
            commands[command]();
        else
            std::cout << INVALID_COMMAND_MESSAGE;
    }
}

/*!
    Main function
*/
int main()
{
    register_commands();
    parse_command();

    return 0;
}