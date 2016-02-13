// IMPLEMENTATION OF "ls -lLR" LINUX COMMAND

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>	//stat
#include <sys/types.h>	//getpwid
#include <pwd.h>		//getpwid
#include <grp.h>		//getgrgid
#include <unistd.h>
#include <time.h>		//time


char *fullpath; 		// contains full path
char *sympath;			// contains symblink path
char *backpath;			// back up path
int symflag = 0, cflag = 0;
struct data {
	char mode[11];
	int nlink;
	char *usrname;
	char *grpname;	
	int size;
	char *time;
	char *name;
} ;
//struct data *strarr;
//int strcount = 0;
void mode_conv(long int,char * );
void print_ls();
void print_data(struct dirent *, struct data *, int );
void symlink_check(int );
char type_find(long int );
void time_conv(time_t t, char *time);
void sort_file(struct data *, int);

int main(int argc, char *argv[])
{
	if(argc < 2)
		printf("ERROR LESS ARGUMENT\n");

	char *pathname = argv[1];
	if (pathname == '\0')
		printf("ERROR NULL PATH\n");

	int len = strlen(pathname);
	fullpath = malloc(1024 * sizeof(char));
	sympath = malloc(1024 * sizeof(char));
	backpath = malloc(1024 * sizeof(char));
	strncpy(fullpath, pathname ,len);
	//strarr = malloc(20 * sizeof(struct data));
	//printf("LENGTH: %d\n", len);
	//printf("%s\n", fullpath);
	
	print_ls();

}

void print_ls()
{
	struct stat statbuf;
	struct dirent *dirp;   //For readdir()
	DIR *dp;
	char *ptr, *sptr;
	struct data *strarr;	
	
	if (lstat(fullpath, &statbuf) < 0) /* stat error */
	{
		printf("ERROR FILE WE CAN NOT STAT\n");
	}
	if (S_ISDIR(statbuf.st_mode) == 0) /* not a directory */
	{	
		// do nothing
		//printf("ERROR FILE OTHER THAN DIRECTORY\n");
	}
	else
	{
			
		if (symflag == 0)
		{
			printf("%s:\n", fullpath);
			
		}
		else
		{
			cflag++;
			printf("%s:\n", sympath);
			int splen;		
			splen = strlen(sympath);
			sptr = sympath + splen; 			// point to end of fullpath 
			*sptr++ = '/';
			*sptr = 0;
		}		
		int fplen;		
		fplen = strlen(fullpath);
		ptr = fullpath + fplen; 			// point to end of fullpath 
		*ptr++ = '/';
		*ptr = 0;
		//printf("FULLPATH IN FUNCTION: %s\n", fullpath);

		if ((dp = opendir(fullpath)) == NULL)		//can't read dir
			printf("ERROR CAN NOT READ DIRECTORY: %s\n", fullpath);
		else {
			//STORE ALL FILE INFO AND SORT 
			printf("directory %s opened\n", fullpath);
			//struct data *strarr = (struct data *)malloc(20);	//allocate memory
			struct data strarr[20];
			//printf("struct data of size %ld allocated\n", sizeof(struct data ));			
			int strcount = -1;
			while ((dirp = readdir(dp)) != NULL)
			{
				if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
					continue; 			// ignore dot and dot-dot 
				strcpy(ptr, dirp->d_name);		// append name after slash to get fullpath
				//if(strcount == 19)
				//{
					//strarr = realloc(strarr,10 * sizeof(struct data ));
					strcount++;
				//}else
				strcount++;
				print_data(dirp,strarr, strcount);				// sort based on filename, pass strarr pointer and strcount
				printf("%s\n", strarr[0].name );
			}//WHILE ENDS
			//SORT ALGO
			printf("%s\n", strarr[0].name );
			sort_file(strarr, strcount);
		
			if(closedir(dp) < 0)		//CLOSE DIR
				printf("CAN NOT CLOSE THE DIRECTORY:%s\n", fullpath);
			ptr = fullpath + fplen +1;	//REDO the fullpath change
			*(ptr) =0;
			
				//printf("PRINT FULLPATH: %s\n", fullpath);

			printf("\n");		//FORMATING....
			//DO RECURSION
			int i;
			for	(i = 0; i < strcount; i++  )
			{
				strcpy(ptr, strarr[i].name); // append name after slash
				if (symflag == 1) strcpy(sptr, strarr[i].name);
				symlink_check(fplen);	//check symlink							

				print_ls();			//RECURSIVE CALL
				if (symflag == 0)
				{
					ptr = fullpath + fplen +1;	//REDO the fullpath change
					*(ptr) =0;
				}
				else {
					ptr = fullpath + fplen +1;	//REDO the fullpath change
					*(ptr) =0;
					if (cflag !=0)
						cflag--;
					else
						strncpy(fullpath, backpath, strlen(backpath));
				}
			
			}//FOR ENDS
			symflag = 0;	 //DISABLE FLAG
			//free(strarr);	//FREE struct array
		}//Inner IF END		
	}//IF END
}//FUNCTION END


void print_data(struct dirent *dire, struct data *strarr, int strcount)
{
	struct stat statbuft;
	struct passwd *passtrt;
	struct group *grp;
	struct tm lt; 
	
	
	printf("%s \n", fullpath ); 	//gives correct answer
	strarr[strcount].name = dire->d_name;
	printf("%s\n", strarr[strcount].name );
	if (stat(fullpath, &statbuft) < 0) //use stat to deal with symlinks
	{
		printf("ERROR FILE/DIR WE CAN NOT STAT\n");
	}
	else
	{
		mode_conv(statbuft.st_mode, strarr[strcount].mode);
		strarr[strcount].nlink = statbuft.st_nlink;
		strarr[strcount].size = statbuft.st_size;

		passtrt = getpwuid(statbuft.st_uid);
		strarr[strcount].usrname = passtrt->pw_name;

		grp = getgrgid(statbuft.st_gid);
		strarr[strcount].grpname = grp->gr_name;

		strarr[strcount].time = malloc(100* sizeof(char));
		time_conv(statbuft.st_mtime, strarr[strcount].time);
	
		//printf("%s %d %s %s %d %s %s\n",strarr[strcount]->mode, strarr[strcount]->nlink,
		//			strarr[strcount]->usrname, strarr[strcount]->grpname, strarr[strcount]->size,
		//			strarr[strcount]->time,strarr[strcount]-> name);
		//free(time);
		fflush(stdout);
	}
	
}
void sort_file(struct data *strarr, int n)
{
	int i, j, r;
	struct data d; 
	printf("%s\n", strarr[0].name );
	printf("SORTING.......BEFORE\n");
	//printf("First filename: %s\n", strarr[0].name );
	for (i = 0; i < n; i++)
	{
		printf("%s %d %s %s %d %s %s\n",strarr[i].mode, strarr[i].nlink,
				strarr[i].usrname, strarr[i].grpname, strarr[i].size,
				strarr[i].time,strarr[i].name);
		free(strarr[i].time);
	}
	for (i = 0; i< n; i++)
	{
		for (j = i; j< n; j++ )
			{
				//printf("BEFORE:strarr[j].name: %s, strarr[j+1].name: %s\n", strarr[j].name, strarr[j+1].name );
				r = strcoll(strarr[j].name, strarr[j+1].name);

				if (r > 0)
				{
					d = strarr[j];
					strarr[j] = strarr[j+1] ;
					strarr[j+1] = d;
				}
				//printf("AFTER: strarr[j].name: %s, strarr[j+1].name: %s\n", strarr[j].name, strarr[j+1].name );
			}
	}
	printf("AFTER................\n");
	for (i = 0; i < n; i++)
	{
		printf("%s %d %s %s %d %s %s\n",strarr[i].mode, strarr[i].nlink,
				strarr[i].usrname, strarr[i].grpname, strarr[i].size,
				strarr[i].time,strarr[i].name);
		free(strarr[i].time);
	}
}
void symlink_check(int fplen)
{
	struct stat statbuf;
	char *ptr;
	char *cont;	

	if (lstat(fullpath, &statbuf) < 0) //for symlink 
	{
		printf("ERROR FILE WE CAN NOT STAT\n");
	}
	if (S_ISLNK(statbuf.st_mode) != 0) // SYMLINK
	{	
		//check it is pointing to directory or file
		//if file than do nothing 
		//else copy fullpath in sympath
		//make fullpath = path to that directory
		if (stat(fullpath, &statbuf) < 0) 
		{
			printf("ERROR FILE/DIR WE CAN NOT STAT\n");
		}
		else
		{
			//check pointing to file or directory
			if (S_ISDIR(statbuf.st_mode) == 0) //true = not dir
			{	
				// do nothing
				//printf("ERROR FILE OTHER THAN DIRECTORY\n");
			}
			else
			{
				symflag = 1;
				//cflag = 1;
				strncpy(sympath, fullpath ,strlen(fullpath));
				strncpy(backpath, fullpath, fplen+1);
				ptr = fullpath;
				if (readlink(fullpath, cont, 1024) == -1)
					printf("ERROR READING CONTENT OF SYMLINK\n");
				else
					strncpy(ptr, cont, strlen(cont)-2);
					
				printf("FULLPATH: %s\n",fullpath);
				printf("BACKPATH: %s\n",backpath);
				printf("SYMPATH: %s\n",sympath);	
			}
		}
	}

	

}

void mode_conv(long int mode, char *modecon)
{
	
	modecon[0] = type_find(mode); 
	modecon[1] = (mode &  S_IRUSR) ? 'r' : '-';
	modecon[2] = (mode &  S_IWUSR) ? 'w' : '-';
	modecon[3] = (mode &  S_IXUSR) ? 'x' : '-';
	modecon[4] = (mode &  S_IRGRP) ? 'r' : '-';
	modecon[5] = (mode &  S_IWGRP) ? 'w' : '-';
	modecon[6] = (mode &  S_IXGRP) ? 'x' : '-';
	modecon[7] = (mode &  S_IROTH) ? 'r' : '-';
	modecon[8] = (mode &  S_IWOTH) ? 'w' : '-';
	modecon[9] = (mode &  S_IXOTH) ? 'x' : '-';

	if (mode & S_ISUID)
        modecon[3] = (mode & S_IXUSR) ? 's' : 'S';
    if (mode & S_ISGID)
        modecon[6] = (mode & S_IXGRP) ? 's' : 'l';
    if (mode & S_ISVTX)
        modecon[9] = (mode & S_IXUSR) ? 't' : 'T';
	modecon[10] = '\0';
//	return modecon;

}

void time_conv(time_t t, char *time1)
{
	
	struct tm lt; 		// for st_mtime
	struct tm *ct;		//for local time
	char cyear[10];		//hold current year
	char buffer[10];	//hold file modification year
	
	time_t rawtime;
	time( &rawtime );
	
	ct = localtime( &rawtime );
	strftime(cyear,sizeof(cyear),"%Y", ct);

	localtime_r(&t, &lt);	
	strftime(buffer,sizeof(buffer),"%Y", &lt);

	if ((atoi(buffer)) == (atoi(cyear)))
	{	
		strftime(time1,100,"%b %d %H:%M", &lt);
	}
	else
	{
		strftime(time1,100,"%b %d %Y", &lt);
	}
}

char type_find(long int mode)
{
	char type;
	if (S_ISREG(mode))			//regular file
		type = '-';
	else if (S_ISDIR(mode)) 	//directory file
		type = 'd';
	else if (S_ISCHR(mode)) 	//character special file
		type = 'c';
	else if (S_ISBLK(mode)) 	//block special file
		type = 'b';
	else if (S_ISFIFO(mode))	//pipe or FIFO
		type = 'p';
	else if (S_ISLNK(mode)) 	//symbolic link
		type = 'l';
	else if (S_ISSOCK(mode)) 	//socket
		type = 's';

	return type;
}
