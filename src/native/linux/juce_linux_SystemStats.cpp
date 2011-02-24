/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-10 by Raw Material Software Ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the GNU General
   Public License (Version 2), as published by the Free Software Foundation.
   A copy of the license is included in the JUCE distribution, or can be found
   online at www.gnu.org/licenses.

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.rawmaterialsoftware.com/juce for more information.

  ==============================================================================
*/

// (This file gets included by juce_linux_NativeCode.cpp, rather than being
// compiled on its own).
#if JUCE_INCLUDED_FILE


//==============================================================================
void Logger::outputDebugString (const String& text)
{
    std::cerr << text << std::endl;
}

//==============================================================================
SystemStats::OperatingSystemType SystemStats::getOperatingSystemType()
{
    return Linux;
}

const String SystemStats::getOperatingSystemName()
{
    return "Linux";
}

bool SystemStats::isOperatingSystem64Bit()
{
#if JUCE_64BIT
    return true;
#else
    //xxx not sure how to find this out?..
    return false;
#endif
}

//==============================================================================
namespace LinuxStatsHelpers
{
    const String getCpuInfo (const char* const key)
    {
        StringArray lines;
        lines.addLines (File ("/proc/cpuinfo").loadFileAsString());

        for (int i = lines.size(); --i >= 0;) // (NB - it's important that this runs in reverse order)
            if (lines[i].startsWithIgnoreCase (key))
                return lines[i].fromFirstOccurrenceOf (":", false, false).trim();

        return String::empty;
    }
}

const String SystemStats::getCpuVendor()
{
    return LinuxStatsHelpers::getCpuInfo ("vendor_id");
}

int SystemStats::getCpuSpeedInMegaherz()
{
    return roundToInt (LinuxStatsHelpers::getCpuInfo ("cpu MHz").getFloatValue());
}

int SystemStats::getMemorySizeInMegabytes()
{
    struct sysinfo sysi;

    if (sysinfo (&sysi) == 0)
        return (sysi.totalram * sysi.mem_unit / (1024 * 1024));

    return 0;
}

int SystemStats::getPageSize()
{
    return sysconf (_SC_PAGESIZE);
}

//==============================================================================
const String SystemStats::getLogonName()
{
    const char* user = getenv ("USER");

    if (user == 0)
    {
        struct passwd* const pw = getpwuid (getuid());
        if (pw != 0)
            user = pw->pw_name;
    }

    return CharPointer_UTF8 (user);
}

const String SystemStats::getFullUserName()
{
    return getLogonName();
}

//==============================================================================
void SystemStats::initialiseStats()
{
    const String flags (LinuxStatsHelpers::getCpuInfo ("flags"));
    cpuFlags.hasMMX = flags.contains ("mmx");
    cpuFlags.hasSSE = flags.contains ("sse");
    cpuFlags.hasSSE2 = flags.contains ("sse2");
    cpuFlags.has3DNow = flags.contains ("3dnow");

    cpuFlags.numCpus = LinuxStatsHelpers::getCpuInfo ("processor").getIntValue() + 1;
}

void PlatformUtilities::fpuReset()
{
}

//==============================================================================
uint32 juce_millisecondsSinceStartup() throw()
{
    timespec t;
    clock_gettime (CLOCK_MONOTONIC, &t);

    return t.tv_sec * 1000 + t.tv_nsec / 1000000;
}

int64 Time::getHighResolutionTicks() throw()
{
    timespec t;
    clock_gettime (CLOCK_MONOTONIC, &t);

    return (t.tv_sec * (int64) 1000000) + (t.tv_nsec / (int64) 1000);
}

int64 Time::getHighResolutionTicksPerSecond() throw()
{
    return 1000000;  // (microseconds)
}

double Time::getMillisecondCounterHiRes() throw()
{
    return getHighResolutionTicks() * 0.001;
}

bool Time::setSystemTimeToThisTime() const
{
    timeval t;
    t.tv_sec = millisSinceEpoch / 1000;
    t.tv_usec = (millisSinceEpoch - t.tv_sec * 1000) * 1000;

    return settimeofday (&t, 0) == 0;
}


#endif
