#ifndef FUNCTIONS_H
#define FUNCTIONS_H

int ChangeDirectory(char *);
int Total(char *);
void CurrentDirectory(char *);
void Echo(char *);
void Permissions(char *);
void User(char *, char *, int);
void Group(char *, char *, int);
void Links(char *, char *, int);
void Size(char *, char *, int);
void LastAccessTime(char *);
void PathModifier(char *);
void ReversePathModifier(char *);
int max(int, int);

#endif