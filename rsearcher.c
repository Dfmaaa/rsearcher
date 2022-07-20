#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#define TRUE 1
#define FALSE 0
#define STOP 2
#define DEC "================================="
#define MAX 2147483647
short int cs;
short int stop;
short int stop_onem;
short int display_found;
unsigned long long int first=0L;
unsigned long long int last=0L;
unsigned long long int line_num=1L;
short int onem;
struct node{
    char *key;
    struct node *next;
};
typedef struct node node;
node *global_file_list=NULL;
void insert(node **, char *);
short int isdir(char *);
short int optsearch(char **,char *, int, int (*)(char *, char *));
short int loptsearch(char **,char *, int, int (*)(char *, char *), short int *);
char *make_str(char);
void print_key_str(char *,char *, unsigned long long int, int (*)(char *, char *));
void help(char *);
char uppercase(char);
int strcmpi(char *, char *);
void one_file(char *, char *);
node *extract(char *, char *);
void display_col(char *, unsigned long long int, unsigned long long int);
void turn_blue_on();
void turn_red_on();
void default_col();
void append_lt(node **, node *);
void gdir(char *, short int);
void insert(node **head,char *key){
    if(head==NULL){
        *head=(node*)malloc(sizeof(node));
        if(*head==NULL){
            perror("[malloc]");
            exit(EXIT_FAILURE);
        }
        (**head).key=key;
        (**head).next=NULL;
        return;
    }
    node *selected=*head;
    while((*selected).next!=NULL){
        selected=(*selected).next;
    }
    (*selected).next=(node*)malloc(sizeof(node));
    if((*selected).next==NULL){
        perror("[malloc]");
        exit(EXIT_FAILURE);
    }
    (*(*selected).next).key=key;
    (*(*selected).next).next=NULL;
}
short int isdir(char *name){
    struct stat cdir;
    if(stat(name,&cdir)==-1){
        perror("[stat]");
        exit(EXIT_FAILURE);
    }
    return (cdir.st_mode&__S_IFMT)==__S_IFDIR;
}
short int optsearch(char **inp_arr, char *key, int inp_len,int (*strcompare_abst)(char *,char *)){
    register int start=1;
    register int end=inp_len-1;
    while(start<=end){
        if(strcompare_abst(*(inp_arr+start++),key)==0){
            return TRUE;
        }
        else if(strcompare_abst(*(inp_arr+end--),key)==0){
            return TRUE;
        }
    }
    return FALSE;
}
char *make_str(char ch){
    char *ret_str=(char*)malloc(sizeof(char)*2);
    if(ret_str==NULL){
        perror("[malloc]");
        exit(EXIT_FAILURE);
    }
    ret_str[0]=ch;
    ret_str[1]='\0';
    return ret_str;
}
void print_key_str(char *full_arr, char *key, unsigned long long int len, int (*strcompare_abst)(char *,char *)){
    register unsigned long long int lp1;
    register unsigned long long int lp2;
    char *full_str;
    for(lp1=0;lp1<len;lp1++){
        full_str=make_str(full_arr[lp1]);
        if(strcompare_abst(full_str,key)==0){
            first=lp1;
            last=lp1;
            printf("[%llu]\tMatch found. First character: %llu, Last character: %llu.\n",line_num,first,last);
            if(display_found==TRUE){
                printf("%s\n",DEC);
                display_col(full_arr,first,last);
                printf("%s\n",DEC);
            }
            free(full_str);
            if(stop==TRUE){
                exit(EXIT_SUCCESS);
            }
            if(stop_onem==TRUE){
                onem=TRUE;
                return;
            }
        }
        for(lp2=lp1+1;lp2<len;lp2++){
            full_str=strcat(full_str,make_str(full_arr[lp2]));
            if(strcompare_abst(full_str,key)==0){
                first=lp1;
                last=lp1;
                printf("[%llu]\tMatch found. First character: %llu, Last character: %llu.\n",line_num,first,last);
                free(full_str);
                if(display_found==TRUE){
                    printf("%s\n",DEC);
                    display_col(full_arr,first,last);
                    printf("%s\n",DEC);
                }
                if(stop==TRUE){
                    exit(EXIT_SUCCESS);
                }
                if(stop_onem==TRUE){
                    onem=TRUE;
                    return;
                }
            }
        }
        free(full_str);
    }
}
void help(char *execname){
    printf("The format is %s [options] [files, directories] [keyword]. Some options are:\n-h\tfor displaying this text.\n-r\trecursive(for directories)\n-case\tcase sensitive searching\n-s\tfor stopping when a match is found.\n-d\tfor displaying line number and highlighting text in red\n-ofs\tsearch every file for one match.\n--files [files seperated by ,]\tfor selecting files\n--dirs [directories seperated by ,]\tfor selecting directories\n",execname);
}
char uppercase(char ch){
    if((int)ch>=97){
        return (char)((int)ch-32);
    }
    else{
        return ch;
    }
}
int strcmpi(char *c1, char *c2){
    int len1=strlen(c1);
    int len2=strlen(c2);
    if(len1!=len2){
        return 1;
    }
    register int counter1=0;
    while((*c1+counter1)!='\0'){
        if(uppercase(*(c1+counter1))!=uppercase(*(c2+counter1))){
            return 1;
        }
        counter1++;
    }
    return 0;
}
void one_file(char *name, char *keyword){
    line_num=1;
    FILE *ptr=fopen(name,"r");
    if(ptr==NULL){
        printf("%s doesn't exist.\n",name);
        return;
    }
    char *str;
    turn_blue_on();
    puts(name);
    default_col();
    while(fgets(str,MAX,ptr)!=EOF){
        print_key_str(str,keyword,strlen(str),cs==1?(int (*)(char *, char *))strcmp:(int (*)(char *, char *))strcmpi);
        if(onem==TRUE&&stop_onem==TRUE){
            return;
        }
        line_num++;
        }
    fclose(ptr);
}
node *extract(char *text_arr, char *delm){
    node *lt=NULL;
    char *otpt=strtok(text_arr,",");
    insert(&lt,otpt);
    while((otpt=strtok(NULL,","))!=NULL){
        insert(&lt,otpt);
    }
    return lt;
}
void display_col(char *text, unsigned long long int x, unsigned long long int y){
    register unsigned long long int counter=0;
    short int flag=0;
    while(*(text+counter)!='\0'){
        if(counter<x||counter>y){
            putchar(*(text+counter));
            counter++;
        }
        else{
            if(counter==x){
                turn_red_on();
            }
            putchar(*(text+counter));
            if(counter==y){
                default_col();
            }
            counter++;
        }
    }
    printf("\n");
}
void turn_blue_on(){
    printf("\033[0;34m");
}
void turn_red_on(){
    printf("\033[1;31m");
}
void default_col(){
    printf("\033[0m");
}
short int loptsearch(char **inp_arr, char *key, int inp_len,int (*strcompare_abst)(char *,char *), short int *pos){
    register int start=1;
    register int end=inp_len-1;
    while(start<=end){
        if(strcompare_abst(*(inp_arr+start++),key)==0){
            *pos=start-1;
            return TRUE;
        }
        else if(strcompare_abst(*(inp_arr+end--),key)==0){
            *pos=end+1;
            return TRUE;
        }
    }
    return FALSE;
}
void append_lt(node **lt1, node *lt2){
    node *current1=*lt1;
    node *current2=lt2;
    if(current1==NULL){
        *lt1=current2;
        return;
    }
    while((*current1).next!=NULL){
        current1=(*current1).next;
    }
    (*current1).next=current2;
}
void gdir(char *name, short int recursive){
    DIR *dp=opendir(name);
    if(dp==NULL){
        printf("%s doesn't exist.\n",name);
        return;
    }
    struct dirent *di;
    while((di=readdir(dp))!=NULL){
        if(isdir((*di).d_name)==FALSE){
            insert(&global_file_list,(*di).d_name);
        }
        else if(recursive==TRUE){
            gdir((*di).d_name,recursive);
        }
    }
    closedir(dp);
}
int main(int argc, char **argv){
    if(argc==1){
        help(argv[0]);
        exit(EXIT_SUCCESS);
    }
    if(optsearch(argv,"-h",argc-1,strcmpi)==TRUE){
        help(argv[0]);
    }
    char *keyword=argv[argc-1];
    short int dir_pos, file_pos;
    node *dir_lt, *file_lt;
    short int recursive=optsearch(argv,"-r",argc-1,strcmpi);
    short int dir_list=loptsearch(argv,"--dirs",argc-1,strcmp,&dir_pos);
    short int file_list=loptsearch(argv,"--files",argc-1,strcmp,&file_pos);
    if(dir_list==TRUE&&dir_pos<argc-3){
        dir_lt=extract(argv[dir_pos+1],",");
        node *current=dir_lt;
        while(current!=NULL){
            gdir((*current).key,recursive);
            current=(*current).next;
        }
    }
    if(file_list==TRUE&&file_pos<argc-3){
        file_lt=extract(argv[file_pos+1],",");
        append_lt(&global_file_list,file_lt);
    }
    if(file_list==FALSE&&dir_list==FALSE){
        gdir(".",recursive);
    }
    stop_onem=optsearch(argv,"-ofs",argc-1,strcmpi);
    cs=optsearch(argv,"-case",argc-2,strcmpi);
    stop=optsearch(argv,"-s",argc-1,strcmpi);
    display_found=optsearch(argv,"-d",argc-1,strcmpi);
    node *current=global_file_list;
    while(current!=NULL){
        one_file((*current).key,keyword);
        current=(*current).next;
    }
    return 0;
}