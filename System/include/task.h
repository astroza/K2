#ifndef KEPLER_H
#define KEPLER_H

#include "port.h"

typedef struct _kpl_task kpl_task;
typedef void (*kpl_task_main)(kpl_task *);

struct _kpl_task {
	void *pos;
	kpl_task_main main;

	kpl_port port;
};

typedef struct {
	void *pos;
} kpl_simple_task;

int8_t 		kpl_task_get_tid(kpl_task *task);
int8_t 		kpl_task_create(kpl_task_main main);
void 		kpl_task_exit(int8_t tid);
kpl_port 	*kpl_task_get_port(int8_t tid);
int8_t 		kpl_task_get_current();

#define KPL_TASK_RESUME(t) 		\
	void *pos = t->pos; 		\
	t->pos = 0; 			\
	if(pos != 0) goto *pos;

#define TASK(name) 			\
	void name(kpl_task *t)

#define TASK_PRIVATE(data_name, vars) 	\
	static struct { 		\
		vars; 			\
	} data_name;

#define TASK_PROGRAM 			\
	KPL_TASK_RESUME(t);


#define RLABEL(line) lb_ ## line
#define LABEL(line) RLABEL(line)
#define WAIT_WHILE(cond) 							\
LABEL(__LINE__):								\
	if(cond) {								\
		t->pos = &&LABEL(__LINE__); 					\
		printf("WAIT: Task=%d Line=%d\n", kpl_task_get_tid(t), __LINE__); 	\
		return;								\
	}

#define WAIT_UNTIL(cond) WAIT_WHILE(!(cond))

#define INIT(name) void __attribute__((constructor)) name()
#endif
