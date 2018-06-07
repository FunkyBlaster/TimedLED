#ifndef MAIN_H_
#define MAIN_H_

void setCurStartTime(int fd, int hours, int min, int ampm);

void setCurEndTime(int fd, int hours, int min, int ampm);

void setTimeZone(int fd, char * tz, char * tzASCII);

#endif /* MAIN_H_ */
