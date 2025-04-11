
#define C_TO_S "../fifos/c_to_s" // Client TO Server



typedef struct msg {
    char flag[3];
    char argv[4][200];
    pid_t pid;
} MSG;



