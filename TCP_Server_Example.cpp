/**
 * Simple C++ TCP server example.
 *
 * This server listens for incoming connections on port 6000. When a client connects, it prints the
 * client's IP address and port number. It then receives data from the client and prints it to the
 * client's IP address and port number. It then receives data from the client and prints it to the
 * console. When the client disconnects, the server closes the connection and waits for another client
 * to connect.
 * 
 * Set the server computer to:
 * IP address: 192.168.1.168
 * Netmask: 255.255.255.0
 * Gateway: 192.168.1.1
 * 
 * Compile: g++ TCP_Server_Example.cpp -o TCP_Server_Example
 * Execute: ./TCP_Server_Example
 * 
 * Author: Marthinus (Marno) Nel
 * Created Date: 05/05/2023
 * Last Updated: 05/05/2023
*/

/* Libraries:

    <iostream>: Input/output stream library for console input/output.
    <cstring>: Library for string and memory manipulation functions.
    <unistd.h>: Header file providing access to the POSIX operating system API, including functions like close().
    <sys/socket.h>: Header file containing definitions and structures for socket programming.
    <netinet/in.h>: Header file containing Internet address family structures and functions.
    <arpa/inet.h>: Header file containing functions for manipulating IP addresses in a standard format.
    <iomanip>: Input/output manipulator library for formatting output.
    <fstream>: Input/output stream class to operate on files (csv).
    <vector>: This library provides a container class called std::vector that represents a dynamic array.
    <csignal>: This library provides access to the C standard library signal handling.
    <string>: This library provides access to the C++ standard library string handling.
    <chrono>: This library provides access to the C++ standard library time handling.
    <ctime>: This library provides access to the C standard library time handling.
*/
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iomanip>
#include <fstream>
#include <vector>
#include <csignal>
#include <string>
#include <chrono>
#include <ctime>

#define PORT 6000           // Port that the RFID reader sends data through
#define BUFFER_SIZE 1024    // Maximum amount of bytes that can be read in one message from the client

using namespace std;

// Function prototypes
void signalHandler(int signal);
std::string generateNewFilename(const std::string& baseFilename);

// Global variables
std::ofstream csvFile; // csv file to save client data to
std::vector<std::string> clientData;

// Main function
int main() {
    int serverSocket, clientSocket, valRead;
    struct sockaddr_in serverAddress;           // Struct that holds the server's IP address and port number.
    int opt = 1;                                // Used for setting socket options
    int addressLength = sizeof(serverAddress);
    char buffer[BUFFER_SIZE] = {0};             // Array used for receiving data from the client.

    /* Create a TCP socket

    serverSocket: A file descriptor is an abstract representation of an open file or input/output resource in a
    computer operating system. It is a non-negative integer that uniquely identifies the opened file
    within the scope of a process. File descriptors are commonly used for performing input/output
    operations on files, sockets, pipes, and other input/output devices.
    
    AF_INET: The first argument to socket() specifies the address domain or the protocol family to
    be used for the socket. In this case, AF_INET is used, which indicates that the socket will us
    the IPv4 addressing scheme.

    SOCK_STREAM: The second argument specifies the type of socket to be created. SOCK_STREAM
    indicates a TCP socket, which provides a reliable, connection-oriented stream of data.

    0: The third argument specifies the protocol to be used. In this case, 0 indicates that the
    operating system should choose the appropriate protocol based on the provided address domain
    and socket type.

    serverSocket = socket(AF_INET, SOCK_STREAM, 0): The socket() function creates a new socket and
    returns a file descriptor associated with that socket. The file descriptor is assigned to the
    variable serverSocket.

    (serverSocket == 0): The result of the socket() function is checked. If the function call
    returns 0, it indicates that the socket creation failed.

    perror("Socket creation failed"): If the socket creation fails, the perror() function is used
    to print an error message to the standard error stream, indicating the reason for the failure.

    return -1: If the socket creation fails, the program returns -1 to indicate an error condition.
    */
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        return -1;
    }

    /* Set socket options to reuse address and port

    The opt variable is used to set socket options for the server socket. Specifically, opt = 1 sets
    the SO_REUSEADDR and SO_REUSEPORT options.

    The SO_REUSEADDR option allows reusing a local address and port combination. It allows the
    server to bind to an address and port even if it is already in use by another socket that is in
    the TIME_WAIT state. This can be useful to quickly restart a server after it has been shut down.

    The SO_REUSEPORT option allows multiple sockets to bind to the same address and port
    combination. It enables the server to distribute incoming connections among multiple sockets,
    which can help achieve higher concurrency or load balancing.

    By setting opt = 1 and passing it as an argument to setsockopt(), the server socket is
    configured to reuse the address and port, enabling faster startup or allowing multiple sockets
    to bind to the same address and port.

    Note that the opt variable is of type int, and the value 1 is used to indicate that the option
    is enabled. If you set opt = 0, it would disable the option.

    By specifying SOL_SOCKET as the level in the setsockopt() or getsockopt() function, you
    indicate that the option is related to the socket itself rather than a specific protocol.
    */
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("Setsockopt failed");
        return -1;
    }

    /* Description of code below:

    serverAddress.sin_family = AF_INET: The sin_family member of the serverAddress structure is set
    to AF_INET, which indicates that the address family being used is the IPv4 addressing scheme.

    serverAddress.sin_addr.s_addr = INADDR_ANY: The sin_addr.s_addr member of the serverAddress
    structure is set to INADDR_ANY. This constant value represents the IP address of the server.
    By setting it to INADDR_ANY, the server can bind to any available network interface on the
    machine. This allows the server to accept connections from any IP address associated with the
    machine. 
        If you want to use a specific server IP address:
    In this case, you use the inet_addr() function to convert the IP address string "192.168.1.168"
    to the appropriate binary representation required by the sin_addr.s_addr member of the
    serverAddress structure. (serverAddress.sin_addr.s_addr = inet_addr("192.168.1.168");)

    serverAddress.sin_port = htons(PORT): The sin_port member of the serverAddress structure is set
    to the desired port number. In this case, the PORT constant is used. The htons() function is
    used to convert the port number from host byte order to network byte order. This conversion
    ensures that the port number is represented in the correct byte order for network communication.

    In summary, this code sets up the server address structure with the appropriate values to
    specify the addressing scheme (IPv4), IP address (any available interface), and port number.
    These values are later used when binding the socket to the server address.
    */
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    /* Bind the socket to the IP address and port
    The bind() function is used to associate a socket with a specific IP address and port number.
    It takes three arguments:

    The file descriptor of the socket to be bound, in this case, serverSocket.

    A pointer to a sockaddr structure that specifies the IP address and port to bind to. In this
    code, the serverAddress structure is passed as the argument, but it needs to be cast to a
    pointer of type struct sockaddr *.

    The size of the serverAddress structure, provided by sizeof(serverAddress).

    If the bind() function fails, it returns a value less than 0, indicating an error. In that case,
    the perror() function is called to print an error message to the standard error stream,
    indicating that the binding has failed. The "Binding failed" message is printed to stderr,
    followed by a description of the error based on the value of the errno variable.
    */
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Binding failed");
        return -1;
    }

    /* Listen for incoming connections

    The listen() function is used to put the server socket in a passive listening state, allowing
    it to accept incoming connection requests. It takes two arguments:

    The file descriptor of the server socket, serverSocket.
    The maximum number of pending connections that can be queued up before they are accepted by the
    server. In this code, a backlog value of 3 is used.

    If the listen() function fails, it returns a value less than 0, indicating an error. In that
    case, the perror() function is called to print an error message to the standard error stream,
    indicating that the listening has failed. The "Listen failed" message is printed to stderr,
    followed by a description of the error based on the value of the errno variable.
    */
    if (listen(serverSocket, 3) < 0) {
        perror("Listen failed");
        return -1;
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    /* Accept a client connection to a server

    This code snippet is responsible for accepting a client connection and setting up a new socket
    (clientSocket) to communicate with the connected client.

    accept() function: It is used to accept an incoming connection on a listening socket
    (serverSocket in this case). It waits until a client connects and returns a new socket
    descriptor (clientSocket) for the established connection.

    (struct sockaddr *)&serverAddress: It is a pointer to a structure that holds the client's
    address information. In this case, it's a cast of the serverAddress structure to a generic
    struct sockaddr* pointer.

    (socklen_t *)&addressLength: It is a pointer to a variable (addressLength) that holds the size
    of the serverAddress structure. It is used to provide the size of the client's address structure
    to the accept() function.

    Assignment and comparison: The result of the accept() function is assigned to clientSocket, and
    then it is compared with less than 0 (< 0) to check if the function call was successful. If the
    value is less than 0, it indicates an error in accepting the connection.

    Error handling: If accept() returns a value less than 0, perror() is used to print an error
    message to the console. It displays a descriptive error message based on the current value of
    errno. After that, the function returns -1 to indicate an error.
    */
    if ((clientSocket = accept(serverSocket, (struct sockaddr *)&serverAddress, (socklen_t *)&addressLength)) < 0) {
        perror("Accept failed");
        return -1;
    }

    std::cout << "Accepted connection from " << inet_ntoa(serverAddress.sin_addr) << ":" << ntohs(serverAddress.sin_port) << std::endl;

    // Register signal handler for Ctrl+C (SIGINT)
    std::signal(SIGINT, signalHandler);

    // // CSV file for logging client data
    // std::string baseFilename = "client_data_log";
    // std::string filename = generateNewFilename(baseFilename);
    // csvFile.open(filename, std::ios::out | std::ios::app);

    // CSV file for logging client data
    std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()); // Get current date and time
    // Convert current time to string with desired format
    std::string folderName = "data_logs";
    std::stringstream ssTime;
    ssTime << folderName << "/" << "client_data_log_" << std::put_time(std::localtime(&currentTime), "%Y-%m-%d");
    std::string baseFilename = ssTime.str();
    // std::string baseFilename = "client_data_log";

    std::string filename = generateNewFilename(baseFilename);
    csvFile.open(filename, std::ios::out | std::ios::app);



    /* Receive data from the client and print it in hexadecimal

    read() function: It reads data from the clientSocket into the buffer array. The BUFFER_SIZE
    constant determines the maximum number of bytes to read in a single iteration.

    valRead variable: It stores the number of bytes read from the client. The result of the read()
    function call is assigned to valRead.

    while loop: It iterates as long as there is data to read from the client. The loop condition
    checks if valRead is greater than 0, indicating that data was successfully read.

    Printing data in hexadecimal format: Inside the loop, each byte of the received data is printed
    in hexadecimal format using std::cout. The std::hex manipulator is used to set the output stream
    to hexadecimal mode. std::setw(2) and std::setfill('0') ensure that each byte is printed as a
    two-digit value with leading zeros if necessary.

    static_cast and unsigned char: The buffer[i] value is cast to unsigned char to ensure it is
    treated as an unsigned integer when converting it to hexadecimal format. This is done to avoid
    sign-extension issues for negative byte values.

    Resetting the buffer: After printing the data, the buffer is reset to zero using memset() to
    clear its contents and prepare it for the next iteration of the loop.

    This code snippet allows you to receive data from the client and display it in hexadecimal
    format, byte by byte.

    The client data is saved in a unsigned char vector (clientData) to easily manipulate it.

    The code also logs the client data to a csv file.
    */
    while ((valRead = read(clientSocket, buffer, BUFFER_SIZE)) > 0) {
        // std::cout << "Received data (hex): ";
        for (int i = 0; i < valRead; i++) {
            unsigned char value = static_cast<unsigned char>(buffer[i]);
            // std::cout << std::hex << std::setw(2) << std::setfill('0') <<
            // static_cast<unsigned int>(value) << " ";

            // Save data to CSV file as hex values
            std::stringstream ss;
            ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(value);
            csvFile << ss.str() << ",";

            // Store client data in clientData vector as (hex) strings
            std::stringstream valueSS;
            valueSS << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(value);
            std::string formattedValue = valueSS.str();
            clientData.push_back(formattedValue);
        }
        std::cout << std::dec << std::endl;
        memset(buffer, 0, sizeof(buffer)); // Clear buffer

        // Add a new line to the CSV file after each message
        csvFile << "\n";

        // Print out the clientData
        std::cout << "Client data (hex): ";
        for (int i = 0; i < clientData.size(); i++) {
             std::cout << clientData[i] << " ";
        }
        std::cout << std::endl; // New line after clientData has been printed

        // Extract out the RFID Data frames
        std::cout << "Head (hex): " << clientData[0] << " " << "\n";

        int TYPE;
        std::stringstream cs(clientData[1]);
        cs >> std::hex >> TYPE;                 // Change string to HEX
        std::cout << "Type (hex): "  << std::hex << TYPE << " " << "\n";

        std::cout << "Len (hex): " << clientData[2] << " " << "\n";
        // Value used to extract data - 16 to convert from HEX to Int
        int Len_int = std::stoi(clientData[2], nullptr, 16);

        std::vector<std::string> Data(clientData.begin() + 3, clientData.begin() + 3 + Len_int); 
        std::cout << "Data (hex): ";
        for (int i = 0; i < Data.size(); i++) {
             std::cout << Data[i] << " ";
        }
        std::cout << "\n";

        std::string CRC = clientData[Len_int + 3];
        std::cout << "CRC (hex): " << CRC << "\n";
        // Calculate the Checksum to ensure correct data was received
        int crc_sum = 0;
        // Sum the hexadecimal values
        for (int i = 1; i < Len_int + 3; i++) {
            crc_sum += std::stoi(clientData[i], nullptr, 16);
        }
        int checksum_int = crc_sum & 0xFF; // Get the last two bytes of the sum
        // Change to HEX value
        std::stringstream crcSS;
        crcSS << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(checksum_int);
        std::string checksum = crcSS.str();
        // Print the summed value in hexadecimal format
        std::cout << "Summed value (hex): " << checksum << "\n";
        // Compare the calculated checksum with the received checksum
        if (checksum == CRC) {
            std::cout << "\033[32mValid data. (Checksums match)\033[0m";
        } else {
            std::cout << "\033[31mInvalid data. (Checksums do not match)\033[0m";
        }

        std::cout << std::endl; // New line after clientData has been printed


        // Switch case to determine how to handle the RFID message based on its TYPE
        switch (TYPE) {
            case 0x3a:
                // Code to be executed if choice is 1
                std::cout << "\033[1;35mTCP connection with RFID reader successful\033[0m" << std::endl;
                break;
            case 0x17:
                // Code to be executed if choice is 2
                std::cout << "\033[1;36mTAG Read\033[0m" << std::endl;
                   // Extract a subset of elements from index 1 to 3 (exclusive)
                break;
            case 0x40:
                // Code to be executed if choice is 3
                std::cout << "\033[1;33mHeartbeat\033[0m" << std::endl;
                break;
            default:
                // Code to be executed if choice doesn't match any case
                std::cout << "\033[1;31mThe RFID type is not recognized\033[0m" << std::endl;
                break;
        }

        // If you are done with the clientData then clear the vector to ensure it only contains one
        // client message at a time
        clientData.clear();
    }

    /* Check if the client disconnected or an error occurred

    This code snippet is responsible for handling the scenarios after reading data from the client.
    Let's understand each part of the code:

    Checking if valRead is 0: If the value of valRead is 0, it indicates that the client has
    disconnected. This condition checks if no data was read from the client, suggesting that the
    connection has been closed.

    Printing "Client disconnected": When the client is disconnected, the code prints the message
    "Client disconnected" to the console using std::cout.

    Checking if valRead is -1: If the value of valRead is -1, it indicates that an error occurred
    during the read operation.

    Printing "Read failed": When the read operation fails, the code uses the perror() function to
    print an error message to the console. The error message is provided as an argument to perror(),
    and it is typically a description of the error encountered during the read operation.

    By checking the values of valRead and handling the different scenarios, this code snippet allows
    you to respond appropriately when the client disconnects or when a read operation fails.
    */
    if (valRead == 0) {
        std::cout << "Client disconnected" << std::endl;
    } else if (valRead == -1) {
        perror("Read failed");
    }

    // Close the client socket
    close(clientSocket);

    // Close the server socket
    close(serverSocket);

    // Close the csv file
    csvFile.close();

    return 0;
}

// Signal handler for Ctrl+C (SIGINT)
void signalHandler(int signal) {
    if (csvFile.is_open()) {
        csvFile.close();
    }
    std::cout << "Program terminated by user." << std::endl;
    exit(signal);
}

// // Generate a new filename if the base filename already exists
// std::string generateNewFilename(const std::string& baseFilename) {
//     std::string newFilename = baseFilename;
//     int counter = 1;
//     newFilename = baseFilename + "_" + std::to_string(counter) + ".csv";

//     while (std::ifstream(newFilename)) {
//         counter++;
//         newFilename = baseFilename + "_" + std::to_string(counter) + ".csv";
//     }

//     return newFilename;
// }


// // Generate a new filename if the base filename already exists
// std::string generateNewFilename(const std::string& baseFilename) {
//     // Generate a unique identifier (e.g., incrementing number)
//     int identifier = 1;
//     std::string filename;
//     do {
//         std::stringstream filenameSS;
//         filenameSS << baseFilename << "_" << identifier << + ".csv";
//         filename = filenameSS.str();
//         identifier++;
//     } while (std::ifstream(filename));

//     return filename;
// }

// Generate a new filename if the base filename already exists
std::string generateNewFilename(const std::string& baseFilename) {
    // Generate a unique identifier (e.g., incrementing number)
    int identifier = 1;
    std::string filename;
    do {
        std::stringstream filenameSS;
        filenameSS << baseFilename << "_" << identifier << ".csv";
        filename = filenameSS.str();
        identifier++;
    } while (std::ifstream(filename.c_str()));

    return filename;
}