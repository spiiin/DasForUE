THIRD_PARTY_INCLUDES_START
#include "daScript/daScript.h"
THIRD_PARTY_INCLUDES_END

#pragma once

class TextPrinterUE : public das::TextWriter
{
public:
    virtual void output() override;
protected:
    int pos = 0;
    static das::mutex pmut;
};
