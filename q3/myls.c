#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdbool.h>
#include <sys/sysmacros.h>
#include <dirent.h>
#include <limits.h>


//Compare time to determine if they are recent.
//Time is recent if less than 6 months apart
bool IsTimeRecent(time_t t)
{
    time_t curTime = time(0);
    double timeDiff = difftime(t, curTime);

    if(timeDiff > 15778800 || timeDiff < -15778800)
    {
        return false;
    }
    else
    {
        return true;
    }
}

//Format Time
//If recent MMM DD HH:mm
//Not recent MMM DD YYYY
void FormatTime(time_t time, bool isRecent, char* timeBuf)
{
    //char* timeBuf;
    struct tm *timeinfo;

    timeinfo = localtime(&time);

    if(IsTimeRecent(time))
    {
        strftime(timeBuf, 13, "%b %d %H:%M", timeinfo);
        return;
    }
    else
    {
        strftime(timeBuf, 12, "%b %d %Y", timeinfo);
        return;
    }
}

// user name of the user owner (hints: Stevens & Rago, 6.2.);
void GetOwnerName(struct stat *buf)
{
    struct passwd *pwd;
    
    pwd = getpwuid(buf->st_uid);
    printf("Username of user owner: %s\n", pwd->pw_name);
    return;
}
// group name of the group owner; (hints: Stevens & Rago, 6.4.);
void GetGroupName(struct stat *buf)
{
    struct group * grp;

    grp = getgrgid(buf->st_gid);
    printf("Group name of group owner: %s\n", grp->gr_name);
    return;
}
// the type of file;
void GetFileType(struct stat *buf)
{
    if(S_ISREG(buf->st_mode))
    {
        printf("File type: Regular file");
    }
    else if(S_ISDIR(buf->st_mode))
    {
        printf("File type: Directory");
    }
    else if(S_ISCHR(buf->st_mode))
    {
        printf("File type: Character special");
    }
    else if(S_ISBLK(buf->st_mode))
    {
        printf("File type: Block special");
    }
    else if(S_ISFIFO(buf->st_mode))
    {
        printf("File type: FIFO");
    }
    else if(S_ISLNK(buf->st_mode))
    {
        printf("File type: Symbolic link");
    }
    else if(S_ISSOCK(buf->st_mode))
    {
        printf("File type: Socket");
    }
    else
    {
        printf("File type: **unknown file type");
    }

    printf("\n");
    return;
}
// full access permissions, reported in the format used by the ls program;
void GetPermission(struct stat *buf)
{
    printf("File permission: ");
        //User read write execute permission
    if(buf->st_mode & S_IRUSR)
    {
        printf("r");
    }
    else
    {
        printf("-");
    }
    if(buf->st_mode & S_IWUSR)
    {
        printf("w");
    }
    else
    {
        printf("-");
    }
    if(buf->st_mode & S_IXUSR)
    {
        printf("x");
    }
    else
    {
        printf("-");
    }

        //Group read write execute permission
    if(buf->st_mode & S_IRGRP)
    {
        printf("r");
    }
    else
    {
        printf("-");
    }
    if(buf->st_mode & S_IWGRP)
    {
        printf("w");
    }
    else
    {
        printf("-");
    }
    if(buf->st_mode & S_IXGRP)
    {
        printf("x");
    }
    else
    {
        printf("-");
    }

        //Others read write execute permission
    if(buf->st_mode & S_IROTH)
    {
        printf("r");
    }
    else
    {
        printf("-");
    }
    if(buf->st_mode & S_IWOTH)
    {
        printf("w");
    }
    else
    {
        printf("-");
    }
    if(buf->st_mode & S_IXOTH)
    {
        printf("x");
    }
    else
    {
        printf("-");
    }

    printf("\n");
    return;
}

// the size of the file;
void GetFileSize(struct stat *buf)
{
    printf("File size: %ld\n", buf->st_size);
    return;
}
// i-node number;
void GetFileINode(struct stat *buf)
{
    printf("File I-node number: %ld\n", buf->st_ino);
    return;
}
// the device number of the device in which the file is stored, including both major number and minor number (hints: Stevens & Rago, 4.23.);
void GetDeviceNumber(struct stat *buf)
{
    printf("Device number(major/minor): %d/%d\n", major(buf->st_dev), minor(buf->st_dev));
    return;
}

// the number of links;
void GetNumOfLinks(struct stat *buf)
{
    printf("Number of links: %ld\n", buf->st_nlink);
    return;
}
// last access time, converted to the format used by the ls program (hint: Stevens & Rago, 6.10.);
void GetLastAccessTime(struct stat *buf)
{
    time_t accessTime = buf->st_atime;
    bool isRecent = IsTimeRecent(accessTime);
    char formattedTime[13];
    FormatTime(accessTime, isRecent, formattedTime);

    printf("Last access time: %s\n", formattedTime);
    return;
}
// last modification time, converted to the format used by the ls program;
void GetLastModifiedTime(struct stat *buf)
{
    time_t modifiedTime = buf->st_mtime;
    bool isRecent = IsTimeRecent(modifiedTime);
    char formattedTime[13];
    FormatTime(modifiedTime, isRecent, formattedTime);

    printf("Last modified time: %s\n", formattedTime);
    return;
}
// last time file status changed, converted to the format used by the ls program;
void GetLastStatusChange(struct stat *buf)
{
    time_t statusChangeTime = buf->st_ctime;
    bool isRecent = IsTimeRecent(statusChangeTime);
    char formattedTime[13];
    FormatTime(statusChangeTime, isRecent, formattedTime);

    printf("Last status changed time: %s\n", formattedTime);
    return;
}


int main(int argc, char *argv[])
{
    struct stat buf;
    int i;
    char cwd[PATH_MAX];


    //Read argv
    //if argc == 1
    if(argc == 1)
    {
        //Get list of files in current directory
        if(getcwd(cwd, sizeof(cwd))!= NULL)
        {
            DIR *dp;
            struct dirent *dirp;

            dp = opendir(cwd);

                printf("Directory path: %s\n\n", cwd);

            while((dirp = readdir(dp)) != NULL)
            {
                //Get file information for each file
                if(lstat(dirp->d_name, &buf) < 0)
                {
                    perror("lstat error");
                    continue;
                }
                //Print file information
                printf("FileName: %s\n", dirp->d_name);
                GetOwnerName(&buf);
                GetGroupName(&buf);
                GetFileType(&buf);
                GetPermission(&buf);
                GetFileSize(&buf);
                GetFileINode(&buf);
                GetDeviceNumber(&buf);
                GetNumOfLinks(&buf);
                GetLastAccessTime(&buf);
                GetLastModifiedTime(&buf);
                GetLastStatusChange(&buf);
                printf("\n");
            }
            closedir(dp);
            
        }
        else
        {
            perror("getcwd() error");
            exit(1);
        }
    }
    //If argc > 1
    if(argc > 1)
    {
        for(i = 1; i < argc; i++)
        {
            //For each argv
            //Get file information
            if(lstat(argv[i], &buf) < 0)
            {
                perror("lstat error");
                continue;
            }
            //Print file information
            printf("Filename: %s\n", argv[i]);
            GetOwnerName(&buf);
            GetGroupName(&buf);
            GetFileType(&buf);
            GetPermission(&buf);
            GetFileSize(&buf);
            GetFileINode(&buf);
            GetDeviceNumber(&buf);
            GetNumOfLinks(&buf);
            GetLastAccessTime(&buf);
            GetLastModifiedTime(&buf);
            GetLastStatusChange(&buf);
            printf("\n");
        }
    }
    
    exit(0);
}