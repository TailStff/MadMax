#include "mmMiniScheduler.h"

mmMiniScheduler::mmMiniScheduler(ExecutionEnv *_executionEnv)
{
    executionEnv = _executionEnv;
}

mmMiniScheduler::~mmMiniScheduler()
{
}

bool mmMiniScheduler::Evaluate(DateTimeDefinition &dateTime)
{
    // Flag that indicate if we reach our target DateTime
    bool match = false;

    // Get current DateTime through executionEnv
    DateTime current = executionEnv->getDateTime();

    // Exemple : year/month/day/hour/minute/second peuvent être -1
    // Crée DateTime cible avec wildcards remplacées par current
    DateTime targetDate(
        dateTime.Year == ANY ? executionEnv->getYear() : dateTime.Year,
        dateTime.Month == ANY ? executionEnv->getMonth() : dateTime.Month,
        dateTime.Day == ANY ? executionEnv->getDay() : dateTime.Day,
        dateTime.Hour == ANY ? executionEnv->getHour() : dateTime.Hour,
        dateTime.Minute == ANY ? executionEnv->getMinute() : dateTime.Minute,
        dateTime.Second == ANY ? 0 : dateTime.Second);

    // DayOfWeek (0 = Dimanche, 1 = Lundi, ... 6 = Samedi)
    bool dayMatch = (dateTime.DayOfWeek == ANY) || (dateTime.DayOfWeek == current.dayOfTheWeek());

    if (targetDate > lastExecution && targetDate <= current && dayMatch)
        match = true;

    bool risingEdge = match && !memMatch;
    memMatch = match;

    lastExecution = current;

    return risingEdge;
}