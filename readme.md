# Miniproject

## Threading
We have chosen to use three threads to solve the task. This is mainly due to the recommendation we got from the problem text. It also seemed logical. Each thread has its own responsibility, one for receiving messages from the NGW100, one for controlling the dynamic system and one for handling signals. 

We chose to use semaphores for communication between the threads. When the controller thread sends a ```GET``` request it waits the ```y_sem``` until the receiving thread gets the response message, parses and stores it and calls post on the semaphore. The same method was used on the signals. A post was called on the ```signal_sem``` by the receiving thread whenever a signal was received. The thread responsible for handling signals was waiting for a ...wait and then sent a ```SIGNAL:ACK```.

## Controller
We used 5 ms for our controller period. This was found with old fashioned trial and error. We got a good response time on our graph. We also tested it by tuning up and down the values. This resulted in our system oscilating and giving incorrect calculations. 

Our parameters were the same as those recomended by the problem text. They worked fine. No need to change a winning team. 

## Plot analysis
When not handling the signals our period timings were pretty much on point with a distribution with a low variance around 5000us (5ms). For the controller step time response we got a nice low value at around 2000us. 

When introducing the signal handling we notices a greater variance in the period timings. It still is centered around 5000us. While this did not impact the performance of the controller. 

The response time for the signal handling seems decent.
