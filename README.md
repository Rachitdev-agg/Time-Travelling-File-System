# **Time-Travelling File System**

This is my implementation of a simplified, in-memory version control system for my COL106 project. It's inspired by Git and manages files with different versions. I built all the main data structures from scratch, including a Tree for the version history, a Hash Map to store the files, and Heaps to handle the system analytics.

## **Prerequisites**

To compile and run this project, you will need:

* A C++ compiler (like g++)  
* A standard shell environment (like bash)

## **How to Compile and Run**

1. **Compile**: To compile my code, you'll need g++. Navigate to the folder with Assignment\_1.cpp and run this in the terminal:  
   g++ Assignment\_1.cpp \-o Assignment\_1

2. **Run**: To start the program, just run the executable file:  
   ./Assignment\_1

3. **Using the Program**: Once it's running, you'll see a \> prompt. The program is now waiting for you to input commands.

## **Key Terms**

* **File**: A logical object in the system that holds content and its own version history.  
* **Snapshot**: A command that marks a file's current version as immutable. Any subsequent INSERT or UPDATE will create a new version.  
* **Version ID**: A unique integer ID for each version of a file, starting from 0\.  
* **Recent Heap**: A max-heap that keeps files ordered by their modified\_time for the RECENT\_FILES command.  
* **Biggest Heap**: A max-heap that keeps files ordered by their total\_versions for the BIGGEST\_TREES command.

## **Command Reference**

My program accepts the following commands. The commands must follow the syntax shown below.

| Command | Syntax | Description |
| :---- | :---- | :---- |
| CREATE | CREATE \<filename\> | Creates a new file with an initial empty version (snapshot). |
| READ | READ \<filename\> | Displays the content of the file's currently active version. |
| INSERT | INSERT \<filename\> \<content\> | Appends content to the active version. Creates a new version if the active one is a snapshot. |
| UPDATE | UPDATE \<filename\> \<content\> | Replaces the content of the active version. Creates a new version if the active one is a snapshot. |
| SNAPSHOT | SNAPSHOT \<filename\> \<message\> | Marks the active version as a snapshot with a given message. |
| ROLLBACK | ROLLBACK \<filename\> \[versionID\] | Sets the active version. Rolls back to the parent if versionID is omitted. |
| HISTORY | HISTORY \<filename\> | Lists the history of all snapshotted versions from the active version back to the root. |
| RECENT\_FILES | RECENT\_FILES \<num\> | Lists the num most recently modified files. |
| BIGGEST\_TREES | BIGGEST\_TREES \<num\> | Lists the num files with the most versions. |

## **Error Handling and Command Conditions**

As per the assignment, I have implemented robust error handling. For a command to execute successfully, the following conditions must be met. If any condition fails, the program will print a specific error message.

* **General**:  
  * The command must be one of the known commands (e.g., CREATE, READ).  
  * The command line cannot be empty.  
* **CREATE \<filename\>**:  
  * Must have exactly one argument: the \<filename\>.  
  * A file with the same \<filename\> must not already exist.  
* **READ \<filename\> / HISTORY \<filename\>**:  
  * Must have exactly one argument: the \<filename\>.  
  * The specified \<filename\> must exist in the file system.  
* **INSERT/UPDATE \<filename\> \<content\>**:  
  * Must have at least two arguments: \<filename\> and \<content\>.  
  * The specified \<filename\> must exist.  
* **SNAPSHOT \<filename\> \<message\>**:  
  * Must have at least two arguments: \<filename\> and \<message\>.  
  * The specified \<filename\> must exist.  
* **ROLLBACK \<filename\> \[versionID\]**:  
  * Must have one or two arguments.  
  * The specified \<filename\> must exist.  
  * If rolling back to parent, the active version cannot be the root.  
  * If a versionID is provided, it must be a valid, non-negative integer that corresponds to an existing version.  
* **RECENT\_FILES/BIGGEST\_TREES \<num\>**:  
  * Must have exactly one argument: \<num\>.  
  * \<num\> must be a valid, positive integer.  
  * \<num\> cannot be greater than the total number of files in the system.

## **Additional Details & Design Choices**

To make the heaps for the analytics commands more efficient, I chose to maintain them in real-time. This led to some specific design choices.

* **Global Variables**:  
  * f\[199\]: This is the array of LinkedLists that acts as my main hash map. I made it global so the getFile function can access it directly.  
  * file\_heap1 & file\_heap2: These are the vectors that store the heaps for RECENT\_FILES and BIGGEST\_TREES. I made them global so that the file class methods could directly modify them.  
* **Key Member Variables in file Class**:  
  * pos\_hash1, pos\_hash2: I added these to each file object to store the file's current index within the heaps. This was necessary to achieve efficient O(log N) updates to the heaps whenever a file is modified.

## **Assumptions**

* **No Spaces in Filenames**: Based on a TA clarification, I have assumed that filenames will not contain any spaces.  
* **Case-Sensitivity**: I decided to make all commands and filenames case-sensitive.  
* **Empty Snapshot Messages**: My implementation allows for creating a snapshot with an empty message.  
* **Input Format**: I've assumed that each command will be on its own line.

## **Example Usage**

\> CREATE notes  
\> INSERT notes Hello world  
\> READ notes  
Hello world  
\> SNAPSHOT notes First version  
\> INSERT notes this is a test  
\> HISTORY notes  
ID:0 timestamp: 1726071000 message: Default Message  
ID:1 timestamp: 1726071005 message: First version  
\> RECENT\_FILES 1  
notes   
\> EXIT

## **Exiting the Program**

To exit my program, just type EXIT and press Enter.

EXIT  
