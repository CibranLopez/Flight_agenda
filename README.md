# Flight_agenda

Here an implementation of Dijkstra algorithm can be found, devoted to solve the minimal path between three different airports.

In order to execute it, 'asprintf' function has to be available. Since it is part of GNU, it may be already available for some GNU versions. Otherwise, including '#define _GNU_SOURCE' may be needed.

Then, the file with all the nodes and ways (the so-called 'Flight201301u.csv') has to be in the same folder as the executable of 'flight_agenda_algorithm.c', (compiled with 'auxiliar_functions.h', where all the needed functions are defined). The output will be printed, in which we can find each airport name, and the arrival and departure times.
