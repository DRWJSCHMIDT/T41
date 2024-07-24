#ifndef G0ORX_CAT_H
#define G0ORX_CAT_H

extern int my_ptt;
extern bool catTX;

extern int CATOptions();
extern char *processCATCommand(char *buffer);
extern void CATSerialEvent();
extern int ChangeBand(long f, bool updateRelays);

#endif // G0ORX_CAT

