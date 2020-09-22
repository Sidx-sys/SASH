#ifndef PROCESS_H
#define PROCESS_H

void Run_FG(char *[]);
void Run_BG(char *[]);
void Pinfo(int);
void AddProcess(char *, int);
int DeleteProcess(char *, int);
void handler();
void Jobs();

#endif