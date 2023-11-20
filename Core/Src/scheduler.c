/*
 * scheduler.c
 *
 *  Created on: Nov 19, 2023
 *      Author: Admin
 */

#include "scheduler.h"

sTasks SCH_tasks_G[SCH_MAX_TASKS];

uint8_t current_index_task = 0;

uint8_t newTaskPos = 0;

void SCH_Reset_Task(int TASK_IDX)
{
	SCH_tasks_G[TASK_IDX].pTask = 0x0000;
	SCH_tasks_G[TASK_IDX].Delay = 0;
	SCH_tasks_G[TASK_IDX].Period = 0;
	SCH_tasks_G[TASK_IDX].RunMe = 0;
	SCH_tasks_G[TASK_IDX].TaskID = 0;
}

void SCH_Place_New_Task(int index)
{
	for (int i = current_index_task - 1; i > index; i--)
	{
		SCH_tasks_G[i] = SCH_tasks_G[i - 1];
	}

	SCH_Reset_Task(index);
}

void SCH_Add_Task(void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD)
{
	sTasks taskTemp;
	taskTemp.pTask = pFunction;
	taskTemp.Delay = DELAY;
	taskTemp.Period = PERIOD;
	taskTemp.RunMe = 0;
	taskTemp.TaskID = current_index_task;

	if (current_index_task < SCH_MAX_TASKS)
	{
		if (current_index_task == 0)
		{
			SCH_tasks_G[0] = taskTemp;
			current_index_task++;
			return;
		}

		for (; newTaskPos < current_index_task; newTaskPos++)
		{
			if (taskTemp.Delay > SCH_tasks_G[newTaskPos].Delay)
				taskTemp.Delay -= SCH_tasks_G[newTaskPos].Delay;
		}

		current_index_task++;
		SCH_Place_New_Task(newTaskPos);
		SCH_tasks_G[newTaskPos] = taskTemp;
		SCH_tasks_G[newTaskPos + 1].Delay -= SCH_tasks_G[newTaskPos].Delay;
	}
}

void SCH_Dispatch_Tasks(void)
{
	while (SCH_tasks_G[0].pTask != 0x0000)
	{
		if (SCH_tasks_G[0].RunMe > 0)
		{
			SCH_tasks_G[0].RunMe--;
			(*SCH_tasks_G[0].pTask)();

			if (SCH_tasks_G[0].Period > 0)
				SCH_Add_Task(SCH_tasks_G[0].pTask, SCH_tasks_G[0].Period, SCH_tasks_G[0].Period);
			SCH_Delete_Tasks(0);
		}
	}
}

void SCH_Update(void)
{
	if (SCH_tasks_G[0].Delay > 0)
		SCH_tasks_G[0].Delay--;
	if (SCH_tasks_G[0].Delay == 0)
		SCH_tasks_G[0].RunMe++;
}

void SCH_Delete_Tasks(uint32_t taskIDX)
{
	if (taskIDX < current_index_task)
	{
		for (int i = taskIDX; i < current_index_task - 1; i++)
		{
			SCH_tasks_G[i] = SCH_tasks_G[i + 1];
		}

		SCH_Reset_Task(current_index_task - 1);

		current_index_task--;
	}
}
