# kvdb

A simple persistent on-disk CLI key-value store written in C.

The following commands are supported:

```
kvdb set {key} {value} : Associates `key` with `value`.
kvdb get {key}         : Fetches the value associated with `key`.
kvdb del {key}         : Removes `key` from the database.
kvdb ts {key}          : Returns the timestamp when `key` was first set and when it was last set.
```

## Installation and basic use

Simply run `make` from the project root dir. This creates the executable in `/bin/kvdb`.

The data directory is `/data` and it will contain a number of CSV files as data is written to the database.

Use `make dropdb` to clear the data dir.

Use `make test` to run a small collection of unit tests.

Example use:

```
➜  kvdb git:(main) ./bin/kvdb set abc def
➜  kvdb git:(main) ./bin/kvdb get abc    
the key 'abc' is associated with the value 'def'
➜  kvdb git:(main) ./bin/kvdb set abc 123
➜  kvdb git:(main) ./bin/kvdb get abc    
the key 'abc' is associated with the value '123'
➜  kvdb git:(main) ./bin/kvdb ts abc 
the key 'abc' was first set 2025-12-05 11:05:19.914 UTC and last set 2025-12-05 11:05:37.736 UTC
➜  kvdb git:(main) ./bin/kvdb del abc
```

After executing this sequence of commands the data directory contains the file `bucket_13.csv` with the contents:

```
EE718BED,abc,SET,def,1764932719,914,
EE718BED,abc,SET,123,1764932737,736,
EE718BED,abc,DEL,
```

The first two line corresponds to the set operations and have the format: key hash, key, operation, value, timestamp, milliseconds.
The last line is the delete operation (tombstone).

## Architecture

The key-value store works by maintaining a number of log files that act as buckets in a hash table.

When a key is inserted it is hashed to a bucket and appended to the log file associated with that bucket. 

Deletions are handled by appending tombstone markers to the log file.

To retrieve the value associated with a key or its timestamps we scan through the log file in the bucket for that key and collect the information.

When writing to a file we acquire an exclusive file lock `flock()`. For reads we use shared locks.

## Limitations

This is a hobby project with a large number of limitations and missing features compared to a production-ready key-value store.

If we focus on architectural limitations then the two main operations that are missing are compaction and re-hashing.
Currently we hash into a hard-coded number of buckets, and the complexity of get is linear in the number of prior sets and deletes that have happened in that bucket. Compaction and re-hashing would mitigate these issues and save space.

We do use LSM-style appends for set and delete which are supposed to be fast. Right now we lock the file when appending, but supposedly small appends are atomic, so we could investigate dropping locks.

Having one file per hash bucket comes with a significant storage overhead when we only have a few entries in each bucket since each file takes up a full block of storage.

Another unfortunate property of the system is that we read the log files in the insertion order. It would be better to read the log files from the back because then we could return as soon as we encounter a matching key.
