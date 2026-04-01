namespace HelpersTime
{

  inline unsigned long long int GetSecondsFromMilliseconds(unsigned long long int value)
  {
    return value / 1000;
  }

  inline double GetMinutesFromMilliseconds(unsigned long long int value)
  {
    return (double)GetSecondsFromMilliseconds(value) / 60.0;
  }

  inline double GetHoursFromMilliseconds(unsigned long long int value)
  {
    return (double)GetSecondsFromMilliseconds(value) / 3600.0;
  }

}