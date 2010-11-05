/*
 * $Id$
 */

#include "version.h"

#define __TO_STR(x)    #x
#define TO_STR(x)    __TO_STR(x)

/* Constants containing version and build information */
const int __attribute__((used)) c_mcivermaj = VER_MAJOR;
const int __attribute__((used)) c_mcivermin = VER_MINOR;
const int __attribute__((used)) c_mciverbuild = VER_BUILD;
const int __attribute__((used)) c_mciverrev = REVISION;
const char __attribute__((used)) c_mciverlabel[] = VER_LABEL;

const char __attribute__((used)) c_mciversionstr[] = "$Version: " TO_STR(VER_MAJOR) "." TO_STR(VER_MINOR) "-" TO_STR(VER_BUILD) "-" VER_LABEL "-r" TO_STR(REVISION) " $";
const char __attribute__((used)) c_svnrevision[] = "$SvnVersion: " TO_STR(REVISION) " $";
const char __attribute__((used)) c_buildcompiler[] = "$BuildCompilerVersion: " VER_COMPILERINFO " $";
const char __attribute__((used)) c_buildmachine[] = "$BuildMachineVersion: " VER_BUILDMACHINE " $";
const char __attribute__((used)) c_buildtime[] = "$BuildTimeVersion: " VER_BUILDTIME " $";
const char __attribute__((used)) c_builduser[] = "$BuildUserVersion: " VER_BUILDUSER " $";

/* Functions to access version info */

const char* verinfo(void) { return c_mciversionstr; }

int vermajor(void) { return c_mcivermaj; }
int verminor(void) { return c_mcivermin; }
int verbuild(void) { return c_mciverbuild; }
int verrevision(void) { return c_mciverrev; }

