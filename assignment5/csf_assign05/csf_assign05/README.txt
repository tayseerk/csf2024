**CONTRIBUTIONS**

*MS1* (forgot to include this in MS1 submission)

Tayseer Karrossi: Item 2 in the suggested approach
Emily Zou: Item 1 in the suggested approach

*MS2*

Tayseer Karrossi: Item 3 in the suggested approach and synchronization report
Emily Zou: Items 1 and 2 in the suggested approach

--------------------------------------------------------------------------

**SYNCHRONIZATION REPORT**

What data structures needed to be synchronized, and why?

- The 'std::map<std::string, Table*> tables' data structure needed to be synchronized because
multiple client threads might try to concurrently create new tables with the 'CREATE' request
or access already existing tables with the 'GET' request. Without synchronizing all accesses to
the 'tables' map, concurrent requests would lead to a table being created at the same time as another 
client thread is reading/accessing the map (race conditions).
- Every 'Table' object needed to be synchronized because it is shared with multiple clients, so 
concurrent requests on the table, like reading a value using 'GET' or writing a value using 'SET', 
have to be protected. If they aren't protected, we run into the risk of having inconsistent states
for the Table since one client thread could be making changes while another is rolling back changes
in the same table (each 'Table' has its own 'pthread_mutex_t' for consistency).

************

How did you synchronize the data structures requiring synchronization?

- I added 'pthread_mutex_t mutex_for_tables' in the Server class and ensured that whenever the server
needed to access the 'tables' map (either for create_table or find_table), 'mutex_for_tables' is locked first.
After the server accesses the 'tables' map and performs an operation/command, the server unlocks 'mutex_for_tables'.
This prevents lost updates or inconsistent states (race conditions) since only one client thread at a time can safely
create a new table into the map or read from it. 
- For autocommit mode, when a request accesses a table such as in GET or SET, they must be locked before the request is carried out and after the request is finished.
To ensure this, the requests first retrieve a pointer to the table. If the client is in autocommit mode, the request will automatically lock and 
unlock the table using the table's 'pthread_mutex_lock' and 'pthread_mutex_unlock' using the table's lock() and unlock() functions. This must happen
upon starting and ending the request. 
- For transaction mode, I used 'pthread_mutex_trylock' for locking tables in order to prevent deadlocks. When the trylock fails
because another transaction is holding the lock, the current transaction fails, all changes are rolled back, and a 'FAILED' response 
is given to the client. By doing this, indefinite blocking for a lock already in transaction mode doesn't occur, which prevents cyclic 
scenarios from leading to a deadlock.

************

Why are you confident that the server is free of race conditions and deadlocks?

- In autocommit mode, each request is treated as a singular transaction. Therefore, once the request is over, lock is immediately released.
This prevents one client from holding a lock over multiple requests, effectively avoiding deadlocks. There is no opportunity for a deadlock
to occur since each time one client holds a lock, the other clients must wait for that client to finish their request before continuing.
- None of the transactions are nested, which reduces the complexity of the lock retrieval patterns, preventing deadlock.
- The code rolls back on a failure and unlocks all the tables. This ensures that incomplete updates to the table are not
processed (preventing inconsistent states) and that the tables are locked only for a while (indefinite lock).
- With 'pthread_mutex_trylock' in transaction mode, the transaction fails, and changes are rolled back if a lock 
cannot be found right away. This prevents a deadlock because it ensures clients do not wait for a lock that belongs to another client. 
- All important data structures are protected with a mutex, which prevents two client threads from reading/modifying data simultaneously (without sync). 


