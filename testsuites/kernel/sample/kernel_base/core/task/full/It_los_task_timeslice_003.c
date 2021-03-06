/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "It_los_task.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

static void TaskF03(void const * argument)
{
    UINT32 ret;

    // 2, assert that result is equal to this.
    ICUNIT_GOTO_EQUAL(g_timesliceTestCount, 2, g_timesliceTestCount, EXIT);
    g_timesliceTestCount++;

EXIT:;
}

static void TaskF02(void const * argument)
{
    UINT32 ret;
    UINT32 intSave;
    UINT64 itTimesliceCount11;

    ICUNIT_GOTO_EQUAL(g_timesliceTestCount, 1, g_timesliceTestCount, EXIT);
    g_timesliceTestCount++;

    intSave = LOS_IntLock();

    itTimesliceCount11 = TestTickCountGet();

    LOS_IntRestore(intSave);

    while (1) {
        dprintf("");
        intSave = LOS_IntLock();
        if ((itTimesliceCount11 + LOSCFG_BASE_CORE_TIMESLICE_TIMEOUT) < TestTickCountGet()) {
            LOS_IntRestore(intSave);
            break;
        }
        LOS_IntRestore(intSave);
    }
    // 4, assert that result is equal to this.
    ICUNIT_GOTO_EQUAL(g_timesliceTestCount, 4, g_timesliceTestCount, EXIT);
    g_timesliceTestCount++;

EXIT:;
}

static void TaskF01(void const * argument)
{
    UINT32 ret;
    UINT32 intSave;

    intSave = LOS_IntLock();

    ICUNIT_GOTO_EQUAL(g_timesliceTestCount, 0, g_timesliceTestCount, EXIT);
    g_timesliceTestCount++;

    g_itTimesliceTestCount1 = TestTickCountGet();

    LOS_IntRestore(intSave);

    while (1) {
        dprintf("");
        intSave = LOS_IntLock();
        if ((g_itTimesliceTestCount1 + LOSCFG_BASE_CORE_TIMESLICE_TIMEOUT) < TestTickCountGet()) {
            LOS_IntRestore(intSave);
            break;
        }
        LOS_IntRestore(intSave);
    }
    // 3, assert that result is equal to this.
    ICUNIT_GOTO_EQUAL(g_timesliceTestCount, 3, g_timesliceTestCount, EXIT);
    g_timesliceTestCount++;

EXIT:;
}

static UINT32 Testcase(void)
{
    UINT32 ret;

    g_itTimesliceTestCount1 = 0;
    g_timesliceTestCount = 0;

    LOS_TaskLock();

    TSK_INIT_PARAM_S task;

    task.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskF01;
    task.pcName = "TimesTsk003A";
    // 2, Used to calculate stack space
    task.uwStackSize = TASK_STACK_SIZE_TEST * 2;
    task.usTaskPrio = TASK_PRIO_TEST_TASK - 1;
    task.uwResved = LOS_TASK_STATUS_DETACHED;
#ifdef LOSCFG_KERNEL_SMP
    task.usCpuAffiMask = CPUID_TO_AFFI_MASK(ArchCurrCpuid());
#endif

    ret = LOS_TaskCreate(&g_testTaskID01, &task);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    task.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskF02;
    task.pcName = "TimesTsk003B";
    // 2, Used to calculate stack space
    task.uwStackSize = TASK_STACK_SIZE_TEST * 2;
    task.usTaskPrio = TASK_PRIO_TEST_TASK - 1;
    task.uwResved = LOS_TASK_STATUS_DETACHED;
#ifdef LOSCFG_KERNEL_SMP
    task.usCpuAffiMask = CPUID_TO_AFFI_MASK(ArchCurrCpuid());
#endif

    ret = LOS_TaskCreate(&g_testTaskID02, &task);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    task.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskF03;
    task.pcName = "TimesTsk003C";
    task.uwStackSize = TASK_STACK_SIZE_TEST;
    task.usTaskPrio = TASK_PRIO_TEST_TASK - 1;
    task.uwResved = LOS_TASK_STATUS_DETACHED;
#ifdef LOSCFG_KERNEL_SMP
    task.usCpuAffiMask = CPUID_TO_AFFI_MASK(ArchCurrCpuid());
#endif

    ret = LOS_TaskCreate(&g_testTaskID03, &task);
    ICUNIT_ASSERT_EQUAL(ret, LOS_OK, ret);

    LOS_TaskUnlock();

    LOS_TaskDelay(20); // 20, set delay time.
    // 5, assert that result is equal to this.
    ICUNIT_ASSERT_EQUAL(g_timesliceTestCount, 5, g_timesliceTestCount);
    LOS_TaskDelete(g_testTaskID03);
    LOS_TaskDelete(g_testTaskID02);
    LOS_TaskDelete(g_testTaskID01);
    return LOS_OK;
}

void ItLosTaskTimeslice003(void) // IT_Layer_ModuleORFeature_No
{
    TEST_ADD_CASE("ItLosTaskTIMESLICE_003", Testcase, TEST_LOS, TEST_TASK, TEST_LEVEL0, TEST_FUNCTION);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */
