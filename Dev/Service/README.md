Use the API
I might code it in c++

''Public Functions'':
main(argc, *argv)					: ''Main Service Loop'' - note: using pthreads is a nice option

''Private Functions'':
get()								: Get data from file or pipe or socket or ...
put()								: Put data in file or pipe or socket or ...
...listener mechanic...


Note:
Is a service that should constantly run of the BBB.
It should be included in the BBControllerDaemon?
The listener should be implemented in this service