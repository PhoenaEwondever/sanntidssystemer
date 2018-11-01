# Miniproject
Extract the files to the same folder as you have your buildroot folder and run ```make```. This compils all .c files into an executable called main. Copy this executable to the desired location on the NGW1000. 

You can also run ```make start``` to compile, copy the file to _/export/nfs/home/avr32/_ and start the _miniporject-sever_.

## Threading
We have chosen to use three threads to solve this task. This is mainly due to the recommendation we got from the problem text. It also seemed logical. Each thread has its own responsibility, one for receiving messages from the NGW100, one for controlling the dynamic system and one for handling signals. 

We chose to use two semaphores for communication between the threads: ```y_sem``` and ```signal_sem```. When the controller thread sends a ```GET``` request it _waits_ ```y_sem``` until the receiving thread gets the response message, parses and stores it and calls _post_ on the semaphore. The same method is used for signals. A _post_ is called on ```signal_sem``` by the receiving thread whenever a signal is received. The thread responsible for handling signals is waiting for a ..._wait_ and then sens a ```SIGNAL:ACK``` to the NGW100.

## Controller
We use 5 ms for our controller period. This was found with old fashioned parameter tuning (trial and error). We got a good response time on our graph. We also tested it by tuning up and down the values. This resulted in our system oscilating and giving incorrect calculations. 

Our parameters are the same as those recomended by the problem text. They worked fine. No need to change a winning team. 

## Plot analysis
When not handling the signals our period timings were pretty much on point with a distribution with a low variance around 5000us (5ms). For the controller step time response we got a nice low value at around 2000us. 

When introducing the signal handling we notices a greater variance in the period timings. It still is centered around 5000us. While this did not impact the performance of the controller as seen from the graph.

The response time for the signal handling seems decent.
