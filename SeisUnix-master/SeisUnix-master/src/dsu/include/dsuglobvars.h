/*
        DSU (AEM) Global definitions needed in applibcations only
*/

#ifdef DO_GLOBALS
int  (*DsuGet)(FILE *, segy *) = (int  (*) (FILE *, segy *)) fgettr;
int  (*DsuVGet)(FILE *, segy *) = (int  (*) (FILE *, segy *)) fvgettr;
void (*DsuPut)(FILE *, segy *) = (void (*) (FILE *, segy *)) fputtr;

DsuTask  ThisDsuTask;
#endif
