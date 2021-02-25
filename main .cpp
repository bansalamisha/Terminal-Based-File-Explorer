#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <dirent.h>
#include <string>
#include <unistd.h>
#include <cstring>
#include <bits/stdc++.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <fcntl.h>
#define esc 27
#define cls printf("\033[H\033[J")

using namespace std;
void dfs(string);
void create_dir(vector<string> &);
void create_file(vector<string> &);
void renamedf(string, string);
void openDirCmd(string);
bool searchFileOrDir(string);
string parsePath(string);
void clearcmd();
void page_down();
void page_up();
void move_down();
void move_up();
void open(string);
void opendirec();
void display(string);
void initSettings();
void pos(int, int);
void pos();
int isDirectory(string);
int is_regular_file(string);
void normalMode();
void resetSettings();
void commandMode();
void process(string);
struct termios initials, news;
unsigned int x = 1, y = 1, page = 0;
long long int top = 0;
unsigned int printSize;
long long int fileCount = 0;
unsigned int rowSizeeff, rowSize, colSize;
vector<string> dirList;
vector<string> bwd;
vector<string> fwd;
int flag = 0;
string root;
string currp;
int is_regular_file(string path)
{
    char *pathc = new char[path.length() + 1];
    strcpy(pathc, path.c_str());
    struct stat path_stat;
    stat(pathc, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

int isDirectory(string path)
{
    char *pathc = new char[path.length() + 1];
    strcpy(pathc, path.c_str());
    struct stat statbuf;
    if (stat(pathc, &statbuf) != 0)
        return 0;
    return S_ISDIR(statbuf.st_mode);
}
void pos()
{
    printf("%c[%d;%dH", esc, x, y);
}

void pos(int xv, int yv)
{
    printf("%c[%d;%dH", esc, xv, yv);
}

void initSettings()
{

    tcgetattr(fileno(stdin), &initials);
    news = initials;
    news.c_lflag &= ~ICANON;
    news.c_lflag &= ~ECHO;
    tcsetattr(fileno(stdin), TCSAFLUSH, &news);
}
bool comp(struct dirent *s1, struct dirent *s2)
{

    return s1->d_name < s2->d_name;
}
void display(string curr)

{

    const char *currc = curr.c_str();
    string path(currp + string("/") + currc);

    char *pathc = new char[path.length() + 1];
    strcpy(pathc, path.c_str());

    struct stat d;
    stat(pathc, &d);

    struct passwd *pw = getpwuid(d.st_uid);
    struct group *gr = getgrgid(d.st_gid);

    printf((S_ISDIR(d.st_mode)) ? "d" : "-");
    printf((d.st_mode & S_IRUSR) ? "r" : "-");
    printf((d.st_mode & S_IWUSR) ? "w" : "-");
    printf((d.st_mode & S_IXUSR) ? "x" : "-");
    printf((d.st_mode & S_IRGRP) ? "r" : "-");
    printf((d.st_mode & S_IWGRP) ? "w" : "-");
    printf((d.st_mode & S_IXGRP) ? "x" : "-");
    printf((d.st_mode & S_IROTH) ? "r" : "-");
    printf((d.st_mode & S_IWOTH) ? "w" : "-");
    printf((d.st_mode & S_IXOTH) ? "x" : "-");

    if (pw != 0)
        printf("\t%-8s", pw->pw_name);
    if (gr != 0)
        printf(" %-8s", gr->gr_name);

    printf("%10.2fK", ((double)d.st_size) / 1024);

    char *mt = (ctime(&d.st_mtime));
    mt[strlen(mt) - 1] = '\0';

    printf("%30s", mt);
    if (S_ISDIR(d.st_mode))
    {
        printf("\033[1;12m");
        printf("\t%-20s\n", currc);
        printf("\033[0m");
    }
    else
        printf("\t%-20s\n", currc);
}

void opendirec()
{
    printf("\033[H\033[J");
    printf("%c[%d;%dH", 27, 1, 1);
    char csource[currp.length()];
    strcpy(csource, currp.c_str());
    DIR *mydir;
    if ((mydir = opendir(csource)) == NULL)
        clog << "Error";

    x = 1;
    y = 80;
    page = 0;

    top = 0;
    struct dirent *curr = NULL;
    long long int fileCount = 0;
    dirList.clear();
    while ((curr = readdir(mydir)) != NULL)
    {

        cls;
        dirList.push_back(curr->d_name);
        fileCount++;
    }
    sort(dirList.begin(), dirList.end());

    struct winsize wind;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &wind);
    rowSize = wind.ws_row;
    rowSizeeff = wind.ws_row - 4;
    colSize = wind.ws_col;
    if (fileCount < rowSizeeff)
    {
        printSize = fileCount;
    }
    else
    {
        printSize = rowSizeeff;
    }

    for (unsigned int i = 0; i < printSize; i++)
    {
        display(dirList[i]);
    }
    pos();
    closedir(mydir);
}

void open(string fname)
{
    string fpath = currp + string("/") + fname;
    char *fpathc = new char[fpath.length() + 1];
    strcpy(fpathc, fpath.c_str());
    if (isDirectory(fpathc))
    {
        currp = fpath;
        opendirec();
    }

    else if (is_regular_file(fpathc))
    {

        pid_t pid = fork();

        if (pid == 0)
        {
            execl("/usr/bin/vi", "vi", fpathc, NULL);
            exit(1);
        }
        wait(NULL);
        pos(x, y);
    }

    else
    {
        //error
    }
}
void move_up()
{
    if (x == 1)
    {
        if (page == 0)
        {
        }
        else
        {
            top--;
            page--;
            cls;
            for (int i = page; i < (page + printSize); i++)
            {
                display(dirList[i]);
            }
            pos();
        }
    }
    else
    {

        x--;

        pos();
    }
}

void move_down()
{
    if (x == (printSize))
    {
        if (dirList.size() > (printSize + page))
        {

            top++;
            page++;
            cls;
            for (int i = top; i < (top + printSize); i++)
            {
                display(dirList[i]);
            }

            pos();
        }
    }
    else
    {

        x++;
        pos();
    }
}

void page_up()
{
    if (page == 0)
    {
    }
    else
    {
        if ((top - printSize) >= 0)
        {
            cls;
            pos(1, 1);
            top -= printSize;
            page -= printSize;
            cls;
            for (int i = top; i < (top + printSize); i++)
            {
                display(dirList[i]);
            }
            pos(1, 80);
        }
        else
        {
            top = 0;
            page = 0;
            cls;
            pos(1, 1);
            for (int i = 0; i < printSize; i++)
            {
                display(dirList[i]);
            }
            pos(1, 80);
        }
    }
}
void page_down()
{
    unsigned int temp1, temp2;
    temp1 = x;
    temp2 = y;
    if ((printSize + page) > dirList.size() or ((top + printSize) >= dirList.size()))
    {
    }

    else if ((page + 2 * printSize) <= dirList.size())
    {

        cls;
        pos(1, 1);
        page += printSize;
        top += printSize;

        for (int i = top; i < (top + printSize); i++)
        {
            display(dirList[i]);
        }
        pos(1, 80);
    }
    else if (((page + 2 * printSize) > dirList.size()))
    {

        cls;
        pos(1, 1);

        page = dirList.size();
        top += printSize;
        for (int i = top; i < dirList.size(); i++)
        {
            display(dirList[i]);
        }
        pos(1, 80);
    }
    else
    {
        cls;
        pos(1, 1);
        top += printSize;

        for (int i = printSize + page; i < dirList.size(); i++)
        {
            display(dirList[i]);
        }
        pos(1, 80);
    }
}
void clearcmd()
{
    int last = rowSize + 1;
    printf("%c[%d;%dH", 27, last, 1);
    printf("%c[2K", 27);
    cout << ":";
    pos(rowSize, 1);
    cout << "COMMAND MODE:";
    pos(rowSize, 14);
}

string parsePath(string ip)
{
    string res = "";
    //absolute
    if (ip[0] == '/')
    {
        res = root + ip;
    }
    else if (ip[0] == '~')
    {
        res = root + ip.substr(1, ip.length());
    }

    else if (ip[0] == '.')
    {

        if (ip.length() == 1)
        {
            res = currp;
        }

        else
        {
            res = currp + ip.substr(1, ip.length());
        }
    }
    else if ((ip[0] >= 65 and ip[0] <= 90) or (ip[0] >= 97 and ip[0] <= 122))
    {
        res = currp + string("/") + ip;
    }

    return res;
}

bool searchFileOrDir(string sf)
{
    queue<string> q;
    q.push(currp);
    while (!q.empty())
    {
        string curr = q.front();
        q.pop();
        struct dirent *d;
        struct stat sb;
        DIR *dr;
        char currPath[curr.length()];
        strcpy(currPath, curr.c_str());
        dr = opendir(currPath);
        if (dr != NULL)
        {
            while ((d = readdir(dr)) != NULL)
            {
                string dpath = currPath + string("/") + d->d_name;
                if (is_regular_file(dpath))
                {
                    if (d->d_name == sf)
                    {
                        return true;
                    }
                }
                else if (isDirectory(dpath))
                {
                    if (d->d_name == sf)
                    {
                        return true;
                    }

                    else
                    {
                        string dir_name = d->d_name;
                        if (dir_name != "." && dir_name != "..")
                        {
                            q.push(dpath);
                        }
                    }
                }
            }
        }
        closedir(dr);
    }
    return false;
}
void openDirCmd(string fpath)
{

    char *fpathc = new char[fpath.length() + 1];
    strcpy(fpathc, fpath.c_str());
    if (isDirectory(fpathc))
    {
        currp = fpath;
        opendirec();
    }
    else
    {
        //error
    }
}

void renamedf(string name1, string name2)
{
    string string1 = parsePath(name1);
    string string2 = parsePath(name2);
    rename(string1.c_str(), string2.c_str());
}
void create_file(vector<string> &tokens)
{
    //cout<<"create_file inside"<<endl;
    if (tokens.size() < 3)
    {
        //printf("Too few arguments:\n");
    }
    else
    {
        string destDir = parsePath(tokens.back());
        destDir = destDir + string("/") + tokens[1];
        char path[destDir.length()];
        strcpy(path, destDir.c_str());
        creat(path, 0777);
    }
    return;
}
void create_dir(vector<string> &tokens)
{
    if (tokens.size() < 3)
    {
        //printf("Too Few Arguments\n");
    }
    else
    {
        string destDir = parsePath(tokens.back());
       // clog<<"Before parsing destdir : "<<destDir<<endl;
        if (!isDirectory(destDir))
        {
            cout << "Destination is not directory." << endl;
            return;
        }
        destDir = destDir + string("/") + tokens[1];
       // clog<<"creating dir : "<<destDir<<endl;
        if (mkdir(destDir.c_str(), 0755) != 0)
        {
            //cout<<"Couldn't create directory!";
        }
        else
        {
            //cout<< "Directory created successfully." << endl;
        }
    }
    return;
}



void delete_file(string fpath) {
    
    char path[1024];
	//fpath = parsePath(fpath);
    //cout<<"deleting file"<<fpath<<endl;
    strcpy(path, fpath.c_str());
    int status = remove(path);
    //clearcmd();
}

void removeDirUtil(string dirPath){
   // clog<<"Inside removeDirUtil"<<endl;
	dirPath = parsePath(dirPath);
    //clog<<"Root path "<<dirPath<<endl;
	dfs(dirPath);
    //clog<<"Exiting removeDirUtil"<<endl;

}
void removeDir(string dpath){
    //clog<<"Entering removeDir"<<endl;
    //dpath = parsePath(dpath);
	char path[1024];
    strcpy(path, dpath.c_str());
   // clog<<"Deleting "<<dpath<<endl;
    int status = rmdir(path);
  //clog<<"Exiting removeDir"<<endl;
}


void dfs(string root) {
  struct dirent *dt;
  struct stat buf;
  DIR *dir;
  char path[1024];
  strcpy(path, root.c_str());
  dir = opendir(path);
  if (dir == NULL){
    if(errno == ENOTDIR){ //if dir is NULL and errno is set to ENOTDIR then it is a file else invalid dir
    	//delete file
     delete_file(root);
    // cout<<"Deleting "<<root<<endl;
    }
    return;
  }
  while ((dt = readdir(dir)) != NULL) {
    string dname = dt->d_name;
    bool iscurr = false, isprev = false;
    if (dname == ".") iscurr = true;
    if (dname == "..") isprev = true;
    if (!iscurr && !isprev) {
      dname = root + "/" + dname;
      dfs(dname);          
    }
  }
  //Delete Dir
    //cout<<"Deleting "<<root<<endl;
  removeDir(root);
}
void process(string ipString)
{   
    clearcmd();
    string temp = "";
    vector<string> tokens;

    for (int i = 0; i < ipString.length(); i++)
    {

        if (ipString[i] == ' ' or i == (ipString.length() - 1))
        {
            if (i == (ipString.length() - 1))
            {
                temp = temp + ipString[i];
            }
            tokens.push_back(temp);
            temp = "";
        }
        else
        {
            temp = temp + ipString[i];
        }
    }

    if (tokens[0].compare("rename") == 0)
    {
        renamedf(tokens[1], tokens[2]);
    }
    else if ((tokens[0].compare("create_file") == 0))
    {
        create_file(tokens);
    }
    else if (tokens[0].compare("create_dir") == 0)
    {   
        //clog<<"Calling create dir\n";
        create_dir(tokens);
    }
    else if ((tokens[0].compare("delete_file") == 0))
    {
        delete_file(parsePath(tokens[1]));
    }
    else if ((tokens[0].compare("delete_dir") == 0))
    {
        removeDirUtil(tokens[1]);
    }

    else if (tokens[0].compare("goto") == 0)
    {
        if (tokens.size() == 2)
        {
            string path = parsePath(tokens[1]);
            DIR *d;
            char *temp = new char[path.length() + 1];
            strcpy(temp, path.c_str());

            bwd.push_back(currp);
            currp = path;
            fwd.clear();
        }
        else
        {
        }
        //error
    }
    else if (tokens[0].compare("search") == 0)
    {
        if (searchFileOrDir(tokens[1]))
            cout << "Required file/directory is present";
        else
        {
            cout << "Required file/directory is not present";
        }
    }
}

void commandMode()
{
    string temp = currp;
    pos(rowSize, 1);
    cout << "COMMAND MODE:";
    pos(rowSize, 14);
    string ipString;
    char ch;

    do
    {
        ch = cin.get();

        if (ch == 10)
        {
            clearcmd();
            process(ipString);
            ipString = "";
        }
        else if (ch == 127)
        {

            clearcmd();
            if (ipString.length() == 1)
            {
                ipString = "";
            }
            else
            {
                ipString = ipString.substr(0, ipString.length() - 1);
            }

            cout << ipString;
        }

        else if (ch != 27)
        {
            ipString += ch;
            cout << ch;
        }
        else if (ch == 27) //change
        {
            break;
        }

    } while (ch != 27);
    currp = root;
    cls;
    openDirCmd(currp);

    /*bwd.push_back(currp);
currp=root;
pos(1,80);*/
    //normalMode();
    return;
}

void normalMode()
{

    x = 1, y = 80;
    pos();
    char ch;
    while (1)
    {
        ch = cin.get();
        if (ch == 27)
        {
            ch = cin.get();
            ch = cin.get();
            if (ch == 'A')
            {
                move_up();
            }

            else if (ch == 'B')
            {
                move_down();
            }
            //forward
            else if (ch == 'D')
            {
                if (!bwd.empty())
                {
                    fwd.push_back(currp);
                    currp = bwd.back();
                    opendirec();
                    bwd.pop_back();
                }
            }

            else if (ch == 'C')
            {
                if (!fwd.empty())
                {
                    bwd.push_back(currp);
                    currp = fwd.back();
                    opendirec();
                    fwd.pop_back();
                }
            }

            else
            {
            }
        }

        else if (ch == 'H' || ch == 'h')
        {
            fwd.clear();
            bwd.push_back(currp);
            currp = root;
            opendirec();
        }
        //backspace
        else if (ch == 127)
        {
            fwd.clear();
            bwd.push_back(currp);
            int end = currp.length() - 1;

            for (int i = end; i > 0; i--)
            {

                if (currp[i] == '/')
                {
                    string temp = currp.substr(0, i);
                    currp = temp;
                    break;
                }
            }

            opendirec();
        }

        else if (ch == 10)
        {
            fwd.clear();
            if (x == 1 and page == 0)
            {
            }

            else if (page == 0 and x == 2)
            {
                if (currp.compare(root) == 0)
                {
                    //cout<<"hola"<<endl;
                }
                else
                {
                    bwd.push_back(currp);
                    int end = currp.length() - 1;

                    for (int i = end; i >= 0; i--)
                    {

                        if (currp[i] == '/')
                        {
                            string temp = currp.substr(0, i);
                            currp = temp;
                            break;
                        }
                    }

                    opendirec();
                }
            }

            else
            {
                bwd.push_back(currp);
                open(dirList[top + x - 1]);
            }
        }
        else if (ch == 'k')
        {
            page_up();
        }
        else if (ch == 'l')
        {
            page_down();
        }

        else if (ch == ':')
        {
            commandMode();

            normalMode();
        }else if(ch == 'q'){
            break;
        }
    }
}

void resetSettings()
{
    tcsetattr(fileno(stdin), TCSAFLUSH, &initials);
}

int main()
{

    char rootc[1024];
    getcwd(rootc, 1024);
    root = string(rootc);
    currp = string(root);
    opendirec();
    initSettings();
    normalMode();
    resetSettings();
    write(1,"\033[H\033[J",6);
}