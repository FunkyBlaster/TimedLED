#ifndef MAIN_H_
#define MAIN_H_

char* getCurSysTimeASCII(int fd);

char* getCurStartTimeASCII(int fd);

char* getCurEndTimeASCII(int fd);

char* getTimeZoneASCII(int fd);

void setCurStartTime(int fd, int hours, int min, int ampm);

void setCurEndTime(int fd, int hours, int min, int ampm);

void setTimeZone(int fd, char * tz, char * tzASCII);

char* SerializeClockData(int fd);

void RegisterPost();

BOOL SyncSystemTimeNTP();

int timeObjEval(struct tm * one, struct tm * two);

#endif /* MAIN_H_ */
