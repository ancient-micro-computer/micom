#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <mutex>
#include "typedef.h"
#include "simulator.h"
#include "macro.h"
#include "DisplayModule.h"
#include "Bus.h"

using namespace Simulator;

DisplayModule::DisplayModule() : Block()
{
}

DisplayModule::~DisplayModule()
{
}

TINT DisplayModule::Exec()
{
    TW32U r0, r1;

    GetReg(0, r0);
    printf("r0:%lx\n", r0);

    GetReg(1, r1);
    printf("r1:%lx\n", r1);

    // Read a value from $0200
    TW32U v, ignore;
    GetMem(ADDRESS_DISPLAY, v, ignore);
    printf("$%lx:%lx\n", ADDRESS_DISPLAY, v);

    return D_ERR_OK;
}

TINT   DisplayModule::GetReg(TINT addr, TW32U &value)
{
    m_ParentBus->get_reg(D_MODULEID_CPU, addr, value);
    return D_ERR_OK;
}

TINT    DisplayModule::SetReg(TINT addr, TW32U value)
{
    // Do nothing
    return D_ERR_OK;
}

TINT   DisplayModule::GetMem(TW32U addr, TW32U &value, TW32U &valid)
{
    m_ParentBus->set_address(addr);
    m_ParentBus->access_read();
    value = m_ParentBus->get_data();

    return D_ERR_OK;
}

TINT    DisplayModule::SetMem(TW32U addr, TW32U value)
{
    // Do nothing
	return D_ERR_OK;
}

TW32U*	DisplayModule::MemPtr(TW32U addr)
{
    // Do nothing
	return NULL;
}
