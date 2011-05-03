
Repository Object
=================

Repository object is main form of data which holds data accessible on application level. It provides simple and uniform access to any content values available in repository. Repository object may be associated with storage manager (which is main entry point to repository), but also may act as volatile data holder. It's implementation specific if (and how) object provides reference to underlying storage manager. One important rule, is once object is associated with manager, it can not be changed during object's lifetime. The reference to storage manager can not be either dropped or changed.    
