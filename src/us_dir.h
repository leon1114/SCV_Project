#ifndef _US_DIR_H
#define _US_DIR_H

#define CH14 14
#define HOME_PULSE 900
#define MAX_DIFF 500

void usdirInit(void);
void seeHome(void);
void seeLeft(int);
void seeRight(int);
void usdirTerm(void);

#endif
