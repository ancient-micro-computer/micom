#ifndef __C_MODULE_DisplayModule_H__
#define __C_MODULE_DisplayModule_H__

#include "typedef.h"
#include "Block.h"

namespace Simulator {

#define ADDRESS_DISPLAY 0x200

class DisplayModule : public Block
{
public:
	DisplayModule();
	~DisplayModule();

    TINT    Exec();     // function for Block execution
    TINT    GetReg(TINT addr, TW32U &value);						// Register get/set
    TINT    SetReg(TINT addr, TW32U value);
    TINT    GetMem(TW32U addr, TW32U &value, TW32U &valid);// Memory get/set
    TINT    SetMem(TW32U addr, TW32U value);
    TW32U*  MemPtr(TW32U addr);

private:
};

};

#endif	// __C_MODULE_DisplayModule_H__
