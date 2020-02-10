//Amy Seidel
//CS4760 - OS 
//Project 1

#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <getopt.h>
#include <errno.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>



void printdir(char *dir);

//making all of the flags global so they can be changed in main and used in the function
int hflag,tflag, gflag, pflag, iflag, uflag, sflag, Lflag, dflag,  Iflag = 0;


int main( int argc, char *argv[] )  {
	
	//variables 
	int var;

	//while loop to execute getopt
	while ((var = getopt(argc, argv, "hILldgipstu")) != -1) { 
		switch(var) {
		  case 'h':
		   	hflag = 1;
		   	break;
		  case 'd':
			dflag = 1;
			break;
		  case 'I':
			Iflag = 1;
			break; 
		  case 'L':
			Lflag = 1;
			break;
		  case 't':
			tflag = 1;
			break;
		  case 'p': 
			pflag = 1;
			break;
		  case 'i':
			iflag = 1;
			break;
		  case 'u':
			uflag = 1;
			break;
		  case 'g':
			gflag = 1;
			break;
		  case 's':
			sflag = 1;
			break;
		  case 'l':
			uflag = 1;
			tflag = 1;
			iflag = 1;
			pflag = 1;
			gflag = 1;
			sflag = 1;
			break;
		case '?':
                perror("You chose an invalid option\n");
                exit(-1);
		}
	}
	if((hflag == 1)) {
		printf("HELP: Enter ./dt -options directory name. Directory name is optional\n");
        printf("-L will follow any symbolic links\n");
        printf("-t will print file type\n");
        printf("-p will print permission bits as rwxrwxrwx\n");
        printf("-i will print the number of links to file in inode table\n");
        printf("-u will print the UID\n");
        printf("-g will print the GID\n");
        printf("-s will print the size\n");
        printf("-d will show the time of last modification\n");
        printf("-l will show everything\n");
		exit(0);
	}

    char* root = ".";


    if (argv[optind] == NULL) {
        char origin[4096];
        getcwd(origin, sizeof(origin));
        root = origin;
    }
    else {
        root = argv[optind];
    }

    printf("directory scan of:  %s\n", root);

    printdir(root);

    //return 0;

}
/*****************************************
 * FUNCTION TO PRINT DIRECTORY AND OPTIONS
 *****************************************/

void printdir(char *dir)
{
    //Error checking if directory is real
    DIR *d = opendir(dir);
    if(errno == 2) {
        perror("Perror: ");
        exit(-1);
    }

    //variables for declaring directory and stat
    struct dirent *entry;
    entry = readdir(d);
    struct stat statbuf;
    lstat(entry->d_name, &statbuf);
    struct passwd* pwd;
    char* fileType = "";
    struct group* grp;


    while((entry = readdir(d)) != NULL) {
            char buffer[512];
            if (S_ISDIR(statbuf.st_mode)) {
                // ignoring "." and ".." directories
                if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
                    continue;

                snprintf(buffer, sizeof(buffer), "%s/%s", dir, entry->d_name);
                stat(buffer, &statbuf);

                //print type of file
                if (tflag == 1) {
                    if (S_ISDIR(statbuf.st_mode)) {
                        fileType = "d";
                        printf("%s", fileType);
                    } else if (S_ISLNK(statbuf.st_mode)) {
                        fileType = "s";
                        printf("%s", fileType);
                    } else {
                        fileType = "-";
                        printf("%s", fileType);
                    }
                }

                //print permissions
                if (pflag == 1) {
                   // printf((S_ISDIR(statbuf.st_mode)) ? "d" : "-");
                    printf((statbuf.st_mode & S_IRUSR) ? "r" : "-");
                    printf((statbuf.st_mode & S_IWUSR) ? "w" : "-");
                    printf((statbuf.st_mode & S_IXUSR) ? "x" : "-");
                    printf((statbuf.st_mode & S_IRGRP) ? "r" : "-");
                    printf((statbuf.st_mode & S_IWGRP) ? "w" : "-");
                    printf((statbuf.st_mode & S_IXGRP) ? "x" : "-");
                    printf((statbuf.st_mode & S_IROTH) ? "r" : "-");
                    printf((statbuf.st_mode & S_IWOTH) ? "w" : "-");
                    printf((statbuf.st_mode & S_IXOTH) ? "x" : "-");
                    printf("\t");
                }

                //print number of links to the inode
                if (iflag == 1) {

                    printf("%d\t", (unsigned int) statbuf.st_nlink);
                }

                //print UID
                if (uflag == 1) {
                    pwd = getpwuid(statbuf.st_uid);
                    if ((pwd = getpwuid(statbuf.st_uid)) != NULL)
                        printf("%-8.8s\t", pwd->pw_name);
                    else
                        printf("%-8d\t", statbuf.st_uid);
                }

                //print group id
                if (gflag == 1) {
                    grp = getgrgid(statbuf.st_gid);
                    if ((grp = getgrgid(statbuf.st_gid)) != NULL)
                        printf("%-8.8s\t", grp->gr_name);
                    else
                        printf("%-8d\t", statbuf.st_gid);
                }

                //Print size
                if (sflag == 1) {
                    if (statbuf.st_size > 1000)
                        printf("%5dK\t", statbuf.st_size / 1000);
                    else if (statbuf.st_size > 1000000)
                        printf("%5dM\t", statbuf.st_size / 1000000);
                    else if (statbuf.st_size > 1e+9)
                        printf("%5dG\t", statbuf.st_size / 1e+9);
                    else
                        printf("%5d\t", statbuf.st_size);
                }

                //print date and time
                if (dflag == 1) {
                    printf("%s\t", ctime(&statbuf.st_atime));
                }
            //    printf("\n");

                printf("%s/%s\n", dir, entry->d_name);
                if (S_ISDIR(statbuf.st_mode)) {
                    printdir(buffer);
                }
            }
            else{
                //print type of file
                if (tflag == 1) {
                    if (S_ISDIR(statbuf.st_mode)) {
                        fileType = "d";
                        printf("%s", fileType);
                    } else if (S_ISLNK(statbuf.st_mode)) {
                        fileType = "s";
                        printf("%s", fileType);
                    } else {
                        fileType = "-";
                        printf("%s", fileType);
                    }
                }
                //print permissions
                if (pflag == 1) {
                    // printf((S_ISDIR(statbuf.st_mode)) ? "d" : "-");
                    printf((statbuf.st_mode & S_IRUSR) ? "r" : "-");
                    printf((statbuf.st_mode & S_IWUSR) ? "w" : "-");
                    printf((statbuf.st_mode & S_IXUSR) ? "x" : "-");
                    printf((statbuf.st_mode & S_IRGRP) ? "r" : "-");
                    printf((statbuf.st_mode & S_IWGRP) ? "w" : "-");
                    printf((statbuf.st_mode & S_IXGRP) ? "x" : "-");
                    printf((statbuf.st_mode & S_IROTH) ? "r" : "-");
                    printf((statbuf.st_mode & S_IWOTH) ? "w" : "-");
                    printf((statbuf.st_mode & S_IXOTH) ? "x" : "-");
                    printf("\t");
                }

                //print number of links to the inode
                if (iflag == 1) {

                    printf("%d\t", (unsigned int) statbuf.st_nlink);
                }
                //print UID
                if (uflag == 1) {
                    pwd = getpwuid(statbuf.st_uid);
                    if ((pwd = getpwuid(statbuf.st_uid)) != NULL)
                        printf("%-8.8s\t", pwd->pw_name);
                    else
                        printf("%-8d\t", statbuf.st_uid);
                }

                //print group id
                if (gflag == 1) {
                    grp = getgrgid(statbuf.st_gid);
                    if ((grp = getgrgid(statbuf.st_gid)) != NULL)
                        printf("%-8.8s\t", grp->gr_name);
                    else
                        printf("%-8d\t", statbuf.st_gid);
                }

                //Print size
                if (sflag == 1) {
                    if (statbuf.st_size > 1000)
                        printf("%5dK\t", statbuf.st_size / 1000);
                    else if (statbuf.st_size > 1000000)
                        printf("%5dM\t", statbuf.st_size / 1000000);
                    else if (statbuf.st_size > 1e+9)
                        printf("%5dG\t", statbuf.st_size / 1e+9);
                    else
                        printf("%5d\t", statbuf.st_size);
                }

                //print date and time
                if (dflag == 1) {
                    printf("%s\t", ctime(&statbuf.st_atime));
                }
                printf("%s/%s\n", dir, entry->d_name);
            }
      //  if (S_ISDIR(statbuf.st_mode)) {
    //        printdir(buffer);
     //   }

    }
    closedir(d);
}

