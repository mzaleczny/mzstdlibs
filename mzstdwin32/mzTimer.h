#pragma once

#include "mzstdwin32Common.h"
#include "mzObject.h"
#include <map>
#include <string>

class  CTimer : public mzstd::CmzObject
{
public:
    class Callback
    {
    public:
        virtual void OnCallbackTimerProc(DWORD dwTime) = 0;
    };

    template<class T>
    class SpecialisedCallback : public Callback
    {
        T* Entity;
        typedef void(T::* fnTimer)(DWORD dwTime);
        fnTimer TimerProc;
    public:
        SpecialisedCallback(T* _Entity, fnTimer _TimerProc) : Entity(_Entity), TimerProc(_TimerProc) {}
        virtual void OnCallbackTimerProc(DWORD dwTime) override
        {
            (Entity->*TimerProc)(dwTime);
        }
    };

    static DECLSPEC_MZSTDWIN32 UINT_PTR AddTimer(Callback* timerObj, DWORD interval)
    {
        UINT_PTR id = SetTimer(NULL, 0, interval, TimerProc);
        // add the timer to the map using the id as the key
        Timers[id] = timerObj;
        return id;
    }

    static DECLSPEC_MZSTDWIN32 void KillTimer(UINT_PTR TimerID)
    {
        std::map<UINT_PTR, Callback*>::const_iterator it = Timers.find(TimerID);
        if (it != Timers.end())
        {
            ::KillTimer(NULL, TimerID);
            delete Timers[TimerID];
            Timers[TimerID] = nullptr;
            Timers.erase(it);
        }
    }

    static void CALLBACK TimerProc(HWND hwnd, UINT msg, UINT_PTR timerId, DWORD dwTime)
    {
        Timers[timerId]->OnCallbackTimerProc(dwTime);
    }
    
    static DECLSPEC_MZSTDWIN32 void CleanupTimers()
    {
        std::wstring s;
        for (std::map<UINT_PTR, Callback*>::const_iterator it = Timers.begin(); it != Timers.end(); ++it)
        {
            UINT_PTR TimerID = it->first;
            ::KillTimer(NULL, TimerID);
            delete Timers[TimerID];
            Timers[TimerID] = nullptr;
            s += L"Clenaup Timer: " + std::to_wstring(TimerID) + L"\n";
        }
        if (s.length() > 0)
        {
            MessageBox(0, s.c_str(), L"Info", MB_ICONINFORMATION | MB_OK);
        }
        Timers.clear();
    }

private:
    static std::map<UINT_PTR, CTimer::Callback *> Timers;
};
