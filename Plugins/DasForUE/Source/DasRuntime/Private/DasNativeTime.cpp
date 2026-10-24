// Native module ue_time: FDateTime and FTimespan for scripts, as the values UEDateTime and UETimespan (ticks) of
// ue_foundation. Built on the public native binds API (DasNativeBinds.h). Engine checks that would assert on bad
// input (invalid dates, out-of-range spans, division by zero) are script errors here.
#include "DasNativeBinds.h"

namespace DasNativeTime
{
// Ticks are added exactly (doubles lose the low ticks of a date); an overflow or a date out of range reports a
// script error. Add64 / Sub64 write Out: call them before passing Out on (argument order is unspecified).
bool Add64(int64 A, int64 B, int64& Out)
{
    if ((B > 0 && A > MAX_int64 - B) || (B < 0 && A < MIN_int64 - B))
        return false;
    Out = A + B;
    return true;
}
bool Sub64(int64 A, int64 B, int64& Out)
{
    if ((B < 0 && A > MAX_int64 + B) || (B > 0 && A < MIN_int64 + B))
        return false;
    Out = A - B;
    return true;
}
FDateTime CheckedDate(bool bValid, int64 Ticks)
{
    if (!bValid || Ticks < FDateTime::MinValue().GetTicks() || Ticks > FDateTime::MaxValue().GetTicks())
    {
        DasNativeError(TEXT("date is out of range"));
        return FDateTime();
    }
    return FDateTime(Ticks);
}
FTimespan CheckedSpan(bool bValid, int64 Ticks)
{
    if (!bValid)
    {
        DasNativeError(TEXT("time span is out of range"));
        return FTimespan();
    }
    return FTimespan(Ticks);
}
// Spans scaled or built from units in double, as the engine does; the result must fit int64.
FTimespan CheckedSpan(double Ticks)
{
    const bool bValid = FMath::IsFinite(Ticks) && FMath::Abs(Ticks) < 9.0e18;
    return CheckedSpan(bValid, bValid ? int64(FMath::RoundHalfFromZero(Ticks)) : 0);
}

// DateTime: construction.
FDateTime MakeDate7(int32 Year, int32 Month, int32 Day, int32 Hour, int32 Minute, int32 Second, int32 Millisecond)
{
    if (!FDateTime::Validate(Year, Month, Day, Hour, Minute, Second, Millisecond))
    {
        const FString Date = FString::Printf(TEXT("%d-%d-%d"), Year, Month, Day);
        const FString Time = FString::Printf(TEXT("%d:%d:%d.%d"), Hour, Minute, Second, Millisecond);
        DasNativeError(TEXT("invalid date ") + Date + TEXT(" ") + Time);
        return FDateTime();
    }
    return FDateTime(Year, Month, Day, Hour, Minute, Second, Millisecond);
}
FDateTime MakeDate6(int32 Year, int32 Month, int32 Day, int32 Hour, int32 Minute, int32 Second)
{
    return MakeDate7(Year, Month, Day, Hour, Minute, Second, 0);
}
FDateTime MakeDate3(int32 Year, int32 Month, int32 Day)
{
    return MakeDate7(Year, Month, Day, 0, 0, 0, 0);
}
FDateTime DateFromTicks(int64 Ticks)
{
    return CheckedDate(true, Ticks);
}
FDateTime FromUnixTimestamp(int64 Seconds)
{
    // Seconds beyond the date range would overflow the multiplication; dates span about 3e11 seconds.
    const bool bInRange = FMath::Abs(double(Seconds)) < 1.0e12;
    const int64 Offset = bInRange ? Seconds * ETimespan::TicksPerSecond : 0;
    int64 Ticks = 0;
    const bool bValid = bInRange && Add64(FDateTime(1970, 1, 1).GetTicks(), Offset, Ticks);
    return CheckedDate(bValid, Ticks);
}
int32 DaysInMonth(int32 Year, int32 Month)
{
    if (Month < 1 || Month > 12)
    {
        DasNativeError(FString::Printf(TEXT("month %d is not 1..12"), Month));
        return 0;
    }
    return FDateTime::DaysInMonth(Year, Month);
}
// Exported engine functions are wrapped: the address of an imported function is not a template argument.
FDateTime Now()
{
    return FDateTime::Now();
}
FDateTime UtcNow()
{
    return FDateTime::UtcNow();
}
int32 DaysInYear(int32 Year)
{
    return FDateTime::DaysInYear(Year);
}
bool IsLeapYear(int32 Year)
{
    return FDateTime::IsLeapYear(Year);
}

// DateTime: parts and text.
#define DAS_DATE_GETTER(Name, Getter)                                                                                  \
    constexpr auto Name = +[](const FDateTime& D)                                                                      \
    {                                                                                                                  \
        return D.Getter();                                                                                             \
    };
DAS_DATE_GETTER(GetYear, GetYear)
DAS_DATE_GETTER(GetMonth, GetMonth)
DAS_DATE_GETTER(GetDay, GetDay)
DAS_DATE_GETTER(GetHour, GetHour)
DAS_DATE_GETTER(GetHour12, GetHour12)
DAS_DATE_GETTER(GetMinute, GetMinute)
DAS_DATE_GETTER(GetSecond, GetSecond)
DAS_DATE_GETTER(GetMillisecond, GetMillisecond)
DAS_DATE_GETTER(GetDayOfYear, GetDayOfYear)
DAS_DATE_GETTER(GetDate, GetDate)
DAS_DATE_GETTER(GetTimeOfDay, GetTimeOfDay)
DAS_DATE_GETTER(IsMorning, IsMorning)
DAS_DATE_GETTER(IsAfternoon, IsAfternoon)
DAS_DATE_GETTER(GetDateTicks, GetTicks)
DAS_DATE_GETTER(ToUnixTimestamp, ToUnixTimestamp)
DAS_DATE_GETTER(ToIso8601, ToIso8601)
DAS_DATE_GETTER(ToHttpDate, ToHttpDate)
DAS_DATE_GETTER(DateToString, ToString)
#undef DAS_DATE_GETTER
FString DateToStringFormat(const FDateTime& D, const FString& Format)
{
    return D.ToString(*Format);
}

// Parsing: can_* answers, the others report a script error on bad text.
using FDateParser = bool (*)(const FString&, FDateTime&);
bool ParseIso8601(const FString& Text, FDateTime& Out)
{
    return FDateTime::ParseIso8601(*Text, Out);
}
bool ParseDateTime(const FString& Text, FDateTime& Out)
{
    return FDateTime::Parse(Text, Out);
}
bool ParseHttpDate(const FString& Text, FDateTime& Out)
{
    return FDateTime::ParseHttpDate(Text, Out);
}
template <FDateParser Parser> bool CanParse(const FString& Text)
{
    FDateTime Unused;
    return Parser(Text, Unused);
}
template <FDateParser Parser> FDateTime Parse(const FString& Text)
{
    FDateTime Result;
    if (!Parser(Text, Result))
        DasNativeError(FString::Printf(TEXT("cannot parse '%s'"), *Text));
    return Result;
}

// DateTime: operators.
FDateTime DateAddSpan(const FDateTime& D, const FTimespan& S)
{
    int64 Ticks = 0;
    const bool bValid = Add64(D.GetTicks(), S.GetTicks(), Ticks);
    return CheckedDate(bValid, Ticks);
}
FDateTime DateSubSpan(const FDateTime& D, const FTimespan& S)
{
    int64 Ticks = 0;
    const bool bValid = Sub64(D.GetTicks(), S.GetTicks(), Ticks);
    return CheckedDate(bValid, Ticks);
}
FTimespan DateSubDate(const FDateTime& A, const FDateTime& B)
{
    return A - B;
}
#define DAS_TIME_COMPARE(Name, Type, Op)                                                                               \
    constexpr auto Name = +[](const Type& A, const Type& B)                                                            \
    {                                                                                                                  \
        return A Op B;                                                                                                 \
    };
DAS_TIME_COMPARE(DateEqual, FDateTime, ==)
DAS_TIME_COMPARE(DateNotEqual, FDateTime, !=)
DAS_TIME_COMPARE(DateLess, FDateTime, <)
DAS_TIME_COMPARE(DateLessEqual, FDateTime, <=)
DAS_TIME_COMPARE(DateGreater, FDateTime, >)
DAS_TIME_COMPARE(DateGreaterEqual, FDateTime, >=)
DAS_TIME_COMPARE(SpanEqual, FTimespan, ==)
DAS_TIME_COMPARE(SpanNotEqual, FTimespan, !=)
DAS_TIME_COMPARE(SpanLess, FTimespan, <)
DAS_TIME_COMPARE(SpanLessEqual, FTimespan, <=)
DAS_TIME_COMPARE(SpanGreater, FTimespan, >)
DAS_TIME_COMPARE(SpanGreaterEqual, FTimespan, >=)
#undef DAS_TIME_COMPARE

// Timespan: construction.
FTimespan MakeSpan5(int32 Days, int32 Hours, int32 Minutes, int32 Seconds, int32 FractionNano)
{
    // Checked in double with a margin, then computed exactly.
    const double Total = ((double(Days) * 24 + Hours) * 60 + Minutes) * 60 + Seconds;
    const bool bValid = FMath::Abs(Total * ETimespan::TicksPerSecond) < 9.0e18;
    const int64 Whole = ((int64(Days) * 24 + Hours) * 60 + Minutes) * 60 + Seconds;
    const int64 Fraction = FractionNano / ETimespan::NanosecondsPerTick;
    return CheckedSpan(bValid, bValid ? Whole * ETimespan::TicksPerSecond + Fraction : 0);
}
FTimespan MakeSpan4(int32 Days, int32 Hours, int32 Minutes, int32 Seconds)
{
    return MakeSpan5(Days, Hours, Minutes, Seconds, 0);
}
FTimespan MakeSpan3(int32 Hours, int32 Minutes, int32 Seconds)
{
    return MakeSpan5(0, Hours, Minutes, Seconds, 0);
}
FTimespan SpanFromTicks(int64 Ticks)
{
    return FTimespan(Ticks);
}
template <int64 TicksPerUnit> FTimespan SpanFrom(double Units)
{
    return CheckedSpan(Units * double(TicksPerUnit));
}

// Timespan: parts.
#define DAS_SPAN_GETTER(Name, Getter)                                                                                  \
    constexpr auto Name = +[](const FTimespan& S)                                                                      \
    {                                                                                                                  \
        return S.Getter();                                                                                             \
    };
DAS_SPAN_GETTER(GetDays, GetDays)
DAS_SPAN_GETTER(GetHours, GetHours)
DAS_SPAN_GETTER(GetMinutes, GetMinutes)
DAS_SPAN_GETTER(GetSeconds, GetSeconds)
DAS_SPAN_GETTER(GetFractionMilli, GetFractionMilli)
DAS_SPAN_GETTER(GetFractionMicro, GetFractionMicro)
DAS_SPAN_GETTER(GetFractionNano, GetFractionNano)
DAS_SPAN_GETTER(GetFractionTicks, GetFractionTicks)
DAS_SPAN_GETTER(GetTotalDays, GetTotalDays)
DAS_SPAN_GETTER(GetTotalHours, GetTotalHours)
DAS_SPAN_GETTER(GetTotalMinutes, GetTotalMinutes)
DAS_SPAN_GETTER(GetTotalSeconds, GetTotalSeconds)
DAS_SPAN_GETTER(GetTotalMilliseconds, GetTotalMilliseconds)
DAS_SPAN_GETTER(GetTotalMicroseconds, GetTotalMicroseconds)
DAS_SPAN_GETTER(GetSpanTicks, GetTicks)
DAS_SPAN_GETTER(IsZero, IsZero)
DAS_SPAN_GETTER(SpanToString, ToString)
#undef DAS_SPAN_GETTER
FString SpanToStringFormat(const FTimespan& S, const FString& Format)
{
    return S.ToString(*Format);
}
FTimespan GetDuration(const FTimespan& S)
{
    const int64 Ticks = S.GetTicks();
    return CheckedSpan(Ticks != MIN_int64, Ticks == MIN_int64 ? 0 : (Ticks < 0 ? -Ticks : Ticks));
}

// Timespan: operators.
FTimespan SpanAdd(const FTimespan& A, const FTimespan& B)
{
    int64 Ticks = 0;
    const bool bValid = Add64(A.GetTicks(), B.GetTicks(), Ticks);
    return CheckedSpan(bValid, Ticks);
}
FTimespan SpanSub(const FTimespan& A, const FTimespan& B)
{
    int64 Ticks = 0;
    const bool bValid = Sub64(A.GetTicks(), B.GetTicks(), Ticks);
    return CheckedSpan(bValid, Ticks);
}
FTimespan SpanNegate(const FTimespan& A)
{
    const int64 Ticks = A.GetTicks();
    return CheckedSpan(Ticks != MIN_int64, Ticks == MIN_int64 ? 0 : -Ticks);
}
FTimespan SpanMul(const FTimespan& A, double Scalar)
{
    return CheckedSpan(double(A.GetTicks()) * Scalar);
}
FTimespan ScalarMulSpan(double Scalar, const FTimespan& A)
{
    return SpanMul(A, Scalar);
}
FTimespan SpanDiv(const FTimespan& A, double Scalar)
{
    if (Scalar == 0)
    {
        DasNativeError(TEXT("division by zero"));
        return FTimespan();
    }
    return CheckedSpan(double(A.GetTicks()) / Scalar);
}
FTimespan SpanMod(const FTimespan& A, const FTimespan& B)
{
    if (B.IsZero())
    {
        DasNativeError(TEXT("division by zero"));
        return FTimespan();
    }
    // MIN_int64 % -1 traps in hardware; the remainder of a division by -1 is 0.
    return FTimespan(B.GetTicks() == -1 ? 0 : A.GetTicks() % B.GetTicks());
}
// Compound assignments (references, DasNativeBinds.h): the checked operators above; a failed one leaves the
// script's variable unchanged.
void DateAddAssign(FDateTime& D, const FTimespan& S)
{
    D = DateAddSpan(D, S);
}
void DateSubAssign(FDateTime& D, const FTimespan& S)
{
    D = DateSubSpan(D, S);
}
void SpanAddAssign(FTimespan& A, const FTimespan& B)
{
    A = SpanAdd(A, B);
}
void SpanSubAssign(FTimespan& A, const FTimespan& B)
{
    A = SpanSub(A, B);
}
void SpanMulAssign(FTimespan& A, double Scalar)
{
    A = SpanMul(A, Scalar);
}
void SpanDivAssign(FTimespan& A, double Scalar)
{
    A = SpanDiv(A, Scalar);
}
void SpanModAssign(FTimespan& A, const FTimespan& B)
{
    A = SpanMod(A, B);
}
double Ratio(const FTimespan& Dividend, const FTimespan& Divisor)
{
    if (Divisor.IsZero())
    {
        DasNativeError(TEXT("division by zero"));
        return 0;
    }
    return FTimespan::Ratio(Dividend, Divisor);
}
}

DAS_NATIVE_BINDS(UETime, "ue_time")
{
    using namespace DasNativeTime;
    // DateTime
    Binds.Function<&MakeDate3>(TEXT("make_date_time"));
    Binds.Function<&MakeDate6>(TEXT("make_date_time"));
    Binds.Function<&MakeDate7>(TEXT("make_date_time"));
    Binds.Function<&DateFromTicks>(TEXT("date_time_from_ticks"));
    Binds.Function<&FromUnixTimestamp>(TEXT("from_unix_timestamp"));
    Binds.Function<&Now>(TEXT("now"));
    Binds.Function<&UtcNow>(TEXT("utc_now"));
    Binds.Function<&FDateTime::Today>(TEXT("today"));
    Binds.Function<&FDateTime::MinValue>(TEXT("min_date_time"));
    Binds.Function<&FDateTime::MaxValue>(TEXT("max_date_time"));
    Binds.Function<&DaysInMonth>(TEXT("days_in_month"));
    Binds.Function<&DaysInYear>(TEXT("days_in_year"));
    Binds.Function<&IsLeapYear>(TEXT("is_leap_year"));
    Binds.Function<GetYear>(TEXT("get_year"));
    Binds.Function<GetMonth>(TEXT("get_month"));
    Binds.Function<GetDay>(TEXT("get_day"));
    Binds.Function<GetHour>(TEXT("get_hour"));
    Binds.Function<GetHour12>(TEXT("get_hour12"));
    Binds.Function<GetMinute>(TEXT("get_minute"));
    Binds.Function<GetSecond>(TEXT("get_second"));
    Binds.Function<GetMillisecond>(TEXT("get_millisecond"));
    Binds.Function<GetDayOfYear>(TEXT("get_day_of_year"));
    Binds.Function<GetDate>(TEXT("get_date"));
    Binds.Function<GetTimeOfDay>(TEXT("get_time_of_day"));
    Binds.Function<IsMorning>(TEXT("is_morning"));
    Binds.Function<IsAfternoon>(TEXT("is_afternoon"));
    Binds.Function<GetDateTicks>(TEXT("get_ticks"));
    Binds.Function<ToUnixTimestamp>(TEXT("to_unix_timestamp"));
    Binds.Function<ToIso8601>(TEXT("to_iso8601"));
    Binds.Function<ToHttpDate>(TEXT("to_http_date"));
    Binds.Function<DateToString>(TEXT("to_string"));
    Binds.Function<&DateToStringFormat>(TEXT("to_string"));
    Binds.Function<&CanParse<&ParseIso8601>>(TEXT("can_parse_iso8601"));
    Binds.Function<&Parse<&ParseIso8601>>(TEXT("parse_iso8601"));
    Binds.Function<&CanParse<&ParseDateTime>>(TEXT("can_parse_date_time"));
    Binds.Function<&Parse<&ParseDateTime>>(TEXT("parse_date_time"));
    Binds.Function<&CanParse<&ParseHttpDate>>(TEXT("can_parse_http_date"));
    Binds.Function<&Parse<&ParseHttpDate>>(TEXT("parse_http_date"));
    Binds.Function<&DateAddSpan>(TEXT("+"));
    Binds.Function<&DateSubSpan>(TEXT("-"));
    Binds.Function<&DateSubDate>(TEXT("-"));
    Binds.Function<&DateAddAssign>(TEXT("+="));
    Binds.Function<&DateSubAssign>(TEXT("-="));
    Binds.Function<DateEqual>(TEXT("=="));
    Binds.Function<DateNotEqual>(TEXT("!="));
    Binds.Function<DateLess>(TEXT("<"));
    Binds.Function<DateLessEqual>(TEXT("<="));
    Binds.Function<DateGreater>(TEXT(">"));
    Binds.Function<DateGreaterEqual>(TEXT(">="));

    // Timespan
    Binds.Function<&MakeSpan3>(TEXT("make_timespan"));
    Binds.Function<&MakeSpan4>(TEXT("make_timespan"));
    Binds.Function<&MakeSpan5>(TEXT("make_timespan"));
    Binds.Function<&SpanFromTicks>(TEXT("timespan_from_ticks"));
    Binds.Function<&SpanFrom<ETimespan::TicksPerDay>>(TEXT("timespan_from_days"));
    Binds.Function<&SpanFrom<ETimespan::TicksPerHour>>(TEXT("timespan_from_hours"));
    Binds.Function<&SpanFrom<ETimespan::TicksPerMinute>>(TEXT("timespan_from_minutes"));
    Binds.Function<&SpanFrom<ETimespan::TicksPerSecond>>(TEXT("timespan_from_seconds"));
    Binds.Function<&SpanFrom<ETimespan::TicksPerMillisecond>>(TEXT("timespan_from_milliseconds"));
    Binds.Function<&SpanFrom<ETimespan::TicksPerMicrosecond>>(TEXT("timespan_from_microseconds"));
    Binds.Function<&FTimespan::Zero>(TEXT("zero_timespan"));
    Binds.Function<&FTimespan::MinValue>(TEXT("min_timespan"));
    Binds.Function<&FTimespan::MaxValue>(TEXT("max_timespan"));
    Binds.Function<GetDays>(TEXT("get_days"));
    Binds.Function<GetHours>(TEXT("get_hours"));
    Binds.Function<GetMinutes>(TEXT("get_minutes"));
    Binds.Function<GetSeconds>(TEXT("get_seconds"));
    Binds.Function<GetFractionMilli>(TEXT("get_fraction_milli"));
    Binds.Function<GetFractionMicro>(TEXT("get_fraction_micro"));
    Binds.Function<GetFractionNano>(TEXT("get_fraction_nano"));
    Binds.Function<GetFractionTicks>(TEXT("get_fraction_ticks"));
    Binds.Function<GetTotalDays>(TEXT("get_total_days"));
    Binds.Function<GetTotalHours>(TEXT("get_total_hours"));
    Binds.Function<GetTotalMinutes>(TEXT("get_total_minutes"));
    Binds.Function<GetTotalSeconds>(TEXT("get_total_seconds"));
    Binds.Function<GetTotalMilliseconds>(TEXT("get_total_milliseconds"));
    Binds.Function<GetTotalMicroseconds>(TEXT("get_total_microseconds"));
    Binds.Function<GetSpanTicks>(TEXT("get_ticks"));
    Binds.Function<&GetDuration>(TEXT("get_duration"));
    Binds.Function<IsZero>(TEXT("is_zero"));
    Binds.Function<SpanToString>(TEXT("to_string"));
    Binds.Function<&SpanToStringFormat>(TEXT("to_string"));
    Binds.Function<&Ratio>(TEXT("ratio"));
    Binds.Function<&SpanAdd>(TEXT("+"));
    Binds.Function<&SpanSub>(TEXT("-"));
    Binds.Function<&SpanNegate>(TEXT("-"));
    Binds.Function<&SpanMul>(TEXT("*"));
    Binds.Function<&ScalarMulSpan>(TEXT("*"));
    Binds.Function<&SpanDiv>(TEXT("/"));
    Binds.Function<&SpanMod>(TEXT("%"));
    Binds.Function<&SpanAddAssign>(TEXT("+="));
    Binds.Function<&SpanSubAssign>(TEXT("-="));
    Binds.Function<&SpanMulAssign>(TEXT("*="));
    Binds.Function<&SpanDivAssign>(TEXT("/="));
    Binds.Function<&SpanModAssign>(TEXT("%="));
    Binds.Function<SpanEqual>(TEXT("=="));
    Binds.Function<SpanNotEqual>(TEXT("!="));
    Binds.Function<SpanLess>(TEXT("<"));
    Binds.Function<SpanLessEqual>(TEXT("<="));
    Binds.Function<SpanGreater>(TEXT(">"));
    Binds.Function<SpanGreaterEqual>(TEXT(">="));
}
