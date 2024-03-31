#include "TextPrinterUE.h"

using namespace das;

mutex TextPrinterUE::pmut;

void TextPrinterUE::output()
{
    lock_guard<mutex> guard(pmut);
    int newPos = tellp();
    if (newPos != pos) {
        string st(data.data() + pos, newPos - pos);
        UE_LOG(LogTemp, Log, TEXT("%s"), ANSI_TO_TCHAR(st.c_str()));
        pos = newPos;
    }
}