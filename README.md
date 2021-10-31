# Terminal-Based-File-Explorer

* Terminal based file explorer for linux operating system with a restricted feature set in C/C++
using system calls .

* It supports two operating Modes Normal Mode and Command Mode.

## Normal Mode

Normal mode is the default mode of your application. It should have the following functionalities -
*  Display a list of directories and files in the current folder
   * Every file in the directory is displayed on a new line with the following attributes for each file -
      * File Name
      * File Size
      * Ownership (user and group) and Permissions
      * Last modified

    * The file explorer shows entries “.” and “..” for current and parent directory respectively.
    * The file explorer handles scrolling in the case of vertical overflow using keys k & l.
    *  Users can navigate up and down in the file list using the corresponding up and down arrow keys.
    
* Open directories and files
  When Enter Key is pressed - 
    * Directory - Clear the screen and navigate into the directory and show the directory contents.
    * File - Open the file in vi editor
    
* Traversal
    * Go back - Left arrow key should take the user to the previously visited directory.
    * Go forward - Right arrow key should take the user to the next directory.
    * Up one level - Backspace key should take the user up one level.
    * Home - h key should take the user to the home folder (the folder where the application was started).



## Command Mode

The application enters the Command mode whenever “:” (colon) key is pressed. In the command
mode, the user can enter different commands. All commands appear in the status bar at
the bottom.

* Copy - ‘copy <source_file(s)> <destination_directory>’
* Move - ‘move <source_file(s)> <destination_directory>’
* Rename - ‘rename <old_filename> <new_filename>’
* Create File - ‘create_file <file_name> <destination_path>’
* Create Directory - ‘create_dir <dir_name> <destination_path>’
* Delete File - ‘delete_file <file_path>’
* Delete Directory - ‘delete_dir <dir_path>’
* Delete Directory - ‘delete_dir <dir_path>’
* Search - ‘search <file_name>’ or ‘search <directory_name>’
* Search - ‘search <file_name>’ or ‘search <directory_name>’


