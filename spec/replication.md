
Replication
===========

Queues
------

Objects replication shall be implemented with queues. Both export and import queue shall provide functionality to store objects in queue, validate objects and perform particular action. There shall be no exception for amount of objects. Single object shall be also replicated with queue holding only one.

Replication queue shall be able to hold any kind of values, including object, serialized one or object's unique identifier. Queue shall provide access to serializer, when it holds serialized objects. In such case, queue is assumed to hold serialized objects in one format only. When format of serialized objects is unknown, particular serializer implementation shall be involved and queue shall hold objects only. For easier queue creation, every storage manager shall provide replication manager which shall be responsible to create at least import and export queues. However, there shall be no limitation how many queues are implemented and used by application. For example, application could implement separate export queues for updated and deleted content.

Replication information
-----------------------

There shall be no strict rule how information about object's import or export should be stored. However, repository object shall provide this information through metadata object, if such is valid for repository object.

Exporting content
-----------------

Exporting queue doesn't have to be associated with storage manager. It's up to the implementation if information about object's export is stored in repository.

Importing content
----------------- 

Importing queue shall be associated with one storage manager only. And, like exporting, it's implementation specific if to store information about object's import in repository. 
