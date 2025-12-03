#ifndef KVDB_CMD_H
#define KVDB_CMD_H

bool set(const char* key, const char* value);
void get(const char* key);
void del(const char* key);
void ts(const char* key);

#endif  // KVDB_CMD_H