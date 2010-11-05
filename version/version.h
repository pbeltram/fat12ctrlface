#ifndef VERSION_H
#define VERSION_H

#ifdef __cplusplus
extern "C" {
#endif

const char* verinfo(void);
int vermajor(void);
int verminor(void);
int verbuild(void);
int verrevision(void);

#ifdef __cplusplus
}
#endif


#endif  /* VERSION_H */
