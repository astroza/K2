#ifndef KEPLER_H

typedef void (*ktask)();
#define TASK_ADDR_NAME(name) __task_ ## name
#define TASK(name) void name(void); \
		   ktask TASK_ADDR_NAME(name) __attribute__(( section(".task_array"), unused)) = name; \
		   void name(void)

#define INIT_ADDR_NAME(name) __init_ ## name
#define INIT(name) void name(void); \
		   ktask INIT_ADDR_NAME(name) __attribute__(( section(".init_array"), unused)) = name; \
		   void name(void)

#endif
