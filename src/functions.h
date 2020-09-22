#ifndef FUNCTIONS_H
#define FUNCTIONS_H

int ChangeDirectory(char *);
int Total(char *);
void CurrentDirectory();
void Echo(char *[]);
void Permissions(char *);
void User(char *, char *, int);
void Group(char *, char *, int);
void Links(char *, char *, int);
void Size(char *, char *, int);
void LastAccessTime(char *);
void PathModifier(char *);
void ReversePathModifier(char *);
int Write_Redirect(int *, char *);
int Append_Redirect(int *, char *);
int Read_Redirect(int *, char *);
void Return_To_STDIN(int, int);
void Return_To_STDOUT(int, int);
int max(int, int);

#endif