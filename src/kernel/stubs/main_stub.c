typedef int bool;
#define true 1
#define false 0

int multibit_memory_init(void){return 0;}
int memory_tools_init(void){return 0;}
int debugger_init(void){return 0;}
int terminal_init(void){return 0;}
int repl_init(void){return 0;}
int desktop_init(void){return 0;}
int terminal_printf(const char* fmt,...){return 0;}
int terminal_handle_input(int code){return 0;}
int terminal_start(void){return 0;}
int repl_start(void){return 0;}
int desktop_start(void){return 0;}

typedef struct {
    int version_major, version_minor, version_patch, status;
} kernel_state_t;
kernel_state_t g_kernel_state = {0};

const char* kernel_get_version_string(void){return "0.0.0-stub";}
const kernel_state_t* kernel_get_state(void){return &g_kernel_state;}

int kernel_early_init(void){return 0;}
int kernel_init(void){return 0;}
int kernel_main(void){return 0;}

typedef struct { int sse2, tm, lm; } cpu_features_t;
typedef struct { cpu_features_t features; } cpu_info_t;
cpu_info_t cpu_info = {0};

