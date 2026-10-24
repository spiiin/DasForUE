options gen2
require ue_time

// The plugin's native module ue_time (DasNativeTime.cpp). main returns 0 when every check passes, otherwise the
// number of the first failed check.
[export]
def main(input:int):int {
    let d = make_date_time(2024, 2, 29, 13, 45, 30, 250)
    if (d.get_year() != 2024 || d.get_month() != 2 || d.get_day() != 29) { return 1 }
    if (d.get_hour() != 13 || d.get_hour12() != 1 || d.get_minute() != 45 || d.get_second() != 30) { return 2 }
    if (d.get_millisecond() != 250 || d.get_day_of_year() != 60 || !d.is_afternoon() || d.is_morning()) { return 3 }
    // Text.
    if (d.to_iso8601() != "2024-02-29T13:45:30.250Z" || d.to_string("%Y-%m-%d") != "2024-02-29") { return 4 }
    if (d.to_http_date() != "Thu, 29 Feb 2024 13:45:30 GMT" || d.to_string() == "") { return 5 }
    // Arithmetic and comparison across a leap day and a year.
    let next = d + timespan_from_days(1.0lf)
    if (next.get_month() != 3 || next.get_day() != 1 || next - timespan_from_days(1.0lf) != d) { return 6 }
    let span = next - d
    if (span != make_timespan(1, 0, 0, 0) || span.get_total_hours() != 24.0lf) { return 7 }
    if (!(d < next) || d >= next || d == next || !(next > d) || !(d <= d) || !(d >= d) || d != d) { return 8 }
    let new_year = make_date_time(2023, 12, 31, 23, 59, 59) + make_timespan(0, 0, 1)
    if (new_year.to_iso8601() != "2024-01-01T00:00:00.000Z") { return 9 }
    // Calendar.
    if (!is_leap_year(2024) || is_leap_year(2023) || !is_leap_year(2000) || is_leap_year(1900)) { return 10 }
    if (days_in_month(2024, 2) != 29 || days_in_month(2023, 2) != 28 || days_in_year(2023) != 365) { return 11 }
    // Unix time and ticks (int64).
    if (from_unix_timestamp(0l) != make_date_time(1970, 1, 1)) { return 12 }
    if (make_date_time(2024, 2, 29).to_unix_timestamp() != 1709164800l) { return 13 }
    if (date_time_from_ticks(d.get_ticks()) != d || d.get_ticks() <= 0l) { return 14 }
    // Parsing.
    if (!can_parse_iso8601("2024-02-29T13:45:30.250Z") || parse_iso8601("2024-02-29T13:45:30.250Z") != d) {
        return 15
    }
    if (can_parse_iso8601("tomorrow")) { return 16 }
    if (parse_http_date("Thu, 29 Feb 2024 13:45:30 GMT") != make_date_time(2024, 2, 29, 13, 45, 30)) { return 17 }
    // Date and time of day.
    if (d.get_date() != make_date_time(2024, 2, 29)) { return 18 }
    if (d.get_time_of_day() != make_timespan(0, 13, 45, 30, 250000000)) { return 19 }
    // Timespan parts and operators.
    let t = make_timespan(1, 2, 3, 4)
    if (t.get_days() != 1 || t.get_hours() != 2 || t.get_minutes() != 3 || t.get_seconds() != 4) { return 20 }
    if (t.get_total_seconds() != 93784.0lf || t.get_ticks() != timespan_from_seconds(93784.0lf).get_ticks()) {
        return 21
    }
    if (-t != zero_timespan() - t || (-t).get_duration() != t || t + t != t * 2.0lf || 2.0lf * t != t * 2.0lf) {
        return 22
    }
    if ((t / 2.0lf).get_total_seconds() != 46892.0lf || t % make_timespan(1, 0, 0) != make_timespan(0, 3, 4)) {
        return 23
    }
    let ms = timespan_from_milliseconds(1500.0lf)
    if (ms.get_seconds() != 1 || ms.get_fraction_milli() != 500 || ratio(t, t * 2.0lf) != 0.5lf) { return 24 }
    if (!zero_timespan().is_zero() || t.is_zero() || !(min_timespan() < max_timespan()) || t.to_string() == "") {
        return 25
    }
    if (!(min_date_time() < max_date_time()) || !(t > ms) || !(ms <= t) || t == ms) { return 26 }
    // The clock: local time is within a day of UTC; today starts at midnight.
    let utc = utc_now()
    if (utc.get_year() < 2026 || (now() - utc).get_duration() > timespan_from_hours(15.0lf)) { return 27 }
    if (today().get_hour() != 0 || today().get_minute() != 0 || today() > now()) { return 28 }
    // Compound assignments change the variable; a failed one (division by zero) leaves it as it was.
    var total = make_timespan(1, 0, 0)
    total += make_timespan(0, 30, 0)
    total -= make_timespan(0, 10, 0)
    if (total != make_timespan(1, 20, 0)) { return 29 }
    total *= 3.0lf
    total /= 2.0lf
    if (total != make_timespan(2, 0, 0)) { return 30 }
    total %= make_timespan(0, 50, 0)
    if (total != make_timespan(0, 20, 0)) { return 31 }
    var failed = false
    try { total /= 0.0lf } recover { failed = true }
    if (!failed || total != make_timespan(0, 20, 0)) { return 32 }
    var day = make_date_time(2024, 2, 28)
    day += timespan_from_days(1.0lf)
    if (day.get_day() != 29) { return 33 }
    day -= timespan_from_days(2.0lf)
    if (day.get_day() != 27) { return 34 }
    return 0
}

