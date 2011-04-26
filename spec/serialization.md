
Serialization
=============

MidgardCR provides simple interface with methods to serialize and unserialize RepositoryObject.
By default, serialization and unserialization create string buffer from object and object from 
string buffer. It's implementation specific what is the format used for string buffer.

Storing serialized object (as string buffer) is not related to serialization, however implementation
can provide routines to store it in files or in memory. In any case, reading or writing string buffer 
is outside the scope of serialization process.

Basic object serialization shall depend on object properties, which can be reflected with GObject API.
In particular cases or for performance purposes, serializer class can be associated with storage manager, 
which provides access to object manager. 
