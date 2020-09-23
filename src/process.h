#ifndef PROCESS_H
#define PROCESS_H

void Run_FG(char *[]);
void Run_BG(char *[]);
void Pinfo(int);
void AddProcess(char *, int);
int DeleteProcess(char *, int);
void SIGCHLD_handler();
void SIGTTIN_handler();
void SIGTTOU_handler();
void Jobs();
void Kjob(char *[]);
void Overkill();
void Fg(char *[]);

#endif