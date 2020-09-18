#ifndef PROCESS_H
#define PROCESS_H

void Run_FG(char *[], int, int, char *, char *);
void Run_BG(char *[], int, int, char *, char *);
void Pinfo(int, int, char *);
void AddProcess(char *, int);
void handler();

#endif