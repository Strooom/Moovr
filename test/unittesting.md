In order to maximize the quality of the Moovr software, during development it is being tested with a battery of unitTests which can run automatically.
Unit tests can be of two types:
* native, ie. they are platform independent, and so they can run on the development PC or even on a virtual machine on github actions
* target : they are hardware specific

The objective is to minimize the hardware dependencies, and to maximize the coverage with native unit tests, as they can run automated.