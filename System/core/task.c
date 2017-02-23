/* 2009/08/20 ElectroLinux
 * Felipe Astroza
 * Curicó
 * file: task.c
 * Kepler tasks
 */
#include <task.h>
#include <stdlib.h>

#ifndef TASKS_MAX
#define TASKS_MAX 5
#endif

static kpl_task tasks_base[TASKS_MAX];
static kpl_task *cur_task;

inline static kpl_task *_task_get_free_slot();
inline static kpl_task *_task_get(int8_t);

/**
 * Consigue el identificador de una tarea.
 * @param task tarea.
 * @return Task ID.
 */
 inline int8_t kpl_task_get_tid(kpl_task *task)
{
	return task - tasks_base;
}

/**
 * Asigna tarea a slot disponible.
 * @param main Funcion principal de la tarea.
 * @return Identificador de la tarea creada, si no hay un slot disponible retorna -1.
 */
int8_t kpl_task_create(kpl_task_main main)
{
	kpl_task *task;

	task = _task_get_free_slot();

	if(task == NULL)
		return -1;

	task->main = main;
	task->pos = 0;

	return kpl_task_get_tid(task);
}

/**
 * Termina una tarea (libera un slot)
 * @param tid Identificador de tarea
 */
void kpl_task_exit(int8_t tid)
{
	kpl_task *task = _task_get(tid);

	if(task != NULL) {
		task->main = NULL;
		task->port.busy = 0;
	}
}

/**
 * Consigue el puerto de una tarea.
 * @param tid Identificador de tarea.
 * @return Si la tarea con identificador tid existe retorna su puerto, sino NULL.
 */
kpl_port *kpl_task_get_port(int8_t tid)
{
	kpl_task *task = _task_get(tid);

	if(task->main != NULL)
		return &task->port;

        return NULL;
}

/**
 * @return Identificador de tarea en ejecucion
 */
int8_t kpl_task_get_current()
{
	return kpl_task_get_tid(cur_task);
}

int main()
{
	do {
		for(cur_task = tasks_base; 
		cur_task < (tasks_base + TASKS_MAX); 
		cur_task++) {
			if(cur_task->main != NULL) {
				cur_task->main(cur_task);
				sleep(1);
			}
		}
	} while(1);

	return 0;
}

static kpl_task *_task_get_free_slot()
{
	register int8_t i;

	for(i = 0; i < TASKS_MAX; i++)
		if(tasks_base[i].main == NULL)
			return tasks_base + i;

	return NULL;
}

inline static kpl_task *_task_get(int8_t tid)
{
	if(tid < 0 || tid >= TASKS_MAX)
		return NULL;

	return tasks_base + tid;
}
