#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <unistd.h>
#define STR_SIZE 1024

void copy_str(char **str1, const char *str2){
	int i = 0;
	for(i; str2[i] != 0; i++)
		(*str1)[i] = str2[i];
	(*str1)[i] = 0;
}

int equals_str(const char* str1, const char* str2){
	int i = 0;
	for(i; str1[i] != 0 && str2[i] != 0; i++)
		if(str1[i] != str2[i])
			return 0;
	return 1;
}

int cocatinate_str(char **str, const char* str2){
	int i = 0;
	for(i; (*str)[i] != 0; i++){}
	for(int j = 0; str2[j] != 0; j++)
		(*str)[i++] = str2[j];
	(*str)[i] = 0;
}

typedef struct node{
char *data;
struct node* next;		
}node;

typedef struct stack{
	node* top;
}stack;

void push(stack **st, const char* data){
	node *tmp = malloc(sizeof(node));
	tmp->data = malloc(STR_SIZE);
	copy_str(&tmp->data, data);
	tmp->next = NULL;
	if((*st)->top == NULL){
		(*st)->top = tmp;
		return;	
	}
	tmp->next = (*st)->top;
	(*st)->top = tmp;
}

int pop(stack *st, char **data){
	if(st->top == NULL)
		return -1;
	node *tmp = st->top;
	copy_str(data, tmp->data);
	st->top = st->top->next;
	free(tmp->data);
	free(tmp);
	return 0;
}

int in_stack(const stack st, const char* data){
	node* tmp = st.top;
	while(tmp != NULL){
		if(equals_str(data, tmp->data))
			return 1;
		tmp = tmp->next;
	}
	return 0;
}

void search(char *dir, void (*callback)(char*, char*, int,  void*), void *userdata){
	DIR *d;
	struct stat st;
	struct dirent *dirent;
	stack *stc = malloc(sizeof(stack));
	stc->top = NULL;
	push(&stc, dir);
	char *str = malloc(STR_SIZE);
	while(!pop(stc, &str)){
		d = opendir(str);
		if(d == NULL){
			printf("Error in: %s\n", str);
			perror("Open Directory exception");
			continue;
		}
		while((dirent = readdir(d)) != NULL){
			char *tmp = malloc(STR_SIZE);
			tmp[0] = 0;
			cocatinate_str(&tmp, str);
			cocatinate_str(&tmp, "/");
			cocatinate_str(&tmp, dirent->d_name);
			if(in_stack(*stc, tmp)) continue;
			if(lstat(tmp, &st) != 0){
				printf("Error in: %s\n", tmp);
				perror("Stat exception");
				continue;
			}
			if((st.st_mode & S_IFMT) == S_IFDIR && !equals_str(dirent->d_name, ".") && !equals_str(dirent->d_name, ".."))
				push(&stc, tmp);
			(*callback)(str, dirent->d_name, st.st_mode & S_IFMT, userdata);
			free(tmp);
		}
		closedir(d);
	}
	free(str);
	free(stc);
}

void callback(char* dir, char *name, int type, void *userdata){
	if(equals_str(name, *((char**) (userdata))))
		printf("%s/%s\n", dir, name);
}

int main(int argc, char **argv){
	if(argc < 3){
		printf("Error: No more arguments!\n");
		return 1;
	}
	char *str = argv[2];
	int p = fork();
	if(p == 0){
		search(argv[1], &callback, &str);
	}
	else{
		wait(NULL);
	}
	return 0;
}
